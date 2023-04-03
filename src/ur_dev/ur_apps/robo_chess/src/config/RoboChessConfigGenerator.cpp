//Corresponding header
#include "robo_chess/config/RoboChessConfigGenerator.h"

//System headers

//Other libraries headers
#include "utils/ErrorCode.h"
#include "utils/Log.h"

//Own components headers
#include "robo_chess/external_api/RoboChessRos2ParamProvider.h"

namespace {
RoboChessExternalInterfaceConfig generetaRoboChessExternalInterfaceConfig(
    const RoboChessRos2Params& params) {
  RoboChessExternalInterfaceConfig cfg;
  cfg.robotIp = params.robotIp;
  cfg.robotInterfacePort = params.robotInterfacePort;

  return cfg;
}

Ros2CommunicatorConfig generateRos2CommunicatorConfig(
    const RoboChessRos2Params& params) {
  Ros2CommunicatorConfig cfg = params.ros2CommunicatorCfg;

  //block the main thread from the executor
  cfg.executionPolicy = ExecutionPolicy::BLOCKING;

  return cfg;
}

} //end anonymous namespace

std::vector<DependencyDescription> RoboChessConfigGenerator::generateDependencies(
    int32_t argc, char **args) {
  const LoadDependencyCb ros2Loader = [argc, args]() {
    rclcpp::InitOptions initOptions;
    //SIGINT signal will cancel the executor and will unblock the main thread
    initOptions.shutdown_on_signal = true;

    rclcpp::init(argc, args, initOptions);
    return ErrorCode::SUCCESS;
  };
  const UnloadDependencyCb ros2Unloader = []() {
    //shutdown the global context only if it hasn't
    //for example: ROS2 signal handlers do that automatically
    if (rclcpp::ok()) {
      const bool success = rclcpp::shutdown();
      if (!success) {
        LOGERR("Error, global context was already shutdowned");
      }
    }
  };

  const DependencyDescription dependency = { .name = "ROS2",
      .loadDependencyCb = ros2Loader, .unloadDependencyCb = ros2Unloader };

  return { dependency };
}

RoboChessConfig RoboChessConfigGenerator::generateConfig() {
  RoboChessConfig cfg;

  auto paramProviderNode = std::make_shared<RoboChessRos2ParamProvider>();
  const auto rosParams = paramProviderNode->getParams();
  rosParams.print();

  cfg.externalInterfaceCfg =
      generetaRoboChessExternalInterfaceConfig(rosParams);
  cfg.ros2CommunicatorCfg = generateRos2CommunicatorConfig(rosParams);

  return cfg;
}

