/*******************************************************************************
 * dnser_task.h - the dnser module implementation
 * each dnser is a thread. It is responsible for query IP of one website
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
#include "dnser_task.h"

extern _MYJFM_NAMESPACE_::Global* glob;

_START_MYJFM_NAMESPACE_

DnserTask::DnserTask(uint32_t id) : 
  _id(id), 
  _scheduler_num(0), 
  _url_queue(NULL) {
  _scheduler_queues.clear();
}

DnserTask::~DnserTask() {}

RES_CODE DnserTask::operator()(void* arg) {
  if (init() != S_OK) {
    return S_FAIL;
  }

  return main_loop();
}

RES_CODE DnserTask::init() {
  if (glob->get_scheduler_num(_scheduler_num) != S_OK) {
    return S_FAIL;
  }

  if (glob->get_dnser_queue(_id, _url_queue) != S_OK) {
    return S_FAIL;
  }

  uint32_t i;
  for (i = 0; i < _scheduler_num; ++i) {
    SharedPointer<SQueue<SharedPointer<Url> > > tmp_q;
    glob->get_scheduler_queue(i, tmp_q);
    _scheduler_queues.push_back(tmp_q);
  }

  return S_OK;
}

RES_CODE DnserTask::main_loop() {
  for (;;) {
    LOG(INFO, "Dnser[%d] fetch one url", _id);

    SharedPointer<Url> url_p;
    _url_queue->pop(url_p);

    if (url_p.is_null()) {
      continue;
    }

    String site;
    Url::EnumStatus status = Url::UNINITIALIZED;

    // if the url has not been initialized, drop it
    url_p->get_status(status);
    if (status != Url::INITIALIZED) {
      continue;
    }

    url_p->get_site(site);

  }

  /*
  for (;;) {
    LOG(INFO, "#Dnser# %d", _id);
    usleep(5000);
  }
  */

  return S_OK;
}

_END_MYJFM_NAMESPACE_

