# Author Zhivko Petrov

cmake_minimum_required(VERSION 3.10)

project(robo_chess_interfaces)

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
    "srv/RobotAbortMotion.srv"
    "srv/RobotChessMove.srv"
    "srv/RobotPark.srv"
    "srv/RobotSetParkMode.srv"
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

ament_export_dependencies(
	rosidl_default_runtime
)

ament_package()
