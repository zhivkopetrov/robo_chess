//Corresponding header
#include "robo_chess/core/helpers/RoboChessApplicationInitHelper.h"

//System headers

//Other libraries headers
#include "utils/Log.h"

//Own components headers
#include "robo_chess/core/RoboChessApplication.h"

namespace {
constexpr auto UR_CONTROL_EXTERNAL_BRIDGE_NODE_NAME = 
  "UrControlBloomExternalBridge";
}

ErrorCode RoboChessApplicationInitHelper::init(
  const RoboChessConfig& cfg, RoboChessApplication& app) {

  //first create objects so callbacks cn be attached
  if (ErrorCode::SUCCESS != createObjects(app)) {
    LOGERR("Error in createObjects()");
    return ErrorCode::FAILURE;
  }

  if (ErrorCode::SUCCESS != initInternal(cfg, app)) {
    LOGERR("Error in initInternal()");
    return ErrorCode::FAILURE;
  }

  return ErrorCode::SUCCESS;
}

ErrorCode RoboChessApplicationInitHelper::createObjects(
  RoboChessApplication& app) {
  app._communicator = std::make_unique<Ros2Communicator>();
  app._roboChessExternalInterface = 
    std::make_shared<RoboChessExternalInterface>();
  app._urControlExternalInterface = 
   std::make_shared<UrControlCommonExternalBridge>(
      UR_CONTROL_EXTERNAL_BRIDGE_NODE_NAME);

  if (!app._communicator || 
      !app._roboChessExternalInterface || 
      !app._urControlExternalInterface) {
    LOGERR("Error, bad alloc for objects");
    return ErrorCode::FAILURE;
  }

  return ErrorCode::SUCCESS;
}

ErrorCode RoboChessApplicationInitHelper::initInternal(
  const RoboChessConfig& cfg, RoboChessApplication& app) {
  if (ErrorCode::SUCCESS != 
        app._actionEventHandlerSpawner.init(cfg.actionEventHandlerPolicy)) {
    LOGERR("Error in ActionEventHandlerSpawner.init()");
    return ErrorCode::FAILURE;
  }

  if (ErrorCode::SUCCESS != app._communicator->init(cfg.ros2CommunicatorCfg)) {
    LOGERR("Error in _communicator.init()");
    return ErrorCode::FAILURE;
  }

  if (ErrorCode::SUCCESS != app._roboChessExternalInterface->init()) {
    LOGERR("Error in RoboChessExternalInterface.init()");
    return ErrorCode::FAILURE;
  }

  if (ErrorCode::SUCCESS != 
      initUrControlExternalBridge(cfg.urControlExternalInterfaceCfg, app)) {
    LOGERR("Error in initUrControlExternalBridge()");
    return ErrorCode::FAILURE;
  }

  return ErrorCode::SUCCESS;
}

ErrorCode RoboChessApplicationInitHelper::initUrControlExternalBridge(
    const UrContolCommonExternalBridgeConfig& cfg, RoboChessApplication& app) {
  using namespace std::placeholders;

  UrControlCommonExternalBridgeOutInterface outInterface;
  outInterface.invokeActionEventCb =
    std::bind(&ActionEventHandlerSpawner::invokeActionEvent, 
    &app._actionEventHandlerSpawner, _1, _2);
  outInterface.robotModeChangeCb = 
    std::bind(&RobotModeHandler::changeRobotMode, &app._robotModeHandler, _1);
  outInterface.safetyModeChangeCb =
    std::bind(&RobotModeHandler::changeSafetyMode, &app._robotModeHandler, _1);

  if (ErrorCode::SUCCESS !=
      app._urControlExternalInterface->init(cfg, outInterface)) {
    LOGERR("Error in UrControlCommonExternalBridge.init()");
    return ErrorCode::FAILURE;
  }

  return ErrorCode::SUCCESS;
}