//Corresponding header
#include "robo_chess/motion/TurnEndMotionSequence.h"

//System headers

//Other libraries headers
#include "urscript_common/motion/MotionUtils.h"
#include "utils/data_type/EnumClassUtils.h"
#include "utils/Log.h"

//Own components headers
#include "robo_chess/defines/RoboChessDefines.h"

TurnEndMotionSequence::TurnEndMotionSequence(
  const TurnEndMotionSequenceConfig& cfg, const std::string& name, int32_t id,
  const std::shared_ptr<UrScriptBuilder>& urScriptBuilder) : 
    MotionSequence(name, id, urScriptBuilder), _cfg(cfg) {
}

void TurnEndMotionSequence::start(const UrscriptsBatchDoneCb& cb) {
  std::vector<UrscriptCommand> commands {
    generateTransportAndClickCommand(),
    generateGripperActuateCommand(GripperActuateType::CLOSE),
    generateRetractCommand(), 
  };

  dispatchUscriptsAsyncCb(commands, cb);
}

void TurnEndMotionSequence::gracefulStop(const UrscriptsBatchDoneCb& cb) {
  //MVP - reuse the ::start() implementation
  start(cb);
}

void TurnEndMotionSequence::recover(const UrscriptsBatchDoneCb& cb) {
  //MVP - reuse the ::start() implementation
  start(cb);
}

ErrorCode TurnEndMotionSequence::setTransportStrategy(int32_t strategyId) {
  LOG("No-op operation setTransportStrategy:[%d] for TurnEndMotionSequence", 
      strategyId);
  return ErrorCode::SUCCESS;
}

UrscriptCommand 
TurnEndMotionSequence::generateRetractCommand() const {
  auto retractCommand = std::make_unique<MoveLinearCommand>(
    _cfg.turnEndApproachCartesian, _cfg.motionVel, _cfg.motionAcc);

  UrScriptCommandContainer cmdContainer;
  cmdContainer.addCommand(std::move(retractCommand));
  const UrScriptPayload cmdPayload = constructUrScript(
    motion::turn_end::RETRACT_NAME, cmdContainer);

  return { cmdPayload };
}

UrscriptCommand TurnEndMotionSequence::generateTransportAndClickCommand() const {
  //Manually compute the blending radius, 
  //because the current robot pose is unknown.
  //It could be obtained, but that would require expanding 
  //the existing GetAngleAxis API
  constexpr double blendingRadius = 0.1; //m
  auto approachCommand = std::make_unique<MoveLinearCommand>(
    _cfg.turnEndApproachCartesian, _cfg.motionVel, _cfg.motionAcc, blendingRadius);
  auto clickCommand = std::make_unique<MoveLinearCommand>(
    _cfg.turnEndApproachCartesian, _cfg.motionVel, _cfg.motionAcc);

  UrScriptCommandContainer cmdContainer;
  cmdContainer.addCommand(std::move(approachCommand))
              .addCommand(std::move(clickCommand));
  const UrScriptPayload cmdPayload = constructUrScript(
    motion::turn_end::TRANSPORT_AND_CLICK_NAME, cmdContainer);

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
    gripper::OPEN_NAME : gripper::CLOSE_NAME;
  const UrScriptPayload cmdPayload = 
    constructUrScript(methodName, cmdContainer);
  return { cmdPayload };
}