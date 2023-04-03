//Corresponding header
#include "robo_chess/core/helpers/RobotModeHandler.h"

//System headers

//Other libraries headers
#include "utils/Log.h"

//Own components headers

void RobotModeHandler::changeRobotMode(RobotMode mode) {
  const auto robotModeCStr = toString(mode).c_str();

  LOG_ON_SAME_LINE("[RobotModeHandler] received Robot Mode: [");
  switch (mode) {
  case RobotMode::Idle:
    LOGY_ON_SAME_LINE("%s", robotModeCStr);
    break;

  case RobotMode::Running:
    LOGG_ON_SAME_LINE("%s", robotModeCStr);
    break;

  default:
    LOG_ON_SAME_LINE("%s", robotModeCStr);
    break;
  }

  LOG("]");
}

void RobotModeHandler::changeSafetyMode(SafetyMode mode) {
  const auto safetyModeCStr = toString(mode).c_str();

  LOG_ON_SAME_LINE("[RobotModeHandler] received Safety Mode: [");
  switch (mode) {
  case SafetyMode::Unknown:
    LOG_ON_SAME_LINE("%s", safetyModeCStr);
    break;

  case SafetyMode::Normal:
    LOGG_ON_SAME_LINE("%s", safetyModeCStr);
    break;

  case SafetyMode::Reduced:
    LOGY_ON_SAME_LINE("%s", safetyModeCStr);
    break;

  default:
    LOGR_ON_SAME_LINE("%s", safetyModeCStr);
    break;
  }

  LOG("]");
}