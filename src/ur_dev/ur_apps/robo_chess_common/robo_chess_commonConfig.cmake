include(CMakeFindDependencyMacro)

#find_dependency will correctly forward REQUIRED or QUIET args to the consumer
#find_package is only for internal use
find_dependency(ament_cmake REQUIRED)
find_dependency(cmake_helpers REQUIRED)
find_dependency(utils REQUIRED)
find_dependency(robo_chess_interfaces REQUIRED)

if(NOT TARGET robo_chess_common::robo_chess_common)
  include(${CMAKE_CURRENT_LIST_DIR}/robo_chess_commonTargets.cmake)
endif()

# This is for catkin compatibility.
set(robo_chess_common_LIBRARIES robo_chess_common::robo_chess_common)

get_target_property(
    robo_chess_common_INCLUDE_DIRS
    robo_chess_common::robo_chess_common
    INTERFACE_INCLUDE_DIRECTORIES
)

