#ifndef _LOG_H_
#define _LOG_H_

#include "config.h"

_START_MYJFM_NAMESPACE_

enum LOG_LEVEL {
  LOG_DUMMY = 0x0, 
  INFO = 0x1, 
  ERROR = 0x2, 
  WARNING = 0x4, 
  FATAL = 0x8
};

inline void LOG(LOG_LEVEL, String&);

_END_MYJFM_NAMESPACE_

#endif

