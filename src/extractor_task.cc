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
#include <string.h>
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
#include "zlib.h"

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
    SharedPointer<Page> page_compressed_p;
    _page_queue->pop(page_compressed_p);

    if (page_compressed_p.is_null()) {
      continue;
    }

    SharedPointer<Page> page_decompressed_p(NULL);
    
    if (decompress_page(page_compressed_p, page_decompressed_p) != S_OK || 
        page_decompressed_p.is_null()) {
      continue;
    }

    String site;
    uint16_t port;
    String file;
    String charset;
    HttpResponseHeader::http_encoding_type encoding_type = 
      HttpResponseHeader::HTTP_ENCODING_UNKNOWN;
    page_compressed_p->get_site(site);
    page_compressed_p->get_port(port);
    page_compressed_p->get_file(file);
    page_compressed_p->get_charset(charset);
    page_compressed_p->get_encoding(encoding_type);

    page_decompressed_p->set_site(site);
    page_decompressed_p->set_port(port);
    page_decompressed_p->set_file(file);
    page_decompressed_p->set_charset(charset);
    page_decompressed_p->set_encoding(encoding_type);

    LOG(INFO, "[%d] site: %s port: %d file : %s charset %s encoding %d", 
        _id, site.c_str(), port, file.c_str(), charset.c_str(), encoding_type);

    save_page(page_decompressed_p);

    Vector<String> links;
    if (extract_links(page_decompressed_p, links) != S_OK) {
      continue;
    }
  }

  return S_OK;
}

RES_CODE ExtractorTask::decompress_page(SharedPointer<Page>& page_compressed_p, 
    SharedPointer<Page>& page_decompressed_p) {
  if (page_compressed_p.is_null()) {
    return S_FAIL;
  }

  z_stream stream;
  memset(&stream, 0, sizeof(stream));

  HttpResponseHeader::http_encoding_type src_page_encoding = 
    HttpResponseHeader::HTTP_ENCODING_UNKNOWN;
  uint32_t src_page_size = 0;
  char* src_page_content = NULL;
  page_compressed_p->get_page_size(src_page_size);
  page_compressed_p->get_page_content(src_page_content);
  page_compressed_p->get_encoding(src_page_encoding);

  switch (src_page_encoding) {
    case HttpResponseHeader::HTTP_ENCODING_TXT:
      page_decompressed_p = page_compressed_p;
      LOG(INFO, "TXT ok!");
      return S_OK;
    case HttpResponseHeader::HTTP_ENCODING_GZIP: 
      if (inflateInit2(&stream, 47) != Z_OK) {
        page_decompressed_p = SharedPointer<Page>(NULL);
        return S_FAIL;
      }

      LOG(INFO, "GZIP ok!");
      break;
    case HttpResponseHeader::HTTP_ENCODING_DEFLATE: 
      if (inflateInit(&stream) != Z_OK) {
        page_decompressed_p = SharedPointer<Page>(NULL);
        return S_FAIL;
      }

      LOG(INFO, "DEFLATE ok!");
      break;
    default:
      page_decompressed_p = SharedPointer<Page>(NULL);
      return S_FAIL;
  }

  stream.next_in = (Bytef*)src_page_content;
  stream.avail_in = (uInt)src_page_size;

  page_decompressed_p = SharedPointer<Page>(new Page(0));

  uint32_t cache_len = src_page_size << 1;
  MemoryPool* memory_pool = MemoryPool::get_instance();
  char* cache = (char*)memory_pool->get_memory(cache_len);

  bool dummy_initialized = false;

  enum state {
    XXXX_DUMMY = 0, 
    XXXX_FAIL = -1, 
    XXXX_OK = 1, 
    XXXX_GOON = 2
  };

  state is_succ = XXXX_DUMMY;
  uint32_t bytes_written = 0;
  int code = 0;

  while (1) {
    stream.next_out = (Bytef*)cache;
    stream.avail_out = (uInt)(cache_len);

    code = inflate(&stream, Z_NO_FLUSH);
    bytes_written = (uInt)(cache_len) - stream.avail_out;

    switch (code) {
      case Z_STREAM_END:
        is_succ = XXXX_OK;
        break;

      case Z_OK:
      case Z_BUF_ERROR:
        is_succ = XXXX_GOON;
        break;

      case Z_DATA_ERROR:
        // some servers don't generate zlib headers
        // insert a dummy header and try again
        static char dummy_head[2] = { 0x8 + 0x7 * 0x10, 
          (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF };
        
        inflateReset(&stream);
        stream.next_in = (Bytef*) dummy_head;
        stream.avail_in = sizeof(dummy_head);
        
        if (inflate(&stream, Z_NO_FLUSH) != Z_OK) {
          is_succ = XXXX_FAIL;
          break;
        }
        
        // stop an endless loop caused by 
        // non-deflate data being labelled as deflate
        if (dummy_initialized) {
          is_succ = XXXX_FAIL;
          break;
        }
        
        dummy_initialized = true;
        stream.next_in = (Bytef*)src_page_content;
        stream.avail_in = (uInt)src_page_size;
        is_succ = XXXX_GOON;
        break;

      default:
        is_succ = XXXX_FAIL;
        break;
    }

    switch (is_succ) {
      case XXXX_GOON:
      case XXXX_OK:
        if (bytes_written > 0) {
          uint32_t have_got_len = 0;
          char* have_got_content = NULL;
          page_decompressed_p->get_page_size(have_got_len);
          page_decompressed_p->get_page_content(have_got_content);
          SharedPointer<Page> tmp_p(new Page(have_got_len + bytes_written));
          if (tmp_p.is_null()) {
            memory_pool->put_memory(cache);
            page_decompressed_p = SharedPointer<Page>(NULL);
            inflateEnd(&stream);
            return S_FAIL;
          }

          char* tmp_content = NULL;
          tmp_p->get_page_content(tmp_content);
          if (have_got_content && have_got_len > 0) {
            memcpy(tmp_content, have_got_content, have_got_len);
          }
          
          memcpy(tmp_content + have_got_len, cache, bytes_written);
          page_decompressed_p = tmp_p;
        }

        if (is_succ == XXXX_GOON) {
          break;
        } else {
          memory_pool->put_memory(cache);
          inflateEnd(&stream);
          return S_OK;
        }

      case XXXX_FAIL:
      default:
        memory_pool->put_memory(cache);
        page_decompressed_p = SharedPointer<Page>(NULL);
        inflateEnd(&stream);
        LOG(INFO, "XXXX_FAIL");
        return S_FAIL;
    }
  }
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

