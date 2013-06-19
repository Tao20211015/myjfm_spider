#ifndef _TASK_H_
#define _TASK_H_

#include "config.h"
#include "shared.h"

_START_MYJFM_NAMESPACE_

// we want this functor class to be the interface 
// that the threads use to execute
class Task : public Shared {
public:
  virtual int operator()(void* arg = NULL) = 0;
  virtual ~Task() {}
};

_END_MYJFM_NAMESPACE_

#endif

