# Author Zhivko Petrov

cmake_minimum_required(VERSION 3.10)

project(
    robo_chess
    LANGUAGES CXX
    VERSION 2.0.0
)

find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(cmake_helpers REQUIRED)
find_package(ur_control_common REQUIRED)
find_package(ros2_game_engine REQUIRED)
find_package(robo_chess_common REQUIRED)

set(_RESOURCE_FOLDER_NAME resources)
set(_INC_FOLDER_NAME include)
set(_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${_INC_FOLDER_NAME}/${PROJECT_NAME})
set(_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(
    ${PROJECT_NAME}
		${_INC_DIR}/config/RoboChessConfigGenerator.h
        ${_INC_DIR}/config/RoboChessConfig.h
		${_INC_DIR}/core/RoboChessApplication.h
        ${_INC_DIR}/core/helpers/RoboChessApplicationInitHelper.h
        ${_INC_DIR}/core/helpers/ActionEventHandlerSpawner.h
        ${_INC_DIR}/core/helpers/RobotModeHandler.h
		${_INC_DIR}/external_api/RoboChessExternalInterface.h
		${_INC_DIR}/external_api/RoboChessRos2ParamProvider.h
        ${_INC_DIR}/motion/ChessMoveMotionSequence.h
        ${_INC_DIR}/motion/ChessMoveMotionSequence.h
        ${_INC_DIR}/motion/ParkMotionSequence.h
        ${_INC_DIR}/motion/config/ParkMotionSequenceConfig.h
        ${_INC_DIR}/motion/config/RoboChessMotionSequenceConfig.h
        ${_INC_DIR}/motion/config/TurnEndMotionSequenceConfig.h
        ${_INC_DIR}/motion/config/RoboChessMotionSequenceConfig.h
		${_INC_DIR}/defines/RoboChessDefines.h
        ${_INC_DIR}/defines/RoboChessFunctionalDefines.h
     
		${_SRC_DIR}/config/RoboChessConfigGenerator.cpp
		${_SRC_DIR}/core/RoboChessApplication.cpp
        ${_SRC_DIR}/core/helpers/RoboChessApplicationInitHelper.cpp
        ${_SRC_DIR}/core/helpers/ActionEventHandlerSpawner.cpp
        ${_SRC_DIR}/core/helpers/RobotModeHandler.cpp
		${_SRC_DIR}/external_api/RoboChessExternalInterface.cpp
		${_SRC_DIR}/external_api/RoboChessRos2ParamProvider.cpp
        ${_SRC_DIR}/motion/ChessMoveMotionSequence.cpp
        ${_SRC_DIR}/motion/ParkMotionSequence.cpp
        ${_SRC_DIR}/motion/TurnEndMotionSequence.cpp
		${_SRC_DIR}/main.cpp
)

target_include_directories(
    ${PROJECT_NAME} 
    PRIVATE
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}/${_INC_FOLDER_NAME}
            ${CMAKE_CURRENT_SOURCE_DIR}/${_RESOURCE_FOLDER_NAME}
        >
        $<INSTALL_INTERFACE:
            ${_INC_FOLDER_NAME}
            ${_RESOURCE_FOLDER_NAME}
        >
) 

target_link_libraries(
    ${PROJECT_NAME}
        PRIVATE
            ros2_game_engine::ros2_game_engine
            ur_control_common::ur_control_common
            robo_chess_common::robo_chess_common
)

ament_target_dependencies(
    ${PROJECT_NAME}
		PUBLIC
            ament_cmake
            ament_index_cpp
            robo_chess_common
)

if(NOT DISABLE_ROS_TOOLING)
    # Ament uses non-monolith build (separate build and install steps).
    # The helpers.cmake has to be included manually.
    # This allows access to the helpers.cmake user defined macros
    # In the case of monolith build - the file is automatically included
    # from the cmake_helpersConfig.cmake when the package is consumed by find_package()
	include(${cmake_helpers_DIR}/helpers.cmake)
	
    enable_ros_tooling_for_target(
        ${PROJECT_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/package.xml
    )
endif()

set_target_cpp_standard(${PROJECT_NAME} 20)
enable_target_warnings(${PROJECT_NAME})
set_target_visibility(${PROJECT_NAME})
# enable_target_sanitizer(${PROJECT_NAME} "thread")

# Install resources (gripper definitions)
install(
    DIRECTORY ${_RESOURCE_FOLDER_NAME}/
    DESTINATION share/${PROJECT_NAME}/${_RESOURCE_FOLDER_NAME}/
)