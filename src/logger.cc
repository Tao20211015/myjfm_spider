/*******************************************************************************
 * logger.cc - the log class which implement the log function
 * we use two buffer to cache the log message, one is used to receive from 
 * all other modules, when the number of the messages is more than 10000, then
 * dump all the messages into the second buffer. And, start another thread to 
 * write all the messages on disk.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <stdlib.h>

#include "config.h"
#include "log.h"
#include "logger.h"
#include "logger_task.h"
#include "global.h"
#include "shared_pointer.h"
#include "thread.h"

extern _MYJFM_NAMESPACE_::Global *glob;

_START_MYJFM_NAMESPACE_

Logger::Logger(int threshold) : 
  _log(NULL), 
  _err(NULL), 
  _threshold(threshold), 
  _thread(NULL) {
  _primary.clear();
  _secondary.clear();
}

// when destruct the logger, all the child threads(the downloaders, 
// the extractors and the schedulers) have stopped.
// So only the main thread and the logger thread are living.
Logger::~Logger() {
  int i;
  for (i = 0; i < 5; ++i) {
    if (_secondary.size() <= 0) {
      break;
    }

    sleep(2);
  }

  // write the log in the _primary buffer onto the disk
  if (i < 5) {
    _primary.swap(_secondary);
    _semaphore.post();
  }

  // we assume that writing all the remaining logs in the _primary buffer onto 
  // the disk will take less than 10 seconds
  for (i = 0; i < 5; ++i) {
    if (_secondary.size() <= 0) {
      break;
    }

    sleep(2);
  }

  // wait for the thread stopping
  _thread->stop_blocking();

  if (_log && _log != (Ofstream*)&Cout) {
    _log->close();
    delete _log;
    _log = NULL;
  }

  if (_err && _err != (Ofstream*)&Cerr) {
    _err->close();
    delete _err;
    _err = NULL;
  }
}

RES_CODE Logger::init() {
  _primary.clear();
  _secondary.clear();

  String log_path;
  glob->get_log_path(log_path);
  log_path += "/log.txt";

  _log = new Ofstream();
  _log->open(log_path.c_str(), Ofstream::out | Ofstream::trunc);

  if (!(*_log)) {
    Cerr << "[WARNING] Can not create log file: " << log_path << Endl;
    Cerr << "[WARNING] Will create it on current path." << Endl;

    glob->get_cur_path(log_path);
    glob->set_log_path(log_path);
    log_path += "/log.txt";
    _log->open(log_path.c_str(), Ofstream::out | Ofstream::trunc);

    if (!(*_log)) {
      Cerr << "[WARNING] Can not create log file: " << log_path << Endl;
      Cerr << "[WARNING] Will use std::cout." << Endl;

      log_path = "";
      glob->set_log_path(log_path);
      delete _log;
      _log = (Ofstream*)&Cout;
    }
  }

  String err_path;
  glob->get_err_path(err_path);
  err_path += "/err.txt";

  _err = new Ofstream();
  _err->open(err_path.c_str(), Ofstream::out | Ofstream::trunc);

  if (!(*_err)) {
    Cerr << "[WARNING] Can not create error file: " << err_path << Endl;
    Cerr << "[WARNING] Will create it on current path." << Endl;

    glob->get_cur_path(err_path);
    glob->set_err_path(err_path);
    err_path += "/err.txt";
    _err->open(err_path.c_str(), Ofstream::out | Ofstream::trunc);

    if (!(*_err)) {
      Cerr << "[WARNING] Can not create error file: " << err_path << Endl;
      Cerr << "[WARNING] Will use std::cout." << Endl;

      err_path = "";
      glob->set_err_path(err_path);
      delete _err;
      _err = (Ofstream*)&Cerr;
    }
  }
  
  SharedPointer<LoggerTask> task(new LoggerTask(this));
  SharedPointer<Thread> thread(new Thread(task));
  _thread = thread;
  
  if (_thread->start() != S_OK) {
    return S_FAILED_TO_LOG_ON_DISK;
  }

  return S_OK;
}

RES_CODE Logger::log(Message& msg) {
  switch (msg._level) {
    case INFO:
      if (_log == (Ofstream*)&Cout) {
        (*_log) << "[INFO] " << msg._msg << Endl;
        return S_OK;
      }
      break;
    case WARNING:
      if (_err == (Ofstream*)&Cerr) {
        (*_err) << "[WARNING] " << msg._msg << Endl;
        return S_OK;
      }
      break;
    case ERROR:
      if (_err == (Ofstream*)&Cerr) {
        (*_err) << "[ERROR] " << msg._msg << Endl;
        return S_OK;
      }
      break;
    case FATAL:
      if (_err == (Ofstream*)&Cerr) {
        (*_err) << "[FATAL] " << msg._msg << Endl;
        return S_OK;
      }
      break;
    default:
      return S_INVALID_LOG;
  }
  
  _mutex.lock();
  _primary.push_back(msg);

  if (_primary.size() > _threshold) {
    // the Writer thread is writing the log on the disk
    // so write it later
    if (_secondary.size() > 0) {
      _mutex.unlock();
      return S_IS_LOGGING;
    }
    
    _primary.swap(_secondary);
    _semaphore.post();
  }

  _mutex.unlock();

  return S_OK;
}

_END_MYJFM_NAMESPACE_

