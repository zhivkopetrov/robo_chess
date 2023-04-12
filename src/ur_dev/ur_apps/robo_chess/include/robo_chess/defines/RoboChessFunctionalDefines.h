#ifndef ROBO_CHESS_ROBOCHESSFUNCTIONALDEFINES_H_
#define ROBO_CHESS_ROBOCHESSFUNCTIONALDEFINES_H_

//System headers
#include <string>
#include <functional>

//Other libraries headers
#include "robo_chess_common/defines/RoboChessCommonDefines.h"

//Own components headers

//Forward declarations

using AbortMotionCb = std::function<void(AbortMotion typ)>;
using ChessMoveCb = std::function<void(ChessMoveType type, 
  const std::string& currPos, const std::string& futurePos)>;
using ParkCb = std::function<void()>;
using SetParkModeCb = std::function<void(ParkMode type)>;

#endif /* ROBO_CHESS_ROBOCHESSFUNCTIONALDEFINES_H_ */
