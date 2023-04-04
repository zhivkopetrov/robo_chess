//Corresponding header
#include "robo_chess/external_api/RoboChessExternalInterface.h"

//System headers

//Other libraries headers
#include "robo_chess_common/defines/RoboChessCommonTopics.h"
#include "robo_chess_common/message_helpers/RoboChessCommonMessageHelpers.h"
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
  _abortMotionService = create_service<RobotAbortMotion>(
    services::ABORT_MOTION_NAME,
    std::bind(&RoboChessExternalInterface::handleAbortMotionService, this,
        std::placeholders::_1, std::placeholders::_2),
    rmw_qos_profile_services_default, _callbackGroup);

  _chessMoveService = create_service<RobotChessMove>(services::CHESS_MOVE_NAME,
    std::bind(&RoboChessExternalInterface::handleChessMoveService, this,
        std::placeholders::_1, std::placeholders::_2),
    rmw_qos_profile_services_default, _callbackGroup);

  _parkService = create_service<RobotPark>(services::PARK_NAME,
    std::bind(&RoboChessExternalInterface::handleParkService, this,
        std::placeholders::_1, std::placeholders::_2),
    rmw_qos_profile_services_default, _callbackGroup);

  _setParkModeService = create_service<RobotSetParkMode>(
    services::SET_PARK_MODE_NAME,
    std::bind(&RoboChessExternalInterface::handleSetParkModeService, this,
        std::placeholders::_1, std::placeholders::_2),
    rmw_qos_profile_services_default, _callbackGroup);

  return ErrorCode::SUCCESS;
}

void RoboChessExternalInterface::handleAbortMotionService(
  const std::shared_ptr<RobotAbortMotion::Request> request,
  std::shared_ptr<RobotAbortMotion::Response> response) {
  const AbortMotion abortMotion = getAbortMotion(request->abort_type);
  LOG("Received RobotAbortMotion::Request with type: [%d], name: [%s]", 
      request->abort_type, toString(abortMotion).c_str());
  if (AbortMotion::UNKNOWN == abortMotion) {
    response->success = false;
    response->error_reason = 
      "Received unsupported RobotAbortMotion::Request value: " + 
        std::to_string(request->abort_type);
    return;
  }

  response->success = true;
}

void RoboChessExternalInterface::handleChessMoveService(
  const std::shared_ptr<RobotChessMove::Request> request,
  std::shared_ptr<RobotChessMove::Response> response) {
  const ChessMoveType chessMoveType = getChessMoveType(request->move_type);
  LOG("Received RobotChessMove::Request with type: [%d], name: [%s], "
      "curr_pos: [%s], future_pos: [%s]", 
      request->move_type, toString(chessMoveType).c_str(), 
      request->curr_pos.c_str(), request->future_pos.c_str());
  if (ChessMoveType::UNKNOWN == chessMoveType) {
    response->success = false;
    response->error_reason = 
      "Received unsupported RobotChessMove::Request value: " + 
        std::to_string(request->move_type);
    return;
  }

  response->success = true;
}

void RoboChessExternalInterface::handleParkService(
  [[maybe_unused]]const std::shared_ptr<RobotPark::Request> request,
  std::shared_ptr<RobotPark::Response> response) {
  LOG("Received RobotPark::Request");
  response->success = true;
}

void RoboChessExternalInterface::handleSetParkModeService(
  const std::shared_ptr<RobotSetParkMode::Request> request,
  std::shared_ptr<RobotSetParkMode::Response> response) {
  const ParkMode parkMode = getParkMode(request->park_mode);
  LOG("Received RobotSetParkMode::Request with type: [%d], name: [%s]", 
      request->park_mode, toString(parkMode).c_str());
  if (ParkMode::UNKNOWN == parkMode) {
    response->success = false;
    response->error_reason = 
      "Received unsupported RobotSetParkMode::Request value: " + 
        std::to_string(request->park_mode);
    return;
  }

  response->success = true;
}