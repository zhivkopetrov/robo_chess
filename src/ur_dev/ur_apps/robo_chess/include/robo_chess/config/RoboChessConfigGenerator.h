#ifndef ROBO_CHESS_ROBOCHESSCONFIGGENERATOR_H_
#define ROBO_CHESS_ROBOCHESSCONFIGGENERATOR_H_

//System headers
#include <cstdint>
#include <vector>

//Other libraries headers
#include "game_engine/defines/DependencyDefines.h"

//Own components headers
#include "robo_chess/config/RoboChessConfig.h"

//Forward declarations

class RoboChessConfigGenerator {
public:
  RoboChessConfigGenerator() = delete;

  static std::vector<DependencyDescription> generateDependencies(int32_t argc,
                                                                 char **args);

  static RoboChessConfig generateConfig();
};

#endif /* ROBO_CHESS_ROBOCHESSCONFIGGENERATOR_H_ */
