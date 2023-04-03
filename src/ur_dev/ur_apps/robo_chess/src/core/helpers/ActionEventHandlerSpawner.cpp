//Corresponding header
#include "robo_chess/core/helpers/ActionEventHandlerSpawner.h"

//System headers

//Other libraries headers
#include "utils/ErrorCode.h"
#include "utils/Log.h"

//Own components headers

ActionEventHandlerSpawner::~ActionEventHandlerSpawner() noexcept {
  _actionEventHandler.deinit();
}

ErrorCode ActionEventHandlerSpawner::init(ActionEventHandlerPolicy policy) {
  if (ErrorCode::SUCCESS != _actionEventHandler.init(policy)) {
    LOGERR("Error, ActionEventHandler.init() failed");
    return ErrorCode::FAILURE;
  }

  return ErrorCode::SUCCESS;
}

void ActionEventHandlerSpawner::spawnAndRun() {
  _thread = std::thread(&ActionEventHandlerSpawner::run, this);
  _isActive = true;
}

void ActionEventHandlerSpawner::shutdown() {
  _isActive = false;
  _actionEventHandler.shutdown();
  _thread.join();
}

void ActionEventHandlerSpawner::invokeActionEvent(
  const ActionEventCb& cb, ActionEventType eventType) {
  _actionEventHandler.invokeActionEvent(cb, eventType);
}

void ActionEventHandlerSpawner::run() {
  while (_isActive) {
    using namespace std::literals;

    //entering non-busy interruptable loop
    _actionEventHandler.processStoredEvents(60s);
  }
}
