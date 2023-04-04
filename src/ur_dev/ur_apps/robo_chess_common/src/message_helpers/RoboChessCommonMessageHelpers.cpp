//Corresponding header
#include "robo_chess_common/message_helpers/RoboChessCommonMessageHelpers.h"

//System headers

//Other libraries headers
#include "robo_chess_interfaces/srv/robot_abort_motion.hpp"
#include "robo_chess_interfaces/srv/robot_chess_move.hpp"
#include "robo_chess_interfaces/srv/robot_set_park_mode.hpp"
#include "utils/data_type/EnumClassUtils.h"
#include "utils/Log.h"

//Own components headers

using robo_chess_interfaces::srv::RobotAbortMotion;
using robo_chess_interfaces::srv::RobotChessMove;
using robo_chess_interfaces::srv::RobotSetParkMode;

int32_t getAbortMotionValue(AbortMotion type) {
  switch (type) {
  case AbortMotion::GRACEFUL_STOP:
    return RobotAbortMotion::Request::GRACEFUL_STOP;
  case AbortMotion::IMMEDIATE_ABORT:
    return RobotAbortMotion::Request::IMMEDIATE_ABORT;
  default:
    LOGERR("Error, received unsupported RobotAbortMotion: %d. Defaulting to "
           "RobotAbortMotion::Request::IMMEDIATE_ABORT", getEnumValue(type));
    return RobotAbortMotion::Request::IMMEDIATE_ABORT;
  }
}

AbortMotion getAbortMotion(int32_t typeValue) {
  switch (typeValue) {
  case RobotAbortMotion::Request::GRACEFUL_STOP:
    return AbortMotion::GRACEFUL_STOP;
  case RobotAbortMotion::Request::IMMEDIATE_ABORT:
    return AbortMotion::IMMEDIATE_ABORT;
  default:
    LOGERR("Error, received unsupported AbortMotion value: %d", typeValue);
    return AbortMotion::UNKNOWN;
  }
}

int32_t getChessMoveTypeValue(ChessMoveType type) {
  switch (type) {
  case ChessMoveType::NON_CAPTURE_MOVE:
    return RobotChessMove::Request::NON_CAPTURE_MOVE;
  case ChessMoveType::CAPTURE_MOVE:
    return RobotChessMove::Request::CAPTURE_MOVE;
  default:
    LOGERR("Error, received unsupported RobotChessMove: %d. Defaulting to "
           "RobotChessMove::Request::NON_CAPTURE_MOVE", getEnumValue(type));
    return RobotChessMove::Request::NON_CAPTURE_MOVE;
  }
}

ChessMoveType getChessMoveType(int32_t typeValue) {
  switch (typeValue) {
  case RobotChessMove::Request::NON_CAPTURE_MOVE:
    return ChessMoveType::NON_CAPTURE_MOVE;
  case RobotChessMove::Request::CAPTURE_MOVE:
    return ChessMoveType::CAPTURE_MOVE;
  default:
    LOGERR("Error, received unsupported ChessMoveType value: %d", typeValue);
    return ChessMoveType::UNKNOWN;
  }
}

int32_t getParkModeValue(ParkMode mode) {
  switch (mode) {
  case ParkMode::AUTOMATIC:
    return RobotSetParkMode::Request::PARK_MODE_AUTOMATIC;
  case ParkMode::MANUAL:
    return RobotSetParkMode::Request::PARK_MODE_MANUAL;
  default:
    LOGERR("Error, received unsupported ParkMode: %d. Defaulting to "
           "RobotSetParkMode::Request::PARK_MODE_AUTOMATIC", 
           getEnumValue(mode));
    return RobotSetParkMode::Request::PARK_MODE_AUTOMATIC;
  }
}

ParkMode getParkMode(int32_t modeValue) {
  switch (modeValue) {
  case RobotSetParkMode::Request::PARK_MODE_AUTOMATIC:
    return ParkMode::AUTOMATIC;
  case RobotSetParkMode::Request::PARK_MODE_MANUAL:
    return ParkMode::AUTOMATIC;
  default:
    LOGERR("Error, received unsupported ChessMoveType value: %d", modeValue);
    return ParkMode::UNKNOWN;
  }
}