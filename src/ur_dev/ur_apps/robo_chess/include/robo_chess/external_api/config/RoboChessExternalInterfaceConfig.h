#ifndef ROBO_CHESS_ROBOCHESSEXTERNALINTERFACECONFIG_H_
#define ROBO_CHESS_ROBOCHESSEXTERNALINTERFACECONFIG_H_

//System headers
#include <cstdint>
#include <string>

//Other libraries headers

//Own components headers

//Forward declarations

struct RoboChessExternalInterfaceConfig {
  std::string robotIp;
  uint16_t robotInterfacePort { };
};

#endif /* ROBO_CHESS_ROBOCHESSEXTERNALINTERFACECONFIG_H_ */
