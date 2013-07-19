/*******************************************************************************
 * extractor_task.cc - the extractor module implementation
 * each extractor is a thread. It analysises the web pages and extracts all the
 * http urls, then put the urls in the global synchronous url queue.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "global.h"
#include "shared_pointer.h"
#include "extractor_task.h"
#include "squeue.h"
#include "url.h"
#include "log.h"

extern _MYJFM_NAMESPACE_::Global* glob;

_START_MYJFM_NAMESPACE_

ExtractorTask::ExtractorTask(uint32_t id) : _id(id) {}

ExtractorTask::~ExtractorTask() {}

RES_CODE ExtractorTask::operator()(void* arg) {
  for (;;) {
    LOG(INFO, "#Extractor# %d", _id);
    usleep(1000);
  }

  return S_OK;
}

RES_CODE ExtractorTask::init() {
  if (glob->get_dnser_num(_dnser_num) != S_OK) {
    return S_FAIL;
  }

  if (glob->get_extractor_queue(_id, _page_queue) != S_OK) {
    return S_FAIL;
  }

  uint32_t i;
  for (i = 0; i < _dnser_num; ++i) {
    SharedPointer<SQueue<SharedPointer<Url> > > tmp_q;
    glob->get_dnser_queue(i, tmp_q);
    _dnser_queues.push_back(tmp_q);
  }

  return S_OK;
}

_END_MYJFM_NAMESPACE_

