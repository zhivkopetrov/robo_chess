#ifndef ROBO_CHESS_ACTIONEVENTHANDLERSPAWNER_H_
#define ROBO_CHESS_ACTIONEVENTHANDLERSPAWNER_H_

//System headers
#include <thread>
#include <atomic>

//Other libraries headers
#include "game_engine/event_handler/ActionEventHandler.h"
#include "utils/class/NonCopyable.h"
#include "utils/class/NonMoveable.h"
#include "utils/ErrorCode.h"

//Own components headers

//Forward declarations

class ActionEventHandlerSpawner : public NonCopyable, public NonMoveable {
public:
  ~ActionEventHandlerSpawner() noexcept;

  ErrorCode init(ActionEventHandlerPolicy policy);

  void spawnAndRun();

  void shutdown();

  //forwards to ActionEventHandler in a thread-safe manner
  void invokeActionEvent(const ActionEventCb& cb, ActionEventType eventType);

private:
  void run();

  ActionEventHandler _actionEventHandler;
  std::thread _thread;
  std::atomic<bool> _isActive = false;
};

#endif /* ROBO_CHESS_ACTIONEVENTHANDLERSPAWNER_H_ */
