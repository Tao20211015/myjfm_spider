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
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#include "config.h"
#include "global.h"
#include "url.h"
#include "md5.h"
#include "squeue.h"
#include "shared_pointer.h"
#include "dnser_task.h"
#include "adns.h"

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

  if (adns_init(&_dns_state, 
      (adns_initflags)(adns_if_nosigpipe | adns_if_noerrprint), NULL)) {
    return S_FAIL;
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

    Vector<uint32_t> ips;
    RES_CODE res_code = _dns_cache->find(site, ips);
    if (res_code == S_OK) {
      url_p->set_ip(ips);
      put_url_into_scheduler(url_p);
      continue;
    }

    bool flag = false;
    if (_is_dnsing_lutable[site].size() == 0) { // need adns_submit
      Vector<SharedPointer<Url> > v;
      v.push_back(url_p);
      _is_dnsing_lutable[site] = v;
      flag = true;
    } else { // do not need adns_submit
      _is_dnsing_lutable[site].push_back(url_p);
    }

    if (flag) {
      adns_query query = NULL;
      adns_status status = (adns_status)adns_submit(_dns_state, site.c_str(), 
          (adns_rrtype)adns_r_a, adns_qf_none, 
          &(_is_dnsing_lutable[site]), &query);
      if (status) { // submit error
        continue;
      }
    }

    adns_query new_query = NULL;
    adns_answer *answer;
    Vector<SharedPointer<Url> > *urls;
    int res = adns_check(_dns_state, &new_query, &answer, (void**)&urls);
    if (res == EAGAIN || res == ESRCH) {
      continue;
    }

    if (answer->status == adns_s_ok) {
      char* strip;
      uint32_t ip;
      Vector<uint32_t> ips;
      const char* rrtype_name;
      const char* formattype_name;
      int len;
      adns_rr_info(answer->type, &rrtype_name, 
          &formattype_name, &len, 0, 0);
      int i;
      for (i = 0; i < answer->nrrs; ++i) {
        adns_rr_info(answer->type, 0, 0, 0, 
            answer->rrs.bytes + i * len, &strip);
        inet_pton(AF_INET, strip, &ip);
        ips.push_back(ip);
      }

      Vector<SharedPointer<Url> >::iterator itr = urls->begin();
      (*itr)->get_site(site);
      for (; itr != urls->end(); ++itr) {
        (*itr)->set_ip(ips);
        put_url_into_scheduler((*itr));
      }

      // remove from temporary lookup table
      _is_dnsing_lutable.erase(site);
      // update dns cache
      _dns_cache->insert(site, ips);
    } else {
      // remove from temporary lookup table
      _is_dnsing_lutable.erase(site);
    }

    free(answer);
  }

  return S_OK;
}

RES_CODE DnserTask::put_url_into_scheduler(SharedPointer<Url>& url_p) {
  return S_OK;
}

_END_MYJFM_NAMESPACE_

