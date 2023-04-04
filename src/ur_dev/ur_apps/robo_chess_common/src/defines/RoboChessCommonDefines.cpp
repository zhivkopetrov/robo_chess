//Corresponding header
#include "robo_chess_common/defines/RoboChessCommonDefines.h"

//System headers

//Other libraries headers
#include "utils/data_type/EnumClassUtils.h"
#include "utils/Log.h"

//Own components headers

std::string toString(AbortMotion type) {
  switch (type) {
  case AbortMotion::GRACEFUL_STOP:
    return "AbortMotion::GRACEFUL_STOP";
  case AbortMotion::IMMEDIATE_ABORT:
    return "AbortMotion::IMMEDIATE_ABORT";
  default:
    LOGERR("Error, received unsupported AbortMotion: %d. Defaulting to "
           "AbortMotion::UNKNOWN", getEnumValue(type));
    return "AbortMotion::UNKNOWN";
  }
}

std::string toString(ChessMoveType type) {
  switch (type) {
  case ChessMoveType::NON_CAPTURE_MOVE:
    return "AbortMotion::NON_CAPTURE_MOVE";
  case ChessMoveType::CAPTURE_MOVE:
    return "AbortMotion::CAPTURE_MOVE";
  default:
    LOGERR("Error, received unsupported ChessMoveType: %d. Defaulting to "
           "ChessMoveType::UNKNOWN", getEnumValue(type));
    return "ChessMoveType::UNKNOWN";
  }
}

std::string toString(ParkMode type) {
  switch (type) {
  case ParkMode::AUTOMATIC:
    return "ParkMode::AUTOMATIC";
  case ParkMode::MANUAL:
    return "ParkMode::MANUAL";
  default:
    LOGERR("Error, received unsupported ParkMode: %d. Defaulting to "
           "ParkMode::UNKNOWN", getEnumValue(type));
    return "ParkMode::UNKNOWN";
  }
}