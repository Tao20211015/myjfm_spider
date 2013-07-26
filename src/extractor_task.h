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
#include "shared_pointer.h"
#include "squeue.h"
#include "url.h"
#include "page.h"

_START_MYJFM_NAMESPACE_

class ExtractorTask : public Task {
public:
  ExtractorTask(uint32_t id);
  ~ExtractorTask();
  virtual RES_CODE operator()(void* arg = NULL);

private:
  uint32_t _id;
  uint32_t _dnser_num;
  String _save_path;
  SharedPointer<SQueue<SharedPointer<Page> > > _page_queue;
  Vector<SharedPointer<SQueue<SharedPointer<Url> > > > _dnser_queues;

  RES_CODE init();
  RES_CODE main_loop();
  RES_CODE save_page(SharedPointer<Page>&);
  RES_CODE extract_links(SharedPointer<Page>&, Vector<String>&);
};

_END_MYJFM_NAMESPACE_

#endif

