//Corresponding header
#include "robo_chess/external_api/RoboChessExternalInterface.h"

//System headers

//Other libraries headers
#include "utils/Log.h"

//Own components headers
#include "robo_chess/defines/RoboChessDefines.h"

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
  _abortMotionService = create_service<AbortMotion>(services::ABORT_MOTION_NAME,
    std::bind(&RoboChessExternalInterface::handleAbortMotionService, this,
        std::placeholders::_1, std::placeholders::_2),
    rmw_qos_profile_services_default, _callbackGroup);

  _chessMoveService = create_service<ChessMove>(services::CHESS_MOVE_NAME,
    std::bind(&RoboChessExternalInterface::handleChessMoveService, this,
        std::placeholders::_1, std::placeholders::_2),
    rmw_qos_profile_services_default, _callbackGroup);

  _parkService = create_service<Park>(services::PARK_NAME,
    std::bind(&RoboChessExternalInterface::handleParkService, this,
        std::placeholders::_1, std::placeholders::_2),
    rmw_qos_profile_services_default, _callbackGroup);

  _setParkModeService = create_service<SetParkMode>(
    services::SET_PARK_MODE_NAME,
    std::bind(&RoboChessExternalInterface::handleSetParkModeService, this,
        std::placeholders::_1, std::placeholders::_2),
    rmw_qos_profile_services_default, _callbackGroup);

  return ErrorCode::SUCCESS;
}

void RoboChessExternalInterface::handleAbortMotionService(
  [[maybe_unused]]const std::shared_ptr<AbortMotion::Request> request,
  [[maybe_unused]]std::shared_ptr<AbortMotion::Response> response) {

}

void RoboChessExternalInterface::handleChessMoveService(
  [[maybe_unused]]const std::shared_ptr<ChessMove::Request> request,
  [[maybe_unused]]std::shared_ptr<ChessMove::Response> response) {

}

void RoboChessExternalInterface::handleParkService(
  [[maybe_unused]]const std::shared_ptr<Park::Request> request,
  [[maybe_unused]]std::shared_ptr<Park::Response> response) {

}

void RoboChessExternalInterface::handleSetParkModeService(
  [[maybe_unused]]const std::shared_ptr<SetParkMode::Request> request,
  [[maybe_unused]]std::shared_ptr<SetParkMode::Response> response) {

}