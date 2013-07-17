/*******************************************************************************
 * extractor_task.h - the extractor module implementation
 * each extractor is a thread. It analysises the web pages and extracts all the
 * http urls, then put the urls in the global synchronous url queue.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _EXTRACTOR_TASK_H_
#define _EXTRACTOR_TASK_H_

#include <stdint.h>

#include "config.h"
#include "task.h"

_START_MYJFM_NAMESPACE_

class ExtractorTask : public Task {
public:
  ExtractorTask(uint32_t id);
  ~ExtractorTask();
  virtual RES_CODE operator()(void* arg = NULL);

private:
  uint32_t _id;
};

_END_MYJFM_NAMESPACE_

#endif

