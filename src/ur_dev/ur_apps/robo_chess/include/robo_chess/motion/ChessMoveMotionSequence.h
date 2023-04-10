#ifndef ROBO_CHESS_CHESSMOVEMOTIONSEQUENCE_H_
#define ROBO_CHESS_CHESSMOVEMOTIONSEQUENCE_H_

//System headers
#include <cstdint>

//Other libraries headers
#include "ur_control_common/motion/MotionSequence.h"

//Own components headers
#include "robo_chess/motion/config/ChessMoveMotionSequenceConfig.h"

//Forward declarations

class ChessMoveMotionSequence final : public MotionSequence {
public:
  ChessMoveMotionSequence(
    const ChessMoveMotionSequenceConfig& cfg, const std::string& name, int32_t id,
    const std::shared_ptr<UrScriptBuilder>& urScriptBuilder,
    const std::shared_ptr<StateFileHandler>& stateFileHandler);

  void start(const UrscriptsBatchDoneCb& cb) override;
  void gracefulStop(const UrscriptsBatchDoneCb& cb) override;
  void recover(const UrscriptsBatchDoneCb& cb) override;
  ErrorCode setTransportStrategy(int32_t strategyId) override;

private:
  //NOTE: both current and future poses are needed to precompute blending radius
  UrscriptCommand generateGraspCommand(
    const WaypointCartesian& currPose, const WaypointCartesian& graspPose);
  UrscriptCommand generateTransportAndPlaceCommand(
    const WaypointCartesian& currPose, const WaypointCartesian& placePose);

  UrscriptCommand generateReturnHomeCommand();
  UrscriptCommand generateReturnHomeAndOpenGripperCommand();

  //Generates a full pick and place plan for the current ChessMove tower sequence.
  //This could be a partial sequence if the ChessMove sequence was stopped/aborte.
  //Or it could be a complete ChessMove sequence.
  std::vector<UrscriptCommand> generateFullPickAndPlaceCommandCycle();

  void handleSuccessfulPlacement();

  WaypointCartesian computeObjectPose(
    const Point3d& towerCenter, int32_t objectIdx) const;

  void loadState();
  void serializeState();

  enum class TowerDirection {
    A_TO_B,
    B_TO_A
  };

  struct ChessMoveState {
    int32_t currentObjectIdx = 0;
    TowerDirection towerDirection = TowerDirection::A_TO_B;
    bool holdingObject = false;
  };

  const ChessMoveMotionSequenceConfig _cfg;
  ChessMoveState _state;
  std::shared_ptr<StateFileHandler> _stateFileHandler;
};

#endif /* ROBO_CHESS_CHESSMOVEMOTIONSEQUENCE_H_ */
