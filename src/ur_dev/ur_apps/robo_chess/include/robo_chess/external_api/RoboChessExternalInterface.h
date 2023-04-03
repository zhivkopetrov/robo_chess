#ifndef ROBO_CHESS_ROBOCHESSEXTERNALINTERFACE_H
#define ROBO_CHESS_ROBOCHESSEXTERNALINTERFACE_H

//System headers

//Other libraries headers
#include <rclcpp/node.hpp>
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
  ErrorCode initCommunication();
};

#endif /* ROBO_CHESS_ROBOCHESSEXTERNALINTERFACE_H */
