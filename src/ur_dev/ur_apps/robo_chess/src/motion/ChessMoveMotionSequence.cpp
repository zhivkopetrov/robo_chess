//Corresponding header
#include "robo_chess/motion/ChessMoveMotionSequence.h"

//System headers

//Other libraries headers
#include "urscript_common/motion/MotionUtils.h"
#include "utils/Log.h"

//Own components headers

ChessMoveMotionSequence::ChessMoveMotionSequence(
  const ChessMoveMotionSequenceConfig& cfg, const std::string& name, int32_t id, 
  const std::shared_ptr<UrScriptBuilder>& urScriptBuilder) 
  : MotionSequence(name, id, urScriptBuilder), _cfg(cfg) {

}

void ChessMoveMotionSequence::start(const UrscriptsBatchDoneCb& cb) {
  auto commands = generateFullPickAndPlaceCommandCycle();
  if (ChessMoveEndStrategy::TRANSITION_TO_IDLE_STATE == _cfg.endStrategy) {
    commands.push_back(generateReturnHomeCommand());
    dispatchUscriptsAsyncCb(commands, cb);
    return;
  }

  //ChessMoveEndStrategy::SWAP_TOWERS
  //after ChessMove tower is completed, same method ::start() will be called,
  //thus the vice-versa construction will begin
  //this will continue indefinitely or until gracefully_stopped/aborted 
  const UrscriptsBatchDoneCb callSameMethodCb = [this, cb](){
    start(cb);
  };
  dispatchUscriptsAsyncCb(commands, callSameMethodCb);
}

void ChessMoveMotionSequence::gracefulStop(const UrscriptsBatchDoneCb& cb) {
  //for now the graceful_stop and recover implementations are identical
  recover(cb);
}

void ChessMoveMotionSequence::recover(const UrscriptsBatchDoneCb& cb) {
  std::vector<UrscriptCommand> commands { generateReturnHomeCommand() };
  if (_state.holdingObject) {
    const Point3d& towerCenterPos = 
      TowerDirection::A_TO_B == _state.towerDirection ?
        _cfg.baseCenterBCartesian.pos : _cfg.baseCenterACartesian.pos;
    const WaypointCartesian placeWaypoint = 
      computeObjectPose(towerCenterPos, _state.currentObjectIdx);

    commands.push_back(
      generateTransportAndPlaceCommand(_cfg.homeCartesian, placeWaypoint));
  }
  commands.push_back(generateReturnHomeAndOpenGripperCommand());

  dispatchUscriptsAsyncCb(commands, cb);
}

ErrorCode ChessMoveMotionSequence::setTransportStrategy(int32_t strategyId) {
  LOG("No-op operation setTransportStrategy:[%d] for ChessMoveMotionSequence", 
      strategyId);
  return ErrorCode::SUCCESS;
}

UrscriptCommand ChessMoveMotionSequence::generateGraspCommand(
    const WaypointCartesian& currPose, const WaypointCartesian& graspPose) {
  const double blendingRadius = computeSafeBlendingRadius(
    currPose.pos, _cfg.graspApproachCartesian.pos, graspPose.pos);

  auto graspApproachCommand = 
    std::make_unique<MoveLinearCommand>(_cfg.graspApproachCartesian, 
      _cfg.pickAndPlaceVel, _cfg.pickAndPlaceAcc, blendingRadius);
  auto graspCommand = std::make_unique<MoveLinearCommand>(
    graspPose, _cfg.pickAndPlaceVel, _cfg.pickAndPlaceAcc);
  constexpr int32_t gripperSpeedPercent = 20;
  auto gripperSpeedCommand = std::make_unique<GripperParamCommand>(
    GripperParamType::SPEED, gripperSpeedPercent);
  constexpr int32_t gripperForcePercent = 50;
  auto gripperForceCommand = std::make_unique<GripperParamCommand>(
    GripperParamType::FORCE, gripperForcePercent);
  auto closeGripperCommand = 
    std::make_unique<GripperActuateCommand>(GripperActuateType::CLOSE);

  UrScriptCommandContainer cmdContainer;
  cmdContainer.addCommand(std::move(graspApproachCommand))
              .addCommand(std::move(graspCommand))
              .addCommand(std::move(gripperSpeedCommand))
              .addCommand(std::move(gripperForceCommand))
              .addCommand(std::move(closeGripperCommand));
  const UrScriptPayload cmdPayload = 
    constructUrScript(Motion::ChessMove::GRASP_NAME, cmdContainer);

  const UrscriptDoneCb doneCb = [this](){
    _state.holdingObject = true;
    serializeState();
  };
  return { cmdPayload, doneCb };
}

UrscriptCommand ChessMoveMotionSequence::generateTransportAndPlaceCommand(
  const WaypointCartesian& currPose, const WaypointCartesian& placePose) {
  const double blendingRadius = computeSafeBlendingRadius(
    currPose.pos, _cfg.graspApproachCartesian.pos, placePose.pos);

  auto transportApproachCommand = 
    std::make_unique<MoveLinearCommand>(_cfg.graspApproachCartesian, 
      _cfg.pickAndPlaceVel, _cfg.pickAndPlaceAcc, blendingRadius);
  auto placeCommand = std::make_unique<MoveLinearCommand>(
    placePose, _cfg.pickAndPlaceVel, _cfg.pickAndPlaceAcc);
  constexpr int32_t gripperSpeedPercent = 20;
  auto gripperSpeedCommand = std::make_unique<GripperParamCommand>(
    GripperParamType::SPEED, gripperSpeedPercent);
  constexpr int32_t gripperForcePercent = 50;
  auto gripperForceCommand = std::make_unique<GripperParamCommand>(
    GripperParamType::FORCE, gripperForcePercent);
  auto openGripperCommand = 
    std::make_unique<GripperPreciseActuateCommand>(_cfg.gripperOpening);

  UrScriptCommandContainer cmdContainer;
  cmdContainer.addCommand(std::move(transportApproachCommand))
              .addCommand(std::move(placeCommand))
              .addCommand(std::move(gripperSpeedCommand))
              .addCommand(std::move(gripperForceCommand))
              .addCommand(std::move(openGripperCommand));
  const UrScriptPayload cmdPayload = constructUrScript(
    Motion::ChessMove::TRANSPORT_AND_PLACE_NAME, cmdContainer);

  const UrscriptDoneCb doneCb = [this](){
    handleSuccessfulPlacement();
    serializeState();
  };
  return { cmdPayload, doneCb };
}

UrscriptCommand ChessMoveMotionSequence::generateReturnHomeCommand() {
  auto returnHomeCommand = std::make_unique<MoveJointCommand>(_cfg.homeJoint);

  UrScriptCommandContainer cmdContainer;
  cmdContainer.addCommand(std::move(returnHomeCommand));
  const UrScriptPayload cmdPayload = 
    constructUrScript(Motion::ChessMove::RETURN_HOME_NAME, cmdContainer);

  return { cmdPayload };
}

UrscriptCommand ChessMoveMotionSequence::generateReturnHomeAndOpenGripperCommand() {
  auto returnHomeCommand = std::make_unique<MoveJointCommand>(_cfg.homeJoint);
  auto openGripperCommand = 
    std::make_unique<GripperPreciseActuateCommand>(_cfg.gripperOpening);

  UrScriptCommandContainer cmdContainer;
  cmdContainer.addCommand(std::move(openGripperCommand))
              .addCommand(std::move(returnHomeCommand));
  const UrScriptPayload cmdPayload = constructUrScript(
    Motion::ChessMove::RETURN_HOME_AND_OPEN_GRIPPER_NAME, cmdContainer);

  return { cmdPayload };
}

std::vector<UrscriptCommand> 
ChessMoveMotionSequence::generateFullPickAndPlaceCommandCycle() {
  std::vector<UrscriptCommand> commands;
  //reserve enough memory for all urscripts
  constexpr int32_t urscriptsPerBlock = 2;
  constexpr int32_t returnHomeUrscripts = 1;
  const int32_t blocksLeftFromCurrTower = 
    _cfg.totalObjectsPerTower - _state.currentObjectIdx;
  commands.reserve(
    (blocksLeftFromCurrTower * urscriptsPerBlock) + returnHomeUrscripts);

  const auto [graspTowerCenterPos, placeTowerCenterPos] = [this](){
    return TowerDirection::A_TO_B == _state.towerDirection ?
      std::make_pair(
        _cfg.baseCenterACartesian.pos, _cfg.baseCenterBCartesian.pos) :
      std::make_pair(
        _cfg.baseCenterBCartesian.pos, _cfg.baseCenterACartesian.pos);
  }();

  WaypointCartesian graspWaypoint;
  WaypointCartesian placeWaypoint;
  for (int32_t objIdx = _state.currentObjectIdx; 
       objIdx < _cfg.totalObjectsPerTower; ++objIdx) {
    //the indexes for grasping/placing should be mirrored
    graspWaypoint = computeObjectPose(graspTowerCenterPos, 
      _cfg.totalObjectsPerTower - objIdx - 1);
    placeWaypoint = computeObjectPose(placeTowerCenterPos, objIdx);

    commands.push_back(generateGraspCommand(placeWaypoint, graspWaypoint));
    commands.push_back(
      generateTransportAndPlaceCommand(graspWaypoint, placeWaypoint));
  }

  return commands;
}

void ChessMoveMotionSequence::handleSuccessfulPlacement() {
  std::string towerStr = TowerDirection::A_TO_B == _state.towerDirection ? 
    TOWER_DIR_A_TO_B_STR : TOWER_DIR_B_TO_A_STR;
  LOGG("ChessMove successful placement for Block[%d], Floor[%d], TowerDir: [%s]", 
       _state.currentObjectIdx, _state.currentObjectIdx / 2, towerStr.c_str());

  _state.holdingObject = false;
  ++_state.currentObjectIdx;
  //swap towers
  if (_state.currentObjectIdx >= _cfg.totalObjectsPerTower) {
    _state.currentObjectIdx = 0;
    _state.towerDirection = TowerDirection::A_TO_B == _state.towerDirection ?
      TowerDirection::B_TO_A : TowerDirection::A_TO_B;

    towerStr = TowerDirection::A_TO_B == _state.towerDirection ? 
      TOWER_DIR_A_TO_B_STR : TOWER_DIR_B_TO_A_STR;
    LOGM("Swapping towers. New TowerDir: [%s]", towerStr.c_str());
  }
}
