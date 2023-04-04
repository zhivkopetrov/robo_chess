#ifndef ROBO_CHESS_ROBOCHESSDEFINES_H_
#define ROBO_CHESS_ROBOCHESSDEFINES_H_

//System headers

//Other libraries headers

//Own components headers

//Forward declarations

enum class ParkMode {
  AUTOMATIC,
  MANUAL
};

enum class AbortMotion {
  GRACEFUL_STOP,
  IMMEDIATE_ABORT
};

enum class ChessMoveType {
  NON_CAPTURE_MOVE,
  CAPTURE_MOVE
};

#endif /* ROBO_CHESS_ROBOCHESSDEFINES_H_ */