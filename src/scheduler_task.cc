/*******************************************************************************
 * scheduler_task.cc - the scheduler module implementation
 * each scheduler is a thread. It is responsible for the url scheduling and all
 * the other jobs.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "global.h"
#include "url.h"
#include "md5.h"
#include "squeue.h"
#include "shared_pointer.h"
#include "scheduler_task.h"

extern _MYJFM_NAMESPACE_::Global* glob;

_START_MYJFM_NAMESPACE_

SchedulerTask::SchedulerTask(uint32_t id) : 
  _id(id), 
  _downloader_num(0), 
  _url_queue(NULL) {
  _downloader_queues.clear();
}

SchedulerTask::~SchedulerTask() {}

RES_CODE SchedulerTask::operator()(void* arg) {
  if (init() != S_OK) {
    return S_FAIL;
  }

  for (;;) {
    // select one url
    SharedPointer<Url> url_p;
    _url_queue->pop(url_p);
    if (url_p.is_null()) {
      continue;
    }

    //put the url into the downloader's queue
    put_url_into_downloader(url_p);
  }

  return S_OK;
}

RES_CODE SchedulerTask::init() {
  if (glob->get_downloader_num(_downloader_num) != S_OK) {
    return S_FAIL;
  }

  if (glob->get_scheduler_queue(_id, _url_queue) != S_OK) {
    return S_FAIL;
  }

  uint32_t i;
  for (i = 0; i < _downloader_num; ++i) {
    SharedPointer<SQueue<SharedPointer<Url> > > tmp_q;
    glob->get_downloader_queue(i, tmp_q);
    _downloader_queues.push_back(tmp_q);
  }

  return S_OK;
}

RES_CODE SchedulerTask::put_url_into_downloader(SharedPointer<Url>& url_p) {
  MD5 md5;
  if (url_p->get_md5(md5) != S_OK) {
    return S_FAIL;
  }

  uint32_t index = 0;
  if (md5.shuffle(_downloader_num, index) != S_OK) {
    return S_FAIL;
  }

  _downloader_queues[index]->push(url_p);

  return S_OK;
}

_END_MYJFM_NAMESPACE_

