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

DnserTask::~DnserTask() {
  _scheduler_queues.clear();
}

RES_CODE DnserTask::operator()(void* arg) {
  if (init() != S_OK) {
    return S_FAIL;
  }

  return main_loop();
}

RES_CODE DnserTask::init() {
  if (init_url_queue() != S_OK) {
    return S_FAIL;
  }

  if (init_scheduler_queue() != S_OK) {
    return S_FAIL;
  }

  if (init_dns_cache() != S_OK) {
    return S_FAIL;
  }

  // init adns
  if (adns_init(&_dns_state, 
      (adns_initflags)(adns_if_nosigpipe | adns_if_noerrprint), NULL)) {
    return S_FAIL;
  }

  return S_OK;
}

RES_CODE DnserTask::init_url_queue() {
  return glob->get_dnser_queue(_id, _url_queue);
}

RES_CODE DnserTask::init_scheduler_queue() {
  if (glob->get_scheduler_num(_scheduler_num) != S_OK) {
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

RES_CODE DnserTask::init_dns_cache() {
  return glob->get_dns_cache(_dns_cache);
}

RES_CODE DnserTask::main_loop() {
  int a = 0;
  int b = 0;
  for (;;) {
    String site;
    SharedPointer<Url> url_p;
    Url::EnumStatus url_status = Url::UNINITIALIZED;
    Vector<uint32_t> ips;
    
    if (_is_dnsing_lutable.size() == 0) {
      a++;
      LOG(WARNING, "Dnser[%d] ----------------a: %d", _id, a);
      _url_queue->pop(url_p);
    } else { // need to adns_check(), non-blocking
      b++;
      LOG(WARNING, "Dnser[%d] ----------------b: %d", _id, b);
      if (_url_queue->try_pop(url_p) != S_OK) {
        goto check;
      }
    }

    if (url_p.is_null()) {
      goto check;
    }

    // if the url has not been initialized, drop it
    url_p->get_status(url_status);
    if (url_status != Url::INITIALIZED) {
      goto check;
    }

    url_p->get_site(site);
    LOG(WARNING, "Dnser[%d] dns for %s", _id, site.c_str());

    if (_dns_cache->find(site, ips) == S_OK) {
      url_p->set_ip(ips);
      put_url_into_scheduler(url_p);
      LOG(WARNING, "Dnser[%d] put_url_into_scheduler() OK!", _id);
      goto check;
    }

    if (_is_dnsing_lutable[site].size() == 0) { // need adns_submit
      Vector<SharedPointer<Url> > v;
      v.push_back(url_p);
      _is_dnsing_lutable[site] = v;
      submit_once(site.c_str(), &(_is_dnsing_lutable[site]));
    } else { // do not need adns_submit
      _is_dnsing_lutable[site].push_back(url_p);
    }

check:
    check_once();
  }

  return S_OK;
}

RES_CODE DnserTask::submit_once(const char* site, void* context) {
  adns_query query = NULL;
  adns_status status = (adns_status)adns_submit(_dns_state, site, 
      (adns_rrtype)adns_r_a, adns_qf_none, context, &query);
  if (status) { // submit error
    LOG(WARNING, "Dnser[%d] adns_submit() error, site is %s", _id, site);
  }

  return S_OK;
}

RES_CODE DnserTask::check_once() {
  String site;
  adns_query new_query = NULL;
  adns_answer *answer;
  Vector<SharedPointer<Url> >* urls;
  int res = adns_check(_dns_state, &new_query, &answer, (void**)&urls);
  if (res == EAGAIN || res == ESRCH) {
    //LOG(WARNING, "Dnser[%d] no ready ips", _id);
    return S_OK;
  }

  Vector<SharedPointer<Url> >::iterator itr = urls->begin();
  (*itr)->get_site(site);

  if (answer->status == adns_s_ok) {
    char* strip;
    uint32_t ip;
    Vector<uint32_t> ips;
    const char* rrtype_name;
    const char* formattype_name;
    int len;

    adns_rr_info(answer->type, &rrtype_name, &formattype_name, &len, 0, 0);

    // only for IPv4
    if (len != 4) {
      _is_dnsing_lutable.erase(site);
      free(answer);
      return S_OK;
    }

    int i;
    for (i = 0; i < answer->nrrs; ++i) {
      adns_rr_info(answer->type, 0, 0, 0, answer->rrs.bytes + i * len, &strip);
      inet_pton(AF_INET, strip, &ip);
      ips.push_back(ip);
    }

    uint32_t j;
    char dst[100];
    for (j = 0; j < ips.size(); ++j) {
      inet_ntop(AF_INET, &(ips[j]), dst, sizeof(dst));
      LOG(WARNING, "Dnser[%d] %s :%s", _id, site.c_str(), dst);
    }

    for (; itr != urls->end(); ++itr) {
      (*itr)->set_ip(ips);
      put_url_into_scheduler((*itr));
    }

    // remove from temporary lookup table
    _is_dnsing_lutable.erase(site);
    // update dns cache
    _dns_cache->insert(site, ips);
  } else if (answer->status == adns_s_prohibitedcname) {
    submit_once(answer->cname, &(_is_dnsing_lutable[site]));
  } else {
    LOG(WARNING, "Dnser[%d] not ands_s_ok, error code %d", _id, answer->status);
    // remove from temporary lookup table
    _is_dnsing_lutable.erase(site);
  }

  free(answer);
  return S_OK;
}

RES_CODE DnserTask::put_url_into_scheduler(SharedPointer<Url>& url_p) {
  MD5 md5;
  if (url_p->get_md5(md5) != S_OK) {
    return S_FAIL;
  }

  uint32_t index = 0;
  if (md5.shuffle(_scheduler_num, index) != S_OK) {
    return S_FAIL;
  }

  _scheduler_queues[index]->push(url_p);

  return S_OK;
}

_END_MYJFM_NAMESPACE_

