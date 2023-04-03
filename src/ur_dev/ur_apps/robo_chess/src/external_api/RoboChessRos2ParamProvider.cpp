//Corresponding header
#include "robo_chess/external_api/RoboChessRos2ParamProvider.h"

//System headers
#include <sstream>
#include <iomanip>
#include <thread>

//Other libraries headers
#include "utils/data_type/EnumClassUtils.h"
#include "utils/Log.h"

//Own components headers

namespace {
constexpr auto NODE_NAME = "RoboChessRos2ParamProvider";

constexpr auto ROS2_EXECUTOR_TYPE_PARAM_NAME = "ros2_executor_type";
constexpr auto ROS2_EXECUTOR_THREADS_NUM_PARAM_NAME =
    "ros2_executor_threads_num";

constexpr auto ROBOT_IP_PARAM_NAME = "robot_ip";
constexpr auto ROBOT_INTERFACE_PORT_PARAM_NAME = "robot_interface_port";

//ROS2 executor
constexpr auto DEFAULT_EXECUTOR_TYPE = 0;
constexpr auto DEFAULT_EXECUTOR_THREADS_NUM = 2;

//robot
constexpr auto DEFAULT_ROBOT_IP = "192.168.1.102";
constexpr uint16_t DEFAULT_ROBOT_INTERFACE_PORT = 30003;

template <typename T>
void handleParamError(const char *paramName, T &value, const T &defaultValue) {
  std::ostringstream ostr;
  ostr << "Param: [" << paramName << "] has invalid value: [" << value
  << "]. Overriding with default value: [" << defaultValue << "]";
  LOGR("%s", ostr.str().c_str());

  value = defaultValue;
}
}

void RoboChessRos2Params::print() const {
  std::ostringstream ostr;
  ostr << "==================================================================\n"
       << "Printing node(" << NODE_NAME << ") params:\n"
       << ROS2_EXECUTOR_TYPE_PARAM_NAME << ": " <<
         getExecutorName(ros2CommunicatorCfg.executorType) << '\n'
       << ROS2_EXECUTOR_THREADS_NUM_PARAM_NAME << ": "
         << ros2CommunicatorCfg.numberOfThreads << '\n'
       << ROBOT_IP_PARAM_NAME << ": " << robotIp << '\n'
       << ROBOT_INTERFACE_PORT_PARAM_NAME << ": " << robotInterfacePort << '\n'
       << "=================================================================\n";

  LOG("%s", ostr.str().c_str());
}

void RoboChessRos2Params::validate() {
  const size_t maxHardwareThreads = std::thread::hardware_concurrency();
  if (ros2CommunicatorCfg.numberOfThreads > maxHardwareThreads) {
    handleParamError(ROS2_EXECUTOR_THREADS_NUM_PARAM_NAME,
        ros2CommunicatorCfg.numberOfThreads, maxHardwareThreads);
  }

  //TODO validate ip and port
}

RoboChessRos2ParamProvider::RoboChessRos2ParamProvider()
    : rclcpp::Node(NODE_NAME) {
  declare_parameter<int32_t>(ROS2_EXECUTOR_TYPE_PARAM_NAME,
      DEFAULT_EXECUTOR_TYPE);
  declare_parameter<int32_t>(ROS2_EXECUTOR_THREADS_NUM_PARAM_NAME,
      DEFAULT_EXECUTOR_THREADS_NUM);

  declare_parameter<std::string>(ROBOT_IP_PARAM_NAME, DEFAULT_ROBOT_IP);
  declare_parameter<uint16_t>(ROBOT_INTERFACE_PORT_PARAM_NAME,
      DEFAULT_ROBOT_INTERFACE_PORT);
}

RoboChessRos2Params RoboChessRos2ParamProvider::getParams() {
  int32_t executorTypeInt{};
  get_parameter(ROS2_EXECUTOR_TYPE_PARAM_NAME, executorTypeInt);
  _params.ros2CommunicatorCfg.executorType =
      toEnum<ExecutorType>(executorTypeInt);
  get_parameter(ROS2_EXECUTOR_THREADS_NUM_PARAM_NAME,
                _params.ros2CommunicatorCfg.numberOfThreads);

  get_parameter(ROBOT_IP_PARAM_NAME, _params.robotIp);
  get_parameter(ROBOT_INTERFACE_PORT_PARAM_NAME, _params.robotInterfacePort);

  _params.validate();

  return _params;
}
