#ifndef ROBO_CHESS_ROBOCHESSROS2PARAMPROVIDER_H_
#define ROBO_CHESS_ROBOCHESSROS2PARAMPROVIDER_H_

//System headers
#include <string>

//Other libraries headers
#include <rclcpp/node.hpp>
#include "urscript_common/defines/UrScriptDefines.h"
#include "ros2_game_engine/communicator/config/Ros2CommunicatorConfig.h"

//Own components headers

//Forward declarations

struct RoboChessRos2Params {
  Ros2CommunicatorConfig ros2CommunicatorCfg;
  GripperType gripperType = GripperType::SIMULATION;
  std::string robotIp;
  uint16_t robotInterfacePort { };

  void print() const;
  void validate();
};

class RoboChessRos2ParamProvider: public rclcpp::Node {
public:
  RoboChessRos2ParamProvider();

  RoboChessRos2Params getParams();

private:
  RoboChessRos2Params _params;
};

#endif /* ROBO_CHESS_ROBOCHESSROS2PARAMPROVIDER_H_ */
