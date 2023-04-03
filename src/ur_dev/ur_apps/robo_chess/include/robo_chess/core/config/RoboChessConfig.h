#ifndef ROBO_CHESS_ROBOCHESSCONFIG_H_
#define ROBO_CHESS_ROBOCHESSCONFIG_H_

//System headers
#include <cstdint>
#include <string>

//Other libraries headers
#include "ros2_game_engine/communicator/config/Ros2CommunicatorConfig.h"

//Own components headers
#include "robo_chess/external_api/config/RoboChessExternalInterfaceConfig.h"

//Forward declarations

struct RoboChessConfig {
  Ros2CommunicatorConfig ros2CommunicatorCfg;
  RoboChessExternalInterfaceConfig externalInterfaceCfg;
};

#endif /* ROBO_CHESS_ROBOCHESSCONFIG_H_ */

