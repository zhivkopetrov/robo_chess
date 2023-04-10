#ifndef ROBO_CHESS_PARKMOTIONSEQUENCE_H_
#define ROBO_CHESS_PARKMOTIONSEQUENCE_H_

//System headers
#include <cstdint>

//Other libraries headers
#include "ur_control_common/motion/MotionSequence.h"

//Own components headers
#include "robo_chess/motion/config/ParkMotionSequenceConfig.h"

//Forward declarations

class ParkMotionSequence final : public MotionSequence {
public:
  ParkMotionSequence(
    const ParkMotionSequenceConfig& cfg, const std::string& name, int32_t id, 
    const std::shared_ptr<UrScriptBuilder>& urScriptBuilder);

  void start(const UrscriptsBatchDoneCb& cb) override;
  void gracefulStop(const UrscriptsBatchDoneCb& cb) override;
  void recover(const UrscriptsBatchDoneCb& cb) override;
  ErrorCode setTransportStrategy(int32_t strategyId) override;

private:
  UrscriptCommand generateReturnHomeCommand() const;

  const ParkMotionSequenceConfig _cfg;
};

#endif /* ROBO_CHESS_PARKMOTIONSEQUENCE_H_ */
