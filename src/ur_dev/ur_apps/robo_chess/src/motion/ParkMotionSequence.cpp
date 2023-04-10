//Corresponding header
#include "robo_chess/motion/ParkMotionSequence.h"

//System headers

//Other libraries headers
#include "urscript_common/motion/MotionUtils.h"
#include "utils/data_type/EnumClassUtils.h"
#include "utils/Log.h"

//Own components headers
#include "robo_chess/defines/RoboChessDefines.h"

ParkMotionSequence::ParkMotionSequence(
  const ParkMotionSequenceConfig& cfg, const std::string& name, int32_t id,
  const std::shared_ptr<UrScriptBuilder>& urScriptBuilder) : 
    MotionSequence(name, id, urScriptBuilder), _cfg(cfg) {

}

void ParkMotionSequence::start(const UrscriptsBatchDoneCb& cb) {
  const std::vector<UrscriptCommand> commands { generateReturnHomeCommand() };

  dispatchUscriptsAsyncCb(commands, cb);
}

void ParkMotionSequence::gracefulStop(const UrscriptsBatchDoneCb& cb) {
  start(cb);
}

void ParkMotionSequence::recover(const UrscriptsBatchDoneCb& cb) {
  start(cb);
}

ErrorCode ParkMotionSequence::setTransportStrategy(int32_t strategyId) {
  LOG("No-op operation setTransportStrategy:[%d] for ParkMotionSequence", 
      strategyId);
  return ErrorCode::SUCCESS;
}

UrscriptCommand ParkMotionSequence::generateReturnHomeCommand() const {
  //TODO implement Retract and return home command, because the robot might 
  //collide with the environment if go gome is directly used.
  //Instead retract vertically from current position to homeCartesian.pos.z
  //and then blend toward homeJoint
  //This would require expanding the getEefAxis API from urscript_bridge 
  auto returnHomeCommand = std::make_unique<MoveJointCommand>(_cfg.homeJoint);

  UrScriptCommandContainer cmdContainer;
  cmdContainer.addCommand(std::move(returnHomeCommand));
  const UrScriptPayload cmdPayload = 
    constructUrScript(motion::park::PARK_NAME, cmdContainer);

  return { cmdPayload };
}