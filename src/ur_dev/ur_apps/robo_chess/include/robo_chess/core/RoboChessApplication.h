#ifndef ROBO_CHESS_ROBOCHESSAPPLICATION_H_
#define ROBO_CHESS_ROBOCHESSAPPLICATION_H_

//System headers
#include <vector>
#include <memory>

//Other libraries headers
#include "ur_control_common/external_api/UrControlCommonExternalBridge.h"
#include "ur_control_common/external_api/DashboardProvider.h"
#include "ur_control_common/motion/MotionExecutor.h"
#include "urscript_common/urscript/UrScriptBuilder.h"
#include "ros2_game_engine/communicator/Ros2Communicator.h"
#include "game_engine/defines/DependencyDefines.h"
#include "utils/class/NonCopyable.h"
#include "utils/class/NonMoveable.h"
#include "utils/ErrorCode.h"

//Own components headers
#include "robo_chess/config/RoboChessConfig.h"
#include "robo_chess/core/helpers/ActionEventHandlerSpawner.h"
#include "robo_chess/core/helpers/RobotModeHandler.h"
#include "robo_chess/external_api/RoboChessExternalInterface.h"

//Forward declarations

class RoboChessApplication : public NonCopyable, public NonMoveable {
public:
  friend class RoboChessApplicationInitHelper;

  ~RoboChessApplication() noexcept;

  ErrorCode loadDependencies(
      const std::vector<DependencyDescription> &dependencies);

  ErrorCode init(const RoboChessConfig &cfg);

  ErrorCode run();

private:
  void deinit();
  void unloadDependencies();

  std::vector<DependencyDescription> _dependencies;
  ActionEventHandlerSpawner _actionEventHandlerSpawner;
  RobotModeHandler _robotModeHandler;
  MotionExecutor _motionExecutor;
  std::shared_ptr<UrScriptBuilder> _urScriptBuilder;

  //ROS2 related objects
  std::unique_ptr<Ros2Communicator> _communicator;
  std::shared_ptr<DashboardProvider> _dashboardProvider;
  std::shared_ptr<RoboChessExternalInterface> _roboChessExternalInterface;
  std::shared_ptr<UrControlCommonExternalBridge> _urControlExternalInterface;
};

#endif /* ROBO_CHESS_ROBOCHESSAPPLICATION_H_ */
