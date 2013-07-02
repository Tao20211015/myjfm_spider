#include "config.h"
#include "log.h"
#include "global.h"
#include "logger.h"

extern _MYJFM_NAMESPACE_::Global* glob;

_START_MYJFM_NAMESPACE_

RES_CODE LOG(LOG_LEVEL level, const char* str) {
  Message msg = {level, str};
  Logger* logger = NULL;
  glob->get_logger(logger);
  return logger->log(msg);
}

_END_MYJFM_NAMESPACE_

