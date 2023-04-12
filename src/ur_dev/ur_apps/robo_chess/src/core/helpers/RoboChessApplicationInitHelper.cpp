//Corresponding header
#include "robo_chess/core/helpers/RoboChessApplicationInitHelper.h"

//System headers

//Other libraries headers
#include "utils/Log.h"

//Own components headers
#include "robo_chess/core/RoboChessApplication.h"
#include "robo_chess/defines/RoboChessDefines.h"
#include "robo_chess/motion/ChessMoveMotionSequence.h"
#include "robo_chess/motion/ParkMotionSequence.h"
#include "robo_chess/motion/TurnEndMotionSequence.h"

namespace {
using namespace std::placeholders;

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
  app._dashboardProvider = std::make_shared<DashboardProvider>();
  app._urScriptBuilder = std::make_shared<UrScriptBuilder>();

  if (!app._communicator || 
      !app._roboChessExternalInterface || 
      !app._urControlExternalInterface ||
      !app._dashboardProvider) {
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

  if (ErrorCode::SUCCESS != initDashboardHelper(app)) {
    LOGERR("Error in initDashboardHelper()");
    return ErrorCode::FAILURE;
  }

  if (ErrorCode::SUCCESS != 
      initMotionExecutor(cfg.roboChessMotionSequenceCfg, app)) {
    LOGERR("Error in initMotionExecutor()");
    return ErrorCode::FAILURE;
  }

  if (ErrorCode::SUCCESS != initUrScriptBuilder(cfg.urScriptBuilderCfg, app)) {
    LOGERR("initUrScriptBuilder() failed");
    return ErrorCode::FAILURE;
  }

  return ErrorCode::SUCCESS;
}

ErrorCode RoboChessApplicationInitHelper::initUrControlExternalBridge(
    const UrContolCommonExternalBridgeConfig& cfg, RoboChessApplication& app) {
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

ErrorCode RoboChessApplicationInitHelper::initDashboardHelper(
    RoboChessApplication &app) {
  DashboardProviderOutInterface outInterface;
  outInterface.invokeActionEventCb = 
    std::bind(&ActionEventHandlerSpawner::invokeActionEvent, 
    &app._actionEventHandlerSpawner, _1, _2);
  outInterface.robotModeChangeCb = 
    std::bind(&RobotModeHandler::changeRobotMode, &app._robotModeHandler, _1);
  outInterface.safetyModeChangeCb = 
    std::bind(&RobotModeHandler::changeSafetyMode, &app._robotModeHandler, _1);

  if (ErrorCode::SUCCESS != app._dashboardProvider->init(outInterface)) {
    LOGERR("Error in _dashboardProvider.init()");
    return ErrorCode::FAILURE;
  }

  return ErrorCode::SUCCESS;
}

ErrorCode RoboChessApplicationInitHelper::initMotionExecutor(
  const RoboChessMotionSequenceConfig &cfg, RoboChessApplication &app) {
  MotionSequenceExecutorOutInterface outInterface;
  const auto externalBridgeRawPointer = app._urControlExternalInterface.get();
  outInterface.publishURScriptCb = std::bind(
    &UrControlCommonExternalBridge::publishURScript, 
    externalBridgeRawPointer, _1);

  outInterface.invokeURScriptServiceCb = std::bind(
    &UrControlCommonExternalBridge::invokeURScriptService, 
    externalBridgeRawPointer, _1);

  outInterface.invokeURScriptPreemptServiceCb = std::bind(
    &UrControlCommonExternalBridge::invokeURScriptPreemptService, 
    externalBridgeRawPointer);

  outInterface.invokeActionEventCb = 
    std::bind(&ActionEventHandlerSpawner::invokeActionEvent, 
    &app._actionEventHandlerSpawner, _1, _2);

  if (ErrorCode::SUCCESS != app._motionExecutor.init(outInterface)) {
    LOGERR("Error in _motionExecutor.init()");
    return ErrorCode::FAILURE;
  }

  auto chessMoveMotionSequence = std::make_unique<ChessMoveMotionSequence>(
    cfg.chessMoveMotionSequenceCfg, motion::CHESS_MOVE_MOTION_SEQUENCE_NAME, 
    motion::CHESS_MOVE_MOTION_ID, app._urScriptBuilder);
  if (ErrorCode::SUCCESS != 
      app._motionExecutor.addSequence(std::move(chessMoveMotionSequence))) {
    LOGERR("Error in motionExecutor.addSequence() for ChessMoveMotionSequence");
    return ErrorCode::FAILURE;
  }

  auto parkMotionSequence = std::make_unique<ParkMotionSequence>(
    cfg.parkMotionSequenceCfg, motion::PARK_MOTION_SEQUENCE_NAME, 
    motion::PARK_MOTION_ID, app._urScriptBuilder);
  if (ErrorCode::SUCCESS != 
      app._motionExecutor.addSequence(std::move(parkMotionSequence))) {
    LOGERR("Error in motionExecutor.addSequence() for ParkMotionSequence");
    return ErrorCode::FAILURE;
  }

  auto turnEndMotionSequence = std::make_unique<TurnEndMotionSequence>(
    cfg.turnEndMotionSequenceCfg, motion::TURN_END_MOTION_SEQUENCE_NAME, 
    motion::TURN_END_MOTION_ID, app._urScriptBuilder);
  if (ErrorCode::SUCCESS != 
      app._motionExecutor.addSequence(std::move(turnEndMotionSequence))) {
    LOGERR("Error in motionExecutor.addSequence() for TurnEndMotionSequence");
    return ErrorCode::FAILURE;
  }

  return ErrorCode::SUCCESS;
}

ErrorCode RoboChessApplicationInitHelper::initUrScriptBuilder(
    const UrScriptBuilderConfig &cfg, RoboChessApplication &app) {
  if (ErrorCode::SUCCESS != app._urScriptBuilder->init(cfg)) {
    LOGERR("Error in _urScriptBuilder->init()");
    return ErrorCode::FAILURE;
  }

  return ErrorCode::SUCCESS;
}