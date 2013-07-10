/*******************************************************************************
 * logger_task.cc - the logger task implementation
 * when the number of the messages are more than 10000, then
 * start another thread to write the messages on disk. the thread executes the
 * logger task to really write the messages on disk.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include "config.h"
#include "logger_task.h"

_START_MYJFM_NAMESPACE_

LoggerTask::LoggerTask(Logger* const logger) : _logger(logger) {}

LoggerTask::~LoggerTask() {}

RES_CODE LoggerTask::operator()(void* arg) {
  if (!_logger || !(_logger->_log) || !(_logger->_err)) {
    return S_BAD_ARG;
  }

  Vector<Message>::iterator itr;

  for (;;) {
    (_logger->_semaphore).wait();

    for (itr = (_logger->_secondary).begin(); 
        itr != (_logger->_secondary).end(); ++itr) {
      switch(itr->_level) {
        case INFO:
          (*(_logger->_log)) << "[INFO] " << itr->_msg << Endl;
          break;
        case WARNING:
          (*(_logger->_err)) << "[WARNING] " << itr->_msg << Endl;
          break;
        case ERROR:
          (*(_logger->_err)) << "[ERROR] " << itr->_msg << Endl;
          break;
        case FATAL:
          (*(_logger->_err)) << "[FATAL] " << itr->_msg << Endl;
          break;
        default:
          break;
      }
    }

    _logger->_secondary.clear();
  }

  return S_OK;
}

_END_MYJFM_NAMESPACE_

