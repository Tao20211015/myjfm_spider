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
#include <sys/stat.h>

#include "config.h"
#include "global.h"
#include "shared_pointer.h"
#include "extractor_task.h"
#include "squeue.h"
#include "url.h"
#include "log.h"
#include "page.h"

extern _MYJFM_NAMESPACE_::Global* glob;

_START_MYJFM_NAMESPACE_

ExtractorTask::ExtractorTask(uint32_t id) : 
  _id(id), 
  _page_queue(NULL) {
}

ExtractorTask::~ExtractorTask() {}

RES_CODE ExtractorTask::operator()(void* arg) {
  if (init() != S_OK) {
    return S_FAIL;
  }

  /*
  for (;;) {
    LOG(INFO, "#Extractor# %d", _id);
    usleep(1000);
  }
  */

  return main_loop();
}

RES_CODE ExtractorTask::init() {
  if (glob->get_dnser_num(_dnser_num) != S_OK) {
    return S_FAIL;
  }

  if (glob->get_extractor_queue(_id, _page_queue) != S_OK) {
    return S_FAIL;
  }

  if (glob->get_save_path(_save_path) != S_OK) {
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

RES_CODE ExtractorTask::main_loop() {
  for (;;) {
    // get the url from _url_queue
    SharedPointer<Page> page_p;
    _page_queue->pop(page_p);

    if (page_p.is_null()) {
      continue;
    }

    String site;
    uint16_t port;
    String file;
    String charset;
    int32_t encoding;
    page_p->get_site(site);
    page_p->get_port(port);
    page_p->get_file(file);
    page_p->get_charset(charset);
    page_p->get_encoding(encoding);

    LOG(INFO, "[%d] site: %s port: %d file : %s charset %s encoding %d", 
        _id, site.c_str(), port, file.c_str(), charset.c_str(), encoding);

    save_page(page_p);

    Vector<String> links;
    if (extract_links(page_p, links) != S_OK) {
      continue;
    }
  }

  return S_OK;
}

RES_CODE ExtractorTask::save_page(SharedPointer<Page>& page_p) {
  String site;
  uint16_t port;
  String file;
  page_p->get_site(site);
  page_p->get_port(port);
  page_p->get_file(file);

  if (file.length() == 0) {
    file = "index.html";
  }

  Vector<String> dirs;
  Utility::split(file, CHARS2STR("/"), dirs);

  uint32_t i;
  String save_path = _save_path;
  for (i = 0; i < dirs.size(); ++i) {
    if (i == 0) {
      save_path = save_path + "/" + site;
    } else {
      save_path = save_path + "/" + dirs[i - 1];
    }

    if (access(save_path.c_str(), F_OK) != 0) {
      if (mkdir(save_path.c_str(), 0755) != 0) {
        LOG(ERROR, "[%d] can't create path: %s", _id, save_path.c_str());
        return S_FAIL;
      }
    }
  }

  save_path = save_path + "/" + dirs[dirs.size() - 1];

  Ofstream fs;
  fs.open(save_path.c_str(), 
      Ofstream::out | Ofstream::trunc);
  /*
  fs.open(save_path.c_str(), 
      Ofstream::out | Ofstream::trunc | Ofstream::binary);
      */
  if (!fs) {
    LOG(ERROR, "[%d] can't create file: %s", _id, save_path.c_str());
    return S_FAIL;
  }

  uint32_t page_size = 0;
  char* content = NULL;
  page_p->get_page_size(page_size);
  page_p->get_page_content(content);
  LOG(WARNING, "[%d] %s***page size: %u", _id, save_path.c_str(), page_size);

  if (content != NULL) {
    fs.write(content, page_size);
    fs.close();
    return S_OK;
  } else {
    fs.close();
    return S_FAIL;
  }
}

RES_CODE ExtractorTask::extract_links(SharedPointer<Page>& page_p, 
    Vector<String>& links) {
  return S_OK;
}

_END_MYJFM_NAMESPACE_

