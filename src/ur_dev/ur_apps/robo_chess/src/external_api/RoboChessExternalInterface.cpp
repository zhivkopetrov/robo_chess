//Corresponding header
#include "robo_chess/external_api/RoboChessExternalInterface.h"

//System headers

//Other libraries headers
#include "utils/Log.h"

//Own components headers

namespace {
constexpr auto NODE_NAME = "RoboChessExternalInterface";
}

RoboChessExternalInterface::RoboChessExternalInterface()
    : rclcpp::Node(NODE_NAME) {

}

ErrorCode RoboChessExternalInterface::init() {
  if (ErrorCode::SUCCESS != initCommunication()) {
    LOGERR("Error, initCommunication() failed");
    return ErrorCode::FAILURE;
  }

  return ErrorCode::SUCCESS;
}

ErrorCode RoboChessExternalInterface::initCommunication() {
  return ErrorCode::SUCCESS;
}