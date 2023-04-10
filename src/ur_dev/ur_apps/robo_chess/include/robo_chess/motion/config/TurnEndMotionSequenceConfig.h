#ifndef ROBO_CHESS_TURNENDMOTIONSEQUENCECONFIG_H_
#define ROBO_CHESS_TURNENDMOTIONSEQUENCECONFIG_H_

//System headers

//Other libraries headers
#include "urscript_common/motion/MotionStructs.h"

//Own components headers

//Forward declarations

struct TurnEndMotionSequenceConfig {
  double motionAcc = 0.0; //[m/s2]
  double motionVel = 0.0; //[m/s]

  WaypointJoint homeJoint;
  WaypointJoint turnEndJoint;
  WaypointJoint turnEndApproachJoint;

  WaypointCartesian homeCartesian;
  WaypointCartesian turnEndCartesian;
  WaypointCartesian turnEndApproachCartesian;
};

#endif /* ROBO_CHESS_TURNENDMOTIONSEQUENCECONFIG_H_ */
