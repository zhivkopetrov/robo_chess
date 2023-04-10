//Corresponding header
#include "robo_chess/motion/TurnEndMotionSequence.h"

//System headers

//Other libraries headers
#include "urscript_common/motion/MotionUtils.h"
#include "utils/data_type/EnumClassUtils.h"
#include "utils/Log.h"

//Own components headers

TurnEndMotionSequence::TurnEndMotionSequence(
  const TurnEndMotionSequenceConfig& cfg, const std::string& name, int32_t id,
  const std::shared_ptr<UrScriptBuilder>& urScriptBuilder) : 
    MotionSequence(name, id, urScriptBuilder), _cfg(cfg) {
  loadState();
}

void TurnEndMotionSequence::start(const UrscriptsBatchDoneCb& cb) {
  std::vector<UrscriptCommand> commands {
    generateGraspCommand(),
    generateGripperActuateCommand(GripperActuateType::CLOSE),
    generateTransportCommand(), 
  };

  UrscriptsBatchDoneCb batchDoneCb;
  if (TurnEndEndStrategy::PLACE_AND_RETURN_HOME == _cfg.endStrategy) {
    commands.push_back(generatePlaceCommand());
    commands.push_back(generateRetractAndReturnHomeCommand());
    batchDoneCb = cb;
  } else { //TurnEndEndStrategy::WAIT_AFTER_TRANSPORT
    batchDoneCb = [](){};
  }

  dispatchUscriptsAsyncCb(commands, batchDoneCb);
}

void TurnEndMotionSequence::gracefulStop(const UrscriptsBatchDoneCb& cb) {
  std::vector<UrscriptCommand> commands;
  UrscriptsBatchDoneCb batchDoneCb = cb;
  if (TurnEndEndStrategy::PLACE_AND_RETURN_HOME == _cfg.endStrategy) {
    commands = generateGracefullyStopPlaceAndReturnHomeStrategy();
  } else { //TurnEndEndStrategy::WAIT_AFTER_TRANSPORT
    commands = generateGracefullyStopWaitAfterTransportStrategy();
    if (_state.holdingObject && !_state.reachedTransportTargetPose) {
      //transport and wait. Ignore the UrscriptsBatchDoneCb callback
      batchDoneCb = [](){};
    }
  }

  dispatchUscriptsAsyncCb(commands, batchDoneCb);
}

void TurnEndMotionSequence::recover(const UrscriptsBatchDoneCb& cb) {
  //first return home to ensure collision free recovery
  std::vector<UrscriptCommand> commands { generateReturnHomeCommand() };

  //reuse the gracefully stopping logic
  std::vector<UrscriptCommand> gracefullyStopCommands;
  UrscriptsBatchDoneCb batchDoneCb = cb;
  if (TurnEndEndStrategy::PLACE_AND_RETURN_HOME == _cfg.endStrategy) {
    gracefullyStopCommands = generateGracefullyStopPlaceAndReturnHomeStrategy();
  } else { //TurnEndEndStrategy::WAIT_AFTER_TRANSPORT
    gracefullyStopCommands = generateGracefullyStopWaitAfterTransportStrategy();
    if (_state.holdingObject && !_state.reachedTransportTargetPose) {
      //transport and wait. Ignore the UrscriptsBatchDoneCb callback
      batchDoneCb = [](){};
    }
  }

  commands.insert(
    commands.end(),
    std::make_move_iterator(gracefullyStopCommands.begin()),
    std::make_move_iterator(gracefullyStopCommands.end())
  );

  dispatchUscriptsAsyncCb(commands, batchDoneCb);
}

ErrorCode TurnEndMotionSequence::setTransportStrategy(int32_t strategyId) {
  if ((getEnumValue(Motion::TurnEnd::TransportStrategy::BASIC) > strategyId) || 
      (getEnumValue(Motion::TurnEnd::TransportStrategy::TWIST) < strategyId)) {
    LOGERR("Received unsupported TransportStrategy:[%d]", strategyId);
    return ErrorCode::FAILURE;
  }

  _transportStrategy = toEnum<Motion::TurnEnd::TransportStrategy>(strategyId);
  return ErrorCode::SUCCESS;
}

UrscriptCommand TurnEndMotionSequence::generateGraspCommand() {
  //NOTE: even though MoveJointCommand waypoints are used
  //their respective Cartesian counterparts are used to compute blending radius
  const double blendingRadius = computeSafeBlendingRadius(
    _cfg.homeCartesian.pos, _cfg.graspApproachCartesian.pos, 
    _cfg.graspCartesian.pos);

  auto graspApproachCommand = std::make_unique<MoveJointCommand>(
    _cfg.graspApproachJoint, _cfg.pickAndPlaceVel, _cfg.pickAndPlaceAcc, 
    blendingRadius);
  auto graspCommand = std::make_unique<MoveJointCommand>(_cfg.graspJoint);

  UrScriptCommandContainer cmdContainer;
  cmdContainer.addCommand(std::move(graspApproachCommand))
              .addCommand(std::move(graspCommand));

  const UrScriptPayload cmdPayload = 
    constructUrScript(Motion::TurnEnd::GRASP_NAME, cmdContainer);
  const UrscriptDoneCb doneCb = [this](){
    _state.holdingObject = true;
    serializeState();
  };
  return { cmdPayload, doneCb };
}

UrscriptCommand TurnEndMotionSequence::generatePlaceCommand() {
  auto placeCommand = std::make_unique<MoveLinearCommand>(_cfg.placeCartesian);

  UrScriptCommandContainer cmdContainer;
  cmdContainer.addCommand(std::move(placeCommand));

  const UrScriptPayload cmdPayload = 
    constructUrScript(Motion::TurnEnd::PLACE_NAME, cmdContainer);
  const UrscriptDoneCb doneCb = [this](){
    _state.holdingObject = false;
    _state.reachedTransportTargetPose = false;
    serializeState();
  };
  return { cmdPayload, doneCb };
}

UrscriptCommand TurnEndMotionSequence::generateHandoverCommand() {
  UrscriptCommand cmd = generateGripperActuateCommand(GripperActuateType::OPEN);

  const UrscriptDoneCb doneCb = [this](){
    _state.holdingObject = false;
    _state.reachedTransportTargetPose = false;
    serializeState();
  };
  cmd.doneCb = doneCb;
  return cmd;
}

UrscriptCommand TurnEndMotionSequence::generateTransportCommand() {
  UrScriptCommandContainer cmdContainer;
  TransportMoveCommands transportMoveCommands = 
    generateTransportMoveCommands(_transportStrategy);
  if (transportMoveCommands.empty()) { //sanity check
    LOGERR("Internal error, generated empty TransportMoveCommands");
    return { };
  }

  std::string_view methodName;
  if (TurnEndEndStrategy::WAIT_AFTER_TRANSPORT == _cfg.endStrategy) {
    //last trajectory waypoint should have zero blending
    MoveCommandBase& lastCmd = *(transportMoveCommands.back());
    lastCmd.blendingRadius = 0.0; //[m]
    methodName = Motion::TurnEnd::TRANSPORT_AND_WAIT_NAME;
  } else { //TurnEndEndStrategy::PLACE_AND_RETURN_HOME
    methodName = Motion::TurnEnd::TRANSPORT_NAME;
  }

  for (auto& moveCommand : transportMoveCommands) {
    cmdContainer.addCommand(std::move(moveCommand));
  }
  const UrScriptPayload cmdPayload = 
    constructUrScript(methodName, cmdContainer);
  const UrscriptDoneCb doneCb = [this](){
    _state.reachedTransportTargetPose = true;
    serializeState();
  };
  return { cmdPayload, doneCb };
}

UrscriptCommand 
TurnEndMotionSequence::generateRetractAndReturnHomeCommand() const {
  //NOTE: manually restrict the level of blending radius
  //Even though a bigger blending radius could be achieved, this might result 
  //in collision with the container that the rose will be placed into
  constexpr double blendingRadius = 0.10; //[m]
  auto placeRetractCommand = std::make_unique<MoveLinearCommand>(
    _cfg.placeApproachCartesian, _cfg.pickAndPlaceVel, _cfg.pickAndPlaceAcc, 
    blendingRadius);
  auto returnHomeCommand = std::make_unique<MoveJointCommand>(_cfg.homeJoint);

  UrScriptCommandContainer cmdContainer;
  cmdContainer.addCommand(std::move(placeRetractCommand))
              .addCommand(std::move(returnHomeCommand));
  const UrScriptPayload cmdPayload = constructUrScript(
    Motion::TurnEnd::RETRACT_AND_RETURN_HOME_NAME, cmdContainer);

  return { cmdPayload };
}

UrscriptCommand TurnEndMotionSequence::generateReturnHomeCommand() const {
  auto returnHomeCommand = std::make_unique<MoveJointCommand>(_cfg.homeJoint);

  UrScriptCommandContainer cmdContainer;
  cmdContainer.addCommand(std::move(returnHomeCommand));
  const UrScriptPayload cmdPayload =
    constructUrScript(Motion::TurnEnd::RETURN_HOME_NAME, cmdContainer);

  return { cmdPayload };
}

UrscriptCommand TurnEndMotionSequence::generateGripperActuateCommand(
  GripperActuateType type) const {
  constexpr int32_t gripperSpeedPercent = 100;
  auto gripperSpeedCommand = std::make_unique<GripperParamCommand>(
    GripperParamType::SPEED, gripperSpeedPercent);
  constexpr int32_t gripperForcePercent = 50;
  auto gripperForceCommand = std::make_unique<GripperParamCommand>(
    GripperParamType::FORCE, gripperForcePercent);
  auto openGripperCommand = std::make_unique<GripperActuateCommand>(type);

  UrScriptCommandContainer cmdContainer;
  cmdContainer.addCommand(std::move(gripperSpeedCommand))
              .addCommand(std::move(gripperForceCommand))
              .addCommand(std::move(openGripperCommand));

  const std::string_view methodName = (GripperActuateType::OPEN == type) ? 
    Motion::TurnEnd::OPEN_GRIPPER_NAME : Motion::TurnEnd::CLOSE_GRIPPER_NAME;
  const UrScriptPayload cmdPayload = 
    constructUrScript(methodName, cmdContainer);
  return { cmdPayload };
}

TransportMoveCommands TurnEndMotionSequence::generateTransportMoveCommands(
  Motion::TurnEnd::TransportStrategy strategy) const {
  switch (strategy) {
  case Motion::TurnEnd::TransportStrategy::BASIC:
    return generateBasicTransportMoveCommands();

  case Motion::TurnEnd::TransportStrategy::FULL_ROTATION:
    return generateFullRotationTransportMoveCommands();

  case Motion::TurnEnd::TransportStrategy::TWIST:
    return generateTwistTransportMoveCommands();
  
  default:
    LOGERR("Error, received unsupported TransportStrategy: [%d]", 
           getEnumValue(strategy));
    break;
  }
}

TransportMoveCommands 
TurnEndMotionSequence::generateBasicTransportMoveCommands() const {
  TransportMoveCommands cmds;
  constexpr double blendingRadius = 0.10; //[m]
  auto graspApproachCommand = std::make_unique<MoveJointCommand>(
    _cfg.graspApproachJoint, _cfg.pickAndPlaceVel, _cfg.pickAndPlaceAcc, 
    blendingRadius);

  auto placeApproachCommand = std::make_unique<MoveJointCommand>(
    _cfg.placeApproachBasicStrategyJoint, _cfg.pickAndPlaceVel, 
    _cfg.pickAndPlaceAcc, blendingRadius);

  cmds.push_back(std::move(graspApproachCommand));
  cmds.push_back(std::move(placeApproachCommand));
  return cmds;
}

TransportMoveCommands 
TurnEndMotionSequence::generateFullRotationTransportMoveCommands() const {
  TransportMoveCommands cmds;
  constexpr double blendingRadius = 0.10; //[m]
  auto graspApproachCommand = std::make_unique<MoveJointCommand>(
    _cfg.graspApproachJoint, _cfg.pickAndPlaceVel, _cfg.pickAndPlaceAcc, 
    blendingRadius);

  auto placeApproachCommand = std::make_unique<MoveJointCommand>(
    _cfg.placeApproachFullRotationStrategyJoint, _cfg.pickAndPlaceVel, 
    _cfg.pickAndPlaceAcc, blendingRadius);
    
  cmds.push_back(std::move(graspApproachCommand));
  cmds.push_back(std::move(placeApproachCommand));
  return cmds;
}

TransportMoveCommands 
TurnEndMotionSequence::generateTwistTransportMoveCommands() const {
  TransportMoveCommands cmds;
  constexpr double blendingRadius = 0.30; //[m]
  //NOTE: 
  //graspApproachJoint is accounted for in the twistStrategyWaypointOneJoint
  auto twistStrategyWaypointOneJointCommand = 
    std::make_unique<MoveJointCommand>(_cfg.twistStrategyWaypointOneJoint, 
    _cfg.pickAndPlaceVel, _cfg.pickAndPlaceAcc, blendingRadius);

  auto twistStrategyWaypointTwoJointCommand = 
    std::make_unique<MoveJointCommand>(_cfg.twistStrategyWaypointTwoJoint, 
    _cfg.pickAndPlaceVel, _cfg.pickAndPlaceAcc, blendingRadius);

  auto twistStrategyWaypointThreeJointCommand = 
    std::make_unique<MoveJointCommand>(_cfg.twistStrategyWaypointThreeJoint, 
    _cfg.pickAndPlaceVel, _cfg.pickAndPlaceAcc, blendingRadius);

  auto twistStrategyWaypointFourthJoint = 
    std::make_unique<MoveJointCommand>(_cfg.twistStrategyWaypointFourthJoint, 
    _cfg.pickAndPlaceVel, _cfg.pickAndPlaceAcc, blendingRadius);
    
  cmds.push_back(std::move(twistStrategyWaypointOneJointCommand));
  cmds.push_back(std::move(twistStrategyWaypointTwoJointCommand));
  cmds.push_back(std::move(twistStrategyWaypointThreeJointCommand));
  cmds.push_back(std::move(twistStrategyWaypointFourthJoint));
  return cmds;
}

std::vector<UrscriptCommand> 
TurnEndMotionSequence::generateGracefullyStopPlaceAndReturnHomeStrategy() {
  std::vector<UrscriptCommand> commands;
  if (_state.holdingObject) {
    commands.push_back(generateTransportCommand());
    commands.push_back(generatePlaceCommand());
    commands.push_back(
      generateGripperActuateCommand(GripperActuateType::OPEN));
    commands.push_back(generateRetractAndReturnHomeCommand());
  } else {
    commands.push_back(generateReturnHomeCommand());
    commands.push_back(
      generateGripperActuateCommand(GripperActuateType::OPEN));
  }
  return commands;
}

std::vector<UrscriptCommand> 
TurnEndMotionSequence::generateGracefullyStopWaitAfterTransportStrategy() {
  std::vector<UrscriptCommand> commands;
  if (_state.holdingObject) {
    //resume after wait
    if (_state.reachedTransportTargetPose) {
      commands.push_back(generateHandoverCommand());
      commands.push_back(generateReturnHomeCommand());
    } else {
      //transport and wait
      commands.push_back(generateTransportCommand());
    }
  } else {
    commands.push_back(generateReturnHomeCommand());
    commands.push_back(
      generateGripperActuateCommand(GripperActuateType::OPEN));
  }
  return commands;
}

void TurnEndMotionSequence::loadState() {
  const auto parseBoolValueCb = [this](const std::string& entryName){
   std::string strValue;
    const ErrorCode errCode = _stateFileHandler->getEntry(
      Motion::TurnEnd::SECTION_NAME, entryName, strValue);
    if (ErrorCode::SUCCESS != errCode) {
      LOGERR("Error trying to getEntry(): [%s] for section: [%s]. "
             "Defaulting to False", Motion::TurnEnd::HOLDING_OBJECT_ENTRY_NAME, 
            Motion::TurnEnd::SECTION_NAME);
      return false;
    }

    if (BOOL_TRUE_VALUE_STR == strValue) {
      return true;
    }
    return false;
  };

  _state.holdingObject = 
    parseBoolValueCb(Motion::TurnEnd::HOLDING_OBJECT_ENTRY_NAME);
  _state.reachedTransportTargetPose = 
    parseBoolValueCb(Motion::TurnEnd::REACHED_TRANSPORT_TARGET_POSE_ENTRY_NAME);
}

void TurnEndMotionSequence::serializeState() {
  const auto serializeBoolValueCb = [this](
    const std::string& entryName, bool value){
    const std::string valueStr = value ? 
      BOOL_TRUE_VALUE_STR : BOOL_FALSE_VALUE_STR;
    const ErrorCode errCode = _stateFileHandler->updateEntry(
      Motion::TurnEnd::SECTION_NAME, entryName, valueStr);
    if (ErrorCode::SUCCESS != errCode) {
      LOGERR("Error trying to serialize TurnEndMotionSequenceState");
    }
  };
  const std::string holdingObjStr = _state.holdingObject ? 
    BOOL_TRUE_VALUE_STR : BOOL_FALSE_VALUE_STR;
  const ErrorCode errCode = _stateFileHandler->updateEntry(
    Motion::TurnEnd::SECTION_NAME, Motion::TurnEnd::HOLDING_OBJECT_ENTRY_NAME, 
    holdingObjStr);
  if (ErrorCode::SUCCESS != errCode) {
    LOGERR("Error trying to serialize TurnEndMotionSequenceState");
  }

  serializeBoolValueCb(Motion::TurnEnd::HOLDING_OBJECT_ENTRY_NAME, 
    _state.holdingObject);
  serializeBoolValueCb(Motion::TurnEnd::REACHED_TRANSPORT_TARGET_POSE_ENTRY_NAME, 
    _state.reachedTransportTargetPose);
}
