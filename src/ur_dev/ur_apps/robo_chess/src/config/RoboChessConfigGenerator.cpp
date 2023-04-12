//Corresponding header
#include "robo_chess/config/RoboChessConfigGenerator.h"

//System headers

//Other libraries headers
#include <rclcpp/utilities.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include "resource_utils/common/ResourceFileHeader.h"
#include "utils/ErrorCode.h"
#include "utils/Log.h"

//Own components headers
#include "robo_chess/external_api/RoboChessRos2ParamProvider.h"

namespace {
constexpr auto PROJECT_NAME = "robo_chess";
constexpr auto SCRIPTS_FOLDER_NAME = "scripts";
constexpr auto GRIPPER_DEFINITIONS_FOLDER_NAME = "gripper_definitions";

const AngleAxis HOME_ORIENTATION(0, 3.148, 0);
const AngleAxis HOME_NINENY_ORIENTATION(2.221, 2.221, 0);
const WaypointJoint WAYPOINT_HOME_JOINT = 
  WaypointJoint({ -90, -90, -90, -90, 90, 0 });
const WaypointCartesian WAYPOINT_HOME_CARTESIAN = 
  WaypointCartesian(Point3d(-0.176, -0.691, 0.502), HOME_ORIENTATION);
constexpr double DEFAULT_MOTION_ACCELERATION = 0.5; // [m/s2]
constexpr double DEFAULT_MOTION_VELOCITY = 0.5; // [m/s]

UrContolCommonExternalBridgeConfig generetaUrContolCommonExternalBridgeConfig(
    const RoboChessRos2Params& params) {
  UrContolCommonExternalBridgeConfig cfg;
  cfg.robotIp = params.robotIp;
  cfg.robotInterfacePort = params.robotInterfacePort;

  return cfg;
}

Ros2CommunicatorConfig generateRos2CommunicatorConfig(
    const RoboChessRos2Params& params) {
  Ros2CommunicatorConfig cfg = params.ros2CommunicatorCfg;

  //block the main thread and wait for shutdown signal
  //a secondary (ActionEventHandler thread) will be spawned to process ActionEvents
  cfg.executionPolicy = ExecutionPolicy::BLOCKING;

  return cfg;
}

TurnEndMotionSequenceConfig generateTurnEndMotionSequenceConfig() {
  TurnEndMotionSequenceConfig cfg;

  cfg.motionAcc = DEFAULT_MOTION_ACCELERATION; 
  cfg.motionVel = DEFAULT_MOTION_VELOCITY;

  //TODO parse from files
  cfg.turnEndApproachCartesian = WaypointCartesian(
    Point3d(-0.196, -0.812, 0.6), HOME_ORIENTATION);
  cfg.turnEndCartesian = WaypointCartesian(
    Point3d(-0.196, -0.812, 0.236), HOME_ORIENTATION);

  cfg.turnEndJoint = 
    WaypointJoint({ -92.42, -127.52, -126.75, 74.27, 91.99, 0 });
  cfg.turnEndApproachJoint = 
    WaypointJoint({ -92.42, -94.62, -118.4, 33.02, 92.71, 0 });

  return cfg;
}

ChessMoveMotionSequenceConfig generateChessMoveMotionSequenceConfig() {
  ChessMoveMotionSequenceConfig cfg;

  ChessPieceDimensions& pieceDimensions = cfg.chessPieceDimensions;
  //TODO populate from config
  pieceDimensions.width = 0.032;  // [m]
  pieceDimensions.depth = 0.032;  // [m]
  pieceDimensions.height = 0.064; // [m]

  constexpr double gripperOpeningAddition = 0.015; // [m]

  //cummulative opening of 40 mm between gripper fingers
  constexpr int32_t m_to_mm_ratio = 1000;
  cfg.gripperOpening = static_cast<int32_t>(
    (pieceDimensions.depth + gripperOpeningAddition) * m_to_mm_ratio);

  cfg.motionAcc = DEFAULT_MOTION_ACCELERATION; 
  cfg.motionVel = DEFAULT_MOTION_VELOCITY;

  //TODO parse from files
  cfg.graspApproachJoint = 
    WaypointJoint({ -34.79, -99.07, -82.88, -88.33, 90.22, 55.23 });

  cfg.zeroOrientation = HOME_ORIENTATION;
  cfg.ninetyOrientation = HOME_NINENY_ORIENTATION;
  cfg.graspApproachCartesian = WaypointCartesian(
    Point3d(0.545, -0.592, 0.475), HOME_ORIENTATION);

  return cfg;
}

ParkMotionSequenceConfig generateParkMotionSequenceConfig() {
  ParkMotionSequenceConfig cfg;

  cfg.motionAcc = DEFAULT_MOTION_ACCELERATION; 
  cfg.motionVel = DEFAULT_MOTION_VELOCITY;

  //TODO parse from files
  cfg.homeCartesian = WAYPOINT_HOME_CARTESIAN;
  cfg.homeJoint = WAYPOINT_HOME_JOINT;

  return cfg;
}

RoboChessMotionSequenceConfig generateRoboChessMotionSequenceConfig() {
  RoboChessMotionSequenceConfig cfg;
  cfg.chessMoveMotionSequenceCfg = generateChessMoveMotionSequenceConfig();
  cfg.turnEndMotionSequenceCfg = generateTurnEndMotionSequenceConfig();
  cfg.parkMotionSequenceCfg = generateParkMotionSequenceConfig();

  return cfg;
}

UrScriptBuilderConfig generateUrScriptBuilderConfig(
  const std::string &projectInstallPrefix,
  const RoboChessRos2Params &rosParams) {
  UrScriptBuilderConfig cfg;

  std::string scriptsFolderLocation = projectInstallPrefix;
  scriptsFolderLocation.append("/").append(
      ResourceFileHeader::getResourcesFolderName().append("/").append(
          SCRIPTS_FOLDER_NAME)).append("/");

  cfg.gripperDefinitionFolder = 
    scriptsFolderLocation + GRIPPER_DEFINITIONS_FOLDER_NAME;

  cfg.gripperType = rosParams.gripperType;

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

  const auto projectInstallPrefix =
      ament_index_cpp::get_package_share_directory(PROJECT_NAME);

  auto paramProviderNode = std::make_shared<RoboChessRos2ParamProvider>();
  const auto rosParams = paramProviderNode->getParams();
  rosParams.print();

  cfg.urControlExternalInterfaceCfg =
      generetaUrContolCommonExternalBridgeConfig(rosParams);
  cfg.ros2CommunicatorCfg = generateRos2CommunicatorConfig(rosParams);
  cfg.roboChessMotionSequenceCfg = generateRoboChessMotionSequenceConfig();
  cfg.urScriptBuilderCfg = 
    generateUrScriptBuilderConfig(projectInstallPrefix, rosParams);
  cfg.actionEventHandlerPolicy = ActionEventHandlerPolicy::BLOCKING;

  return cfg;
}

