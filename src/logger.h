#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "config.h"
#include "mutex.h"
#include "semaphore.h"
#include "task.h"
#include "thread.h"
#include "shared_pointer.h"

_START_MYJFM_NAMESPACE_

struct Message {
  LOG_LEVEL _level;
  String _msg;
};

class Logger {
  friend class LoggerTask;

#define DEFAULT_THRESHOLD 10000

public:
  Logger(int threshold = DEFAULT_THRESHOLD);
  ~Logger();
  RES_CODE init();
  RES_CODE log(Message&);

private:
  Logger(const Logger&);
  Logger& operator=(const Logger&);

  Ofstream* _log;
  Ofstream* _err;

  int _threshold;
  Vector<Message> _primary;
  Vector<Message> _secondary;
  Mutex _mutex;
  Semaphore _semaphore;
  SharedPointer<Thread> _thread;
};

_END_MYJFM_NAMESPACE_

#endif

