#ifndef _THREAD_H_
#define _THREAD_H_

#include "config.h"
#include "shared.h"
#include "runner.h"
#include "sharepointer.h"
#include <pthread.h>

_START_MYJFM_NAMESPACE_

// the function body that the thread will use to execute
typedef void* (*thread_run_func)(void*);

class Thread : public Shared {
  enum _enum_state {
    INIT, 
    START, 
    JOINED, 
    STOP
  };

public:
  // run the runner
  Thread(const Sharepointer<Runner>& runner, int detached = 0);

  // run the function body
  Thread(thread_run_func, void* arg, int detached = 0);

  ~Thread();

  // start the execution
  int start();

  // stop the execution
  int stop();

  // wait this thread until exiting
  int join();

  // get the thread id via pthread_self()
  pthread_t gettid() const;

private:
  set_detached();

  int _use_runner;
  Sharepointer<Runner> _runner;

  thread_run_func _run_func;
  void* func_arg;

  pthread_t _pid;
  _enum_state _state;
  int _detached;
};

_END_MYJFM_NAMESPACE_

#endif

