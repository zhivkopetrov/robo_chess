#ifndef ROBO_CHESS_ROBOT_MODE_HANDLER_H_
#define ROBO_CHESS_ROBOT_MODE_HANDLER_H_

//System headers

//Other libraries headers
#include "ur_control_common/defines/UrControlCommonDefines.h"

//Own components headers

//Forward declarations

class RobotModeHandler {
public:
  void changeRobotMode(RobotMode mode);
  void changeSafetyMode(SafetyMode mode);

private:
  RobotMode _robotMode = RobotMode::Unknown;
  SafetyMode _safetyMode = SafetyMode::Unknown;
};

#endif /* ROBO_CHESS_ROBOT_MODE_HANDLER_H_ */
