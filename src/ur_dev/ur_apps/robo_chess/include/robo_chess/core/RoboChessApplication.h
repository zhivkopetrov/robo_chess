#ifndef ROBO_CHESS_ROBOCHESSAPPLICATION_H_
#define ROBO_CHESS_ROBOCHESSAPPLICATION_H_

//System headers
#include <vector>
#include <memory>

//Other libraries headers
#include "ros2_game_engine/communicator/Ros2Communicator.h"
#include "game_engine/defines/DependencyDefines.h"
#include "utils/class/NonCopyable.h"
#include "utils/class/NonMoveable.h"
#include "utils/ErrorCode.h"

//Own components headers
#include "robo_chess/core/config/RoboChessConfig.h"
#include "robo_chess/external_api/RoboChessExternalInterface.h"

//Forward declarations

class RoboChessApplication : public NonCopyable, public NonMoveable {
public:
  ~RoboChessApplication() noexcept;

  ErrorCode loadDependencies(
      const std::vector<DependencyDescription> &dependencies);

  ErrorCode init(const RoboChessConfig &cfg);

  ErrorCode run();

private:
  void deinit();
  void unloadDependencies();

  std::vector<DependencyDescription> _dependencies;

  std::unique_ptr<Ros2Communicator> _communicator;
  std::shared_ptr<RoboChessExternalInterface> _externalInterface;
};

#endif /* ROBO_CHESS_ROBOCHESSAPPLICATION_H_ */
