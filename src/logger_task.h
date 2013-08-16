/*******************************************************************************
 * logger_task.h - the logger task implementation
 * when the number of the messages are more than 10000, then
 * start another thread to write the messages on disk. the thread executes the
 * logger task to really write the messages on disk.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _LOGGER_TASK_H_
#define _LOGGER_TASK_H_

#include "config.h"
#include "task.h"
#include "logger.h"

_START_MYJFM_NAMESPACE_

class LoggerTask : public Task {
public:
  explicit LoggerTask(Logger* const);
  ~LoggerTask();
  virtual RES_CODE operator()(void* arg = NULL);

private:
  Logger* _logger;
};

_END_MYJFM_NAMESPACE_

#endif

