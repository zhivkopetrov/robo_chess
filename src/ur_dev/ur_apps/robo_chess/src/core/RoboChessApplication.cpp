//Corresponding header
#include "robo_chess/core/RoboChessApplication.h"

//System headers

//Other libraries headers
#include "utils/ErrorCode.h"
#include "utils/Log.h"

//Own components headers
#include "robo_chess/core/helpers/RoboChessApplicationInitHelper.h"
#include "robo_chess/defines/RoboChessDefines.h"

RoboChessApplication::~RoboChessApplication() noexcept {
  LOGG("RoboChess initiating shutdown ...");
  deinit();
  unloadDependencies();
  LOGG("RoboChess shutdown complete");
}

ErrorCode RoboChessApplication::loadDependencies(
    const std::vector<DependencyDescription> &dependencies) {
  _dependencies = dependencies;
  for (const auto& [name, loadDependency, unloadDependency] : _dependencies) {
    if (ErrorCode::SUCCESS != loadDependency()) {
      LOGERR("loadDependency() failed for %s", name.c_str());
      return ErrorCode::FAILURE;
    }
  }

  return ErrorCode::SUCCESS;
}

ErrorCode RoboChessApplication::init(const RoboChessConfig &cfg) {
  if (ErrorCode::SUCCESS != RoboChessApplicationInitHelper::init(cfg, *this)) {
    LOGERR("Error in RoboChessApplicationInitHelper.init()");
    return ErrorCode::FAILURE;
  }

  _communicator->registerNode(_dashboardProvider);
  _communicator->registerNode(_urControlExternalInterface);
  _communicator->registerNode(_roboChessExternalInterface);
  return ErrorCode::SUCCESS;
}

ErrorCode RoboChessApplication::run() {
  LOGG("Starting RoboChess Application");

  _actionEventHandlerSpawner.spawnAndRun();

  //blocking call
  _communicator->start();

  return ErrorCode::SUCCESS;
}

void RoboChessApplication::deinit() {
  _actionEventHandlerSpawner.shutdown();

  _communicator->unregisterNode(_dashboardProvider);
  _communicator->unregisterNode(_roboChessExternalInterface);
  _communicator->unregisterNode(_urControlExternalInterface);

  _communicator->deinit();
}

void RoboChessApplication::unloadDependencies() {
  //unload dependency in reverse order -> they might be dependent
  for (auto revIt = _dependencies.rbegin(); revIt != _dependencies.rend();
      ++revIt) {
    revIt->unloadDependencyCb();
  }
}

void RoboChessApplication::abortMotion(AbortMotion type) {
  _motionExecutor.loadSequence(motion::PARK_MOTION_ID);
  const MotionAction motionType = 
    (AbortMotion::GRACEFUL_STOP == type) ? 
      MotionAction::GRACEFUL_STOP : MotionAction::ABORT;

  const UrscriptsBatchDoneCb emptyCb = [](){};
  _motionExecutor.performAction(motionType, emptyCb);
}

void RoboChessApplication::chessMove(
  [[maybe_unused]]ChessMoveType type, 
  [[maybe_unused]]const std::string& currPos, 
  [[maybe_unused]]const std::string& futurePos) {
  const UrscriptsBatchDoneCb emptyCb = [](){};
  _motionExecutor.loadSequence(motion::CHESS_MOVE_MOTION_ID);
  _motionExecutor.performAction(MotionAction::START, emptyCb);
}

void RoboChessApplication::park() {
  const UrscriptsBatchDoneCb emptyCb = [](){};
  _motionExecutor.loadSequence(motion::PARK_MOTION_ID);
  _motionExecutor.performAction(MotionAction::START, emptyCb);
}

void RoboChessApplication::setParkMode(ParkMode type) {
  _parkMode = type;
}