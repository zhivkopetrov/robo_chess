//Corresponding header
#include "robo_chess/core/RoboChessApplication.h"

//System headers

//Other libraries headers
#include "utils/ErrorCode.h"
#include "utils/Log.h"

//Own components headers
#include "robo_chess/core/helpers/RoboChessApplicationInitHelper.h"

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
