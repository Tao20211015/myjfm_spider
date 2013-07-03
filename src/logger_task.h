#ifndef _LOGGER_TASK_H_
#define _LOGGER_TASK_H_

#include "config.h"
#include "task.h"
#include "logger.h"

_START_MYJFM_NAMESPACE_

class LoggerTask : public Task {
public:
  LoggerTask(Logger* const);
  ~LoggerTask();
  virtual RES_CODE operator()(void* arg = NULL);

private:
  Logger* _logger;
};

_END_MYJFM_NAMESPACE_

#endif

