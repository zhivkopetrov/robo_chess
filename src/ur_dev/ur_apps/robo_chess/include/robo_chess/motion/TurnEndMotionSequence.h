#ifndef ROBO_CHESS_TURNENDMOTIONSEQUENCE_H_
#define ROBO_CHESS_TURNENDMOTIONSEQUENCE_H_

//System headers
#include <cstdint>

//Other libraries headers
#include "ur_control_common/motion/MotionSequence.h"

//Own components headers
#include "robo_chess/motion/config/TurnEndMotionSequenceConfig.h"

//Forward declarations

class TurnEndMotionSequence final : public MotionSequence {
public:
  TurnEndMotionSequence(
    const TurnEndMotionSequenceConfig& cfg, const std::string& name, int32_t id, 
    const std::shared_ptr<UrScriptBuilder>& urScriptBuilder);

  void start(const UrscriptsBatchDoneCb& cb) override;
  void gracefulStop(const UrscriptsBatchDoneCb& cb) override;
  void recover(const UrscriptsBatchDoneCb& cb) override;
  ErrorCode setTransportStrategy(int32_t strategyId) override;

private:
  struct TurnEndState {
    bool holdingObject = false;
  };

  //==========START stateful commands===========
  UrscriptCommand generateGraspCommand();

  //used for TurnEndEndStrategy::PLACE_AND_RETURN_HOME strategy
  UrscriptCommand generatePlaceCommand();

  //used for TurnEndEndStrategy::WAIT_AFTER_TRANSPORT strategy
  UrscriptCommand generateHandoverCommand();

  UrscriptCommand generateTransportCommand();

  std::vector<UrscriptCommand> 
    generateGracefullyStopPlaceAndReturnHomeStrategy();
  std::vector<UrscriptCommand> 
    generateGracefullyStopWaitAfterTransportStrategy();
  //===========END stateful commands============

  //used for TurnEndEndStrategy::PLACE_AND_RETURN_HOME strategy
  UrscriptCommand generateRetractAndReturnHomeCommand() const;
  UrscriptCommand generateReturnHomeCommand() const;
  UrscriptCommand generateGripperActuateCommand(GripperActuateType type) const;

  TransportMoveCommands generateTransportMoveCommands(
    Motion::TurnEnd::TransportStrategy strategy) const;
  TransportMoveCommands generateBasicTransportMoveCommands() const;
  TransportMoveCommands generateFullRotationTransportMoveCommands() const;
  TransportMoveCommands generateTwistTransportMoveCommands() const;

  void loadState();
  void serializeState();

  const TurnEndMotionSequenceConfig _cfg;
  TurnEndState _state;
};

#endif /* ROBO_CHESS_TURNENDMOTIONSEQUENCE_H_ */
