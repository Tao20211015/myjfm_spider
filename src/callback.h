/*******************************************************************************
 * callback.h - The event-driven callback base class implementation
 * At present, we do not use event-driven mechanism to set up the skeleton of 
 * our software. So, the callback class is not in use.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

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

