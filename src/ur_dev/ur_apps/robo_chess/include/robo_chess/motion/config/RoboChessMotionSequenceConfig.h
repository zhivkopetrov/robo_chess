#ifndef ROBO_CHESS_ROBOCHESSMOTIONSEQUENCECONFIG_H_
#define ROBO_CHESS_ROBOCHESSMOTIONSEQUENCECONFIG_H_

//System headers

//Other libraries headers

//Own components headers
#include "robo_chess/motion/config/TurnEndMotionSequenceConfig.h"
#include "robo_chess/motion/config/ChessMoveMotionSequenceConfig.h"
#include "robo_chess/motion/config/ParkMotionSequenceConfig.h"

//Forward declarations

struct RoboChessMotionSequenceConfig {
  TurnEndMotionSequenceConfig turnEndMotionSequenceCfg;
  ChessMoveMotionSequenceConfig chessMoveMotionSequenceCfg;
  ParkMotionSequenceConfig parkMotionSequenceCfg;
};

#endif /* ROBO_CHESS_ROBOCHESSMOTIONSEQUENCECONFIG_H_ */
