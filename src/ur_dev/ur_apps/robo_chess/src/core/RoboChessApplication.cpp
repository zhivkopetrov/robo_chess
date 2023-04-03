//Corresponding header
#include "robo_chess/core/RoboChessApplication.h"

//System headers

//Other libraries headers
#include "game_engine/communicator/NullCommunicator.h"
#include "utils/debug/SignalHandler.h"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

//Own components headers

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
  _communicator = std::make_unique<Ros2Communicator>();
  if (ErrorCode::SUCCESS != _communicator->init(cfg.ros2CommunicatorCfg)) {
    LOGERR("Error in _communicator.init()");
    return ErrorCode::FAILURE;
  }

  _externalInterface = std::make_shared<RoboChessExternalInterface>();
  if (ErrorCode::SUCCESS !=
      _externalInterface->init(cfg.externalInterfaceCfg)) {
    LOGERR("Error in _externalInterface.init()");
    return ErrorCode::FAILURE;
  }

  _communicator->registerNode(_externalInterface);
  return ErrorCode::SUCCESS;
}

ErrorCode RoboChessApplication::run() {
  LOGG("Starting RoboChess Application");

  //blocking call
  _communicator->start();

  return ErrorCode::SUCCESS;
}

void RoboChessApplication::deinit() {
  //manually reset point after deinit to enforce destruction order
  _communicator->unregisterNode(_externalInterface);
  _externalInterface.reset();

  _communicator->deinit();
  _communicator.reset();
}

void RoboChessApplication::unloadDependencies() {
  //unload dependency in reverse order -> they might be dependent
  for (auto revIt = _dependencies.rbegin(); revIt != _dependencies.rend();
      ++revIt) {
    revIt->unloadDependencyCb();
  }
}
