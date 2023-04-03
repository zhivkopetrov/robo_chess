#ifndef ROBO_CHESS_ROBOCHESSEXTERNALINTERFACE_H
#define ROBO_CHESS_ROBOCHESSEXTERNALINTERFACE_H

//System headers

//Other libraries headers
#include <rclcpp/node.hpp>
#include "utils/class/NonCopyable.h"
#include "utils/class/NonMoveable.h"
#include "utils/ErrorCode.h"

//Own components headers
#include "robo_chess/external_api/config/RoboChessExternalInterfaceConfig.h"

//Forward declarations

class RoboChessExternalInterface: public rclcpp::Node,
    public NonCopyable,
    public NonMoveable {
public:
  RoboChessExternalInterface();

  ErrorCode init(const RoboChessExternalInterfaceConfig& cfg);

private:
  ErrorCode initCommunication();
};

#endif /* ROBO_CHESS_ROBOCHESSEXTERNALINTERFACE_H */
