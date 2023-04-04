#ifndef ROBO_CHESS_COMMON_ROBOCHESSCOMMONDEFINES_H_
#define ROBO_CHESS_COMMON_ROBOCHESSCOMMONDEFINES_H_

//System headers
#include <string>

//Other libraries headers

//Own components headers

//Forward declarations

enum class AbortMotion {
  GRACEFUL_STOP,
  IMMEDIATE_ABORT,

  UNKNOWN = 255
};

enum class ChessMoveType {
  NON_CAPTURE_MOVE,
  CAPTURE_MOVE,

  UNKNOWN = 255
};

enum class ParkMode {
  AUTOMATIC,
  MANUAL,

  UNKNOWN = 255
};

std::string toString(AbortMotion type);
std::string toString(ChessMoveType type);
std::string toString(ParkMode type);

#endif /* ROBO_CHESS_COMMON_ROBOCHESSCOMMONDEFINES_H_ */