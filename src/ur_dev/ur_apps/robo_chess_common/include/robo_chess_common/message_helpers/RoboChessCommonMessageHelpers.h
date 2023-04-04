#ifndef ROBO_CHESS_COMMON_ROBOCHESSCOMMONMESSAGEHELPERS_H_
#define ROBO_CHESS_COMMON_ROBOCHESSCOMMONMESSAGEHELPERS_H_

//System headers
#include <cstdint>

//Other libraries headers
#include "robo_chess_common/defines/RoboChessCommonDefines.h"

//Own components headers

//Forward declarations

int32_t getAbortMotionValue(AbortMotion type);
AbortMotion getAbortMotion(int32_t typeValue);

int32_t getChessMoveTypeValue(ChessMoveType type);
ChessMoveType getChessMoveType(int32_t typeValue);

int32_t getParkModeValue(ParkMode mode);
ParkMode getParkMode(int32_t modeValue);

#endif /* ROBO_CHESS_COMMON_ROBOCHESSCOMMONMESSAGEHELPERS_H_ */
