/*******************************************************************************
 * log.cc - the log function entry
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#include "config.h"
#include "log.h"
#include "global.h"
#include "logger.h"

extern _MYJFM_NAMESPACE_::Global* glob;

RES_CODE LOG(LOG_LEVEL level, const char* fmt, ...) {
  time_t now = time(NULL);
  va_list ap;
  char buffer[MAX_LOG_LEN];

  strftime(buffer, MAX_LOG_LEN, "[%d %b %H:%M:%S] ", localtime(&now));
  char *log = buffer + strlen(buffer);

  va_start(ap, fmt);
  vsnprintf(log, MAX_LOG_LEN - strlen(buffer), fmt, ap);
  va_end(ap);

  _MYJFM_NAMESPACE_::Message msg = {level, buffer};

  _MYJFM_NAMESPACE_::Logger* logger = NULL;
  glob->get_logger(logger);

  return logger->log(msg);
}

