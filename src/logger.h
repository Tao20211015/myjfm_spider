#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "config.h"
#include "mutex.h"
#include "task.h"

_START_MYJFM_NAMESPACE_

struct Message {
  LOG_LEVEL _level;
  String _msg;
};

class Logger {
  friend class Loggertask;

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
};

_END_MYJFM_NAMESPACE_

#endif

