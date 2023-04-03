#ifndef ROBO_CHESS_ROBOCHESSAPPLICATIONINITHELPER_H_
#define ROBO_CHESS_ROBOCHESSAPPLICATIONINITHELPER_H_

//System headers

//Other libraries headers
#include "utils/ErrorCode.h"

//Own components headers

//Forward declarations
class RoboChessApplication;
struct RoboChessConfig;
struct UrContolCommonExternalBridgeConfig;

class RoboChessApplicationInitHelper {
public:
  RoboChessApplicationInitHelper() = delete;

  static ErrorCode init(const RoboChessConfig& cfg, RoboChessApplication& app);

private:
  static ErrorCode createObjects(RoboChessApplication& app);
  
  static ErrorCode initInternal(
    const RoboChessConfig& cfg, RoboChessApplication& app);

  static ErrorCode initUrControlExternalBridge(
    const UrContolCommonExternalBridgeConfig& cfg, RoboChessApplication& app);
};

#endif /* ROBO_CHESS_ROBOCHESSAPPLICATIONINITHELPER_H_ */
