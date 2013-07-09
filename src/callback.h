#ifndef _CALLBACK_H_
#define _CALLBACK_H_

#include "config.h"
#include "shared.h"

_START_MYJFM_NAMESPACE_

class Callback : public Shared {
public:
  virtual RES_CODE operator()(void* arg = NULL) = 0;
  virtual ~Callback() {}
};

_END_MYJFM_NAMESPACE_

#endif

