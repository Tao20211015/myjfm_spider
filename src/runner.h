#ifndef _RUNNER_H_
#define _RUNNER_H_

#include "config.h"
#include "shared.h"

_START_MYJFM_NAMESPACE_

// we want this functor class to be the interface 
// that the threads use to execute
class Runner : public Shared {
public:
  virtual int operator()(void* param = NULL) = 0;
  virtual ~Runner() {}
};

_END_MYJFM_NAMESPACE_

#endif

