#ifndef ROBO_CHESS_ROBOCHESSEXTERNALINTERFACE_H
#define ROBO_CHESS_ROBOCHESSEXTERNALINTERFACE_H

//System headers

//Other libraries headers
#include <rclcpp/node.hpp>
#include "robo_chess_interfaces/srv/robot_abort_motion.hpp"
#include "robo_chess_interfaces/srv/robot_chess_move.hpp"
#include "robo_chess_interfaces/srv/robot_park.hpp"
#include "robo_chess_interfaces/srv/robot_set_park_mode.hpp"
#include "utils/class/NonCopyable.h"
#include "utils/class/NonMoveable.h"
#include "utils/ErrorCode.h"

//Own components headers

//Forward declarations

class RoboChessExternalInterface: public rclcpp::Node,
    public NonCopyable,
    public NonMoveable {
public:
  RoboChessExternalInterface();

  ErrorCode init();

private:
  using AbortMotion = robo_chess_interfaces::srv::RobotAbortMotion;
  using ChessMove = robo_chess_interfaces::srv::RobotChessMove;
  using Park = robo_chess_interfaces::srv::RobotPark;
  using SetParkMode = robo_chess_interfaces::srv::RobotSetParkMode;

  ErrorCode initCommunication();

  void handleAbortMotionService(
    const std::shared_ptr<AbortMotion::Request> request,
    std::shared_ptr<AbortMotion::Response> response);

  void handleChessMoveService(
    const std::shared_ptr<ChessMove::Request> request,
    std::shared_ptr<ChessMove::Response> response);

  void handleParkService(
    const std::shared_ptr<Park::Request> request,
    std::shared_ptr<Park::Response> response);

  void handleSetParkModeService(
    const std::shared_ptr<SetParkMode::Request> request,
    std::shared_ptr<SetParkMode::Response> response);

  rclcpp::Service<AbortMotion>::SharedPtr _abortMotionService;
  rclcpp::Service<ChessMove>::SharedPtr _chessMoveService;
  rclcpp::Service<Park>::SharedPtr _parkService;
  rclcpp::Service<SetParkMode>::SharedPtr _setParkModeService;

  const rclcpp::CallbackGroup::SharedPtr _callbackGroup = create_callback_group(
      rclcpp::CallbackGroupType::Reentrant);
};

#endif /* ROBO_CHESS_ROBOCHESSEXTERNALINTERFACE_H */
