#ifndef ROBO_CHESS_CHESSMOVEMOTIONSEQUENCECONFIG_H_
#define ROBO_CHESS_CHESSMOVEMOTIONSEQUENCECONFIG_H_

//System headers
#include <cstdint>

//Other libraries headers
#include "urscript_common/motion/MotionStructs.h"

//Own components headers

//Forward declarations

struct ChessPieceDimensions {
  double width { };
  double depth { };
  double height { };
};

struct ChessMoveMotionSequenceConfig {
  ChessPieceDimensions chessPieceDimensions;
  int32_t gripperOpening = 0; // [mm]

  AngleAxis zeroOrientation;   //neutral orientation
  AngleAxis ninetyOrientation; //neutral orientation + 90 deg on wrist_3

  double motionAcc = 0.0; //[m/s2]
  double motionVel = 0.0; //[m/s]

  WaypointJoint graspApproachJoint;

  WaypointCartesian graspApproachCartesian;
};

#endif /* ROBO_CHESS_CHESSMOVEMOTIONSEQUENCECONFIG_H_ */
