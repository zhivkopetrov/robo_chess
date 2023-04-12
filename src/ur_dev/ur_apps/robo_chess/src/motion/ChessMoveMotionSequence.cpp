//Corresponding header
#include "robo_chess/motion/ChessMoveMotionSequence.h"

//System headers

//Other libraries headers
#include "urscript_common/motion/MotionUtils.h"
#include "utils/Log.h"

//Own components headers
#include "robo_chess/defines/RoboChessDefines.h"

ChessMoveMotionSequence::ChessMoveMotionSequence(
  const ChessMoveMotionSequenceConfig& cfg, const std::string& name, int32_t id, 
  const std::shared_ptr<UrScriptBuilder>& urScriptBuilder) 
  : MotionSequence(name, id, urScriptBuilder), _cfg(cfg) {

}

void ChessMoveMotionSequence::start(const UrscriptsBatchDoneCb& cb) {
  const auto commands = generateFullPickAndPlaceCommandCycle();
  dispatchUscriptsAsyncCb(commands, cb);
}

void ChessMoveMotionSequence::gracefulStop(const UrscriptsBatchDoneCb& cb) {
  //MVP - reuse the ::start() implementation
  start(cb);
}

void ChessMoveMotionSequence::recover(const UrscriptsBatchDoneCb& cb) {
  //MVP - reuse the ::start() implementation
  start(cb);
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
      _cfg.motionVel, _cfg.motionAcc, blendingRadius);
  auto graspCommand = std::make_unique<MoveLinearCommand>(
    graspPose, _cfg.motionVel, _cfg.motionAcc);
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
    constructUrScript(motion::chess_move::GRASP_NAME, cmdContainer);

  const UrscriptDoneCb doneCb = [this](){
    _state.holdingObject = true;
  };
  return { cmdPayload, doneCb };
}

UrscriptCommand ChessMoveMotionSequence::generateTransportAndPlaceCommand(
  const WaypointCartesian& currPose, const WaypointCartesian& placePose) {
  const double blendingRadius = computeSafeBlendingRadius(
    currPose.pos, _cfg.graspApproachCartesian.pos, placePose.pos);

  auto transportApproachCommand = 
    std::make_unique<MoveLinearCommand>(_cfg.graspApproachCartesian, 
      _cfg.motionVel, _cfg.motionAcc, blendingRadius);
  auto placeCommand = std::make_unique<MoveLinearCommand>(
    placePose, _cfg.motionVel, _cfg.motionAcc);
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
    motion::chess_move::TRANSPORT_AND_PLACE_NAME, cmdContainer);

  const UrscriptDoneCb doneCb = [this](){
    handleSuccessfulPlacement();
  };
  return { cmdPayload, doneCb };
}

std::vector<UrscriptCommand> 
ChessMoveMotionSequence::generateFullPickAndPlaceCommandCycle() {
  return {};
}

void ChessMoveMotionSequence::handleSuccessfulPlacement() {
  _state.holdingObject = false;
}
