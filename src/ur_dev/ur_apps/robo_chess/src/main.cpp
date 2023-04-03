//System headers
#include <cstdint>

//Other libraries headers
#include "utils/Log.h"

//Own components headers
#include "robo_chess/core/RoboChessApplication.h"
#include "robo_chess/config/RoboChessConfigGenerator.h"

int32_t main(int32_t argc, char **args) {
  RoboChessApplication app;

  const auto dependencies = RoboChessConfigGenerator::generateDependencies(
      argc, args);
  if (ErrorCode::SUCCESS != app.loadDependencies(dependencies)) {
    LOGERR("app.loadDependencies() failed");
    return EXIT_FAILURE;
  }

  const auto cfg = RoboChessConfigGenerator::generateConfig();
  if (ErrorCode::SUCCESS != app.init(cfg)) {
    LOGERR("app.init() failed");
    return EXIT_FAILURE;
  }

  if (ErrorCode::SUCCESS != app.run()) {
    LOGERR("app.run() failed");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
