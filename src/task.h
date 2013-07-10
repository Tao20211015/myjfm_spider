/*******************************************************************************
 * task.h - The thread execution body implementation
 * all real tasks that thread executes must inherited from it
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _TASK_H_
#define _TASK_H_

#include "config.h"
#include "shared.h"

_START_MYJFM_NAMESPACE_

// we want this functor class to be the interface 
// that the threads use to execute
class Task : public Shared {
public:
  virtual RES_CODE operator()(void* arg = NULL) = 0;
  virtual ~Task() {}
};

_END_MYJFM_NAMESPACE_

#endif

