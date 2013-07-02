#ifndef _LOGGERTASK_H_
#define _LOGGERTASK_H_

#include "config.h"
#include "task.h"
#include "logger.h"

_START_MYJFM_NAMESPACE_

class Loggertask : public Task {
public:
  Loggertask(Logger* const);
  ~Loggertask();
  virtual RES_CODE operator()(void* arg = NULL);

private:
  Logger* _logger;
};

_END_MYJFM_NAMESPACE_

#endif

