/*******************************************************************************
 * dnser_task.h - the dnser module implementation
 * each dnser is a thread. It is responsible for query IP of one website
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _DNSER_TASK_H_
#define _DNSER_TASK_H_

#include <stdint.h>

#include "config.h"
#include "task.h"
#include "shared_pointer.h"
#include "squeue.h"
#include "url.h"
#include "dns_cache.h"
#include "adns.h"

_START_MYJFM_NAMESPACE_

class DnserTask : public Task {
public:
  DnserTask(uint32_t);
  ~DnserTask();
  virtual RES_CODE operator()(void* arg = NULL);

private:
  uint32_t _id;
  uint32_t _scheduler_num;
  adns_state _dns_state;
  SharedPointer<DnsCache> _dns_cache;
  Map<String, Vector<SharedPointer<Url> > > _is_dnsing_lutable;
  SharedPointer<SQueue<SharedPointer<Url> > > _url_queue;
  Vector<SharedPointer<SQueue<SharedPointer<Url> > > > _scheduler_queues;

  RES_CODE init();
  RES_CODE init_dns_cache();
  RES_CODE main_loop();
  RES_CODE put_url_into_scheduler(SharedPointer<Url>&);
};

_END_MYJFM_NAMESPACE_

#endif

