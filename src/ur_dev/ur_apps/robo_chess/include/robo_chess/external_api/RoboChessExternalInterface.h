#ifndef ROBO_CHESS_ROBOCHESSEXTERNALINTERFACE_H
#define ROBO_CHESS_ROBOCHESSEXTERNALINTERFACE_H

//System headers

//Other libraries headers
#include <rclcpp/node.hpp>
#include "robo_chess_interfaces/srv/robot_abort_motion.hpp"
#include "robo_chess_interfaces/srv/robot_chess_move.hpp"
#include "robo_chess_interfaces/srv/robot_park.hpp"
#include "robo_chess_interfaces/srv/robot_set_park_mode.hpp"
#include "game_engine/defines/ActionEventDefines.h"
#include "utils/class/NonCopyable.h"
#include "utils/class/NonMoveable.h"
#include "utils/ErrorCode.h"

//Own components headers
#include "robo_chess/defines/RoboChessFunctionalDefines.h"

//Forward declarations

struct RoboChessExternalInterfaceOutInterface {
  InvokeActionEventCb invokeActionEventCb;
  AbortMotionCb abortMotionCb;
  ChessMoveCb chessMoveCb;
  ParkCb parkCb;
  SetParkModeCb setParkModeCb;
};

class RoboChessExternalInterface: public rclcpp::Node,
    public NonCopyable,
    public NonMoveable {
public:
  RoboChessExternalInterface();

  ErrorCode init(const RoboChessExternalInterfaceOutInterface& outInterface);

private:
  using RobotAbortMotion = robo_chess_interfaces::srv::RobotAbortMotion;
  using RobotChessMove = robo_chess_interfaces::srv::RobotChessMove;
  using RobotPark = robo_chess_interfaces::srv::RobotPark;
  using RobotSetParkMode = robo_chess_interfaces::srv::RobotSetParkMode;

  ErrorCode initOutInterface(
    const RoboChessExternalInterfaceOutInterface& outInterface);
  ErrorCode initCommunication();

  void handleAbortMotionService(
    const std::shared_ptr<RobotAbortMotion::Request> request,
    std::shared_ptr<RobotAbortMotion::Response> response);

  void handleChessMoveService(
    const std::shared_ptr<RobotChessMove::Request> request,
    std::shared_ptr<RobotChessMove::Response> response);

  void handleParkService(
    const std::shared_ptr<RobotPark::Request> request,
    std::shared_ptr<RobotPark::Response> response);

  void handleSetParkModeService(
    const std::shared_ptr<RobotSetParkMode::Request> request,
    std::shared_ptr<RobotSetParkMode::Response> response);

  rclcpp::Service<RobotAbortMotion>::SharedPtr _abortMotionService;
  rclcpp::Service<RobotChessMove>::SharedPtr _chessMoveService;
  rclcpp::Service<RobotPark>::SharedPtr _parkService;
  rclcpp::Service<RobotSetParkMode>::SharedPtr _setParkModeService;

  const rclcpp::CallbackGroup::SharedPtr _callbackGroup = create_callback_group(
      rclcpp::CallbackGroupType::Reentrant);

  RoboChessExternalInterfaceOutInterface _outInterface;
};

#endif /* ROBO_CHESS_ROBOCHESSEXTERNALINTERFACE_H */
