#ifndef ROBO_CHESS_ROBOCHESSCONFIG_H_
#define ROBO_CHESS_ROBOCHESSCONFIG_H_

//System headers
#include <cstdint>
#include <string>

//Other libraries headers
#include "ur_control_common/external_api/config/UrContolCommonExternalBridgeConfig.h"
#include "ros2_game_engine/communicator/config/Ros2CommunicatorConfig.h"
#include "game_engine/defines/ActionEventDefines.h"

//Own components headers
#include "robo_chess/motion/config/RoboChessMotionSequenceConfig.h"

//Forward declarations

struct RoboChessConfig {
  Ros2CommunicatorConfig ros2CommunicatorCfg;
  UrContolCommonExternalBridgeConfig urControlExternalInterfaceCfg;
  RoboChessMotionSequenceConfig roboChessMotionSequenceCfg;
  ActionEventHandlerPolicy actionEventHandlerPolicy = 
    ActionEventHandlerPolicy::BLOCKING;
};

#endif /* ROBO_CHESS_ROBOCHESSCONFIG_H_ */

