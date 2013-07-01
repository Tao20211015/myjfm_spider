#include "config.h"
#include "global.h"
#include "log.h"

extern _MYJFM_NAMESPACE_::Global *glob;

_START_MYJFM_NAMESPACE_

void LOG(LOG_LEVEL level, const char* msg) {
  switch (level) {
    case INFO:
      (*(glob->_log)) << msg << Endl;
      break;
    case WARNING:
    case ERROR:
    case FATAL:
      (*(glob->_err)) << msg << Endl;
      break;
    default:
      break;
  }
}

_END_MYJFM_NAMESPACE_

