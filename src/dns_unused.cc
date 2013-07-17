/*******************************************************************************
 * dns_cache.h - cache the mapping from site to ips
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <arpa/inet.h>
#include <stdint.h>

#include "config.h"
#include "global.h"
#include "dns.h"

extern _MYJFM_NAMESPACE_::Global* glob;

_START_MYJFM_NAMESPACE_

Dns::Dns() : _fd(-1) {}

Dns::~Dns() {
  close_connection();
}

RES_CODE Dns::init(String& nameserver_ip) {
  if (nameserver_ip.length() == 0) {
    _fd = -1;
    return S_FAIL;
  }

  memset(&_request_header, 0, sizeof(_request_header));
  _request_header.id = htons(ID);
  _request_header.flag = htons(0x0780); //0 0000 1 1 1 1 000 0000b
  _request_header.query_num = htons(1);
  _request_header.record_num = 0;
  _request_header.auth_record_num = 0;
  _request_header.extra_record_num = 0;

  bzero((char*)&_addr_info, sizeof(_addr_info));
  _addr_info.sin_family = AF_INET;
  _addr_info.sin_port = htons(PORT);
  _addr_info.sin_addr.s_addr = inet_addr(nameserver_ip.c_str());

  if (create_connection() != S_OK) {
    close_connection();
    return S_FAIL;
  }

  return S_OK;
}

RES_CODE Dns::create_connection() {
  close_connection();

  _fd = socket(PF_INET, SOCK_DGRAM, 0);
  if (_fd < 0) {
    _fd = -1;
    return S_FAIL;
  }

  // set the send and receive timeout
  int timeout = 0;
  if (glob->get_dns_timeout(timeout) != S_OK) {
    return S_FAIL;
  }

  struct timeval tm = {(time_t)timeout, (suseconds_t)0};

  if (setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, &tm, sizeof(tm)) != 0) {
    return S_FAIL;
  }

  if (setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(tm)) != 0) {
    return S_FAIL;
  }

  return S_OK;
}

RES_CODE Dns::close_connection() {
  if (_fd > 0) {
    close(_fd);
  }
  
  _fd = -1;

  return S_OK;
}

RES_CODE Dns::generate_request(String& site, char* buffer, uint32_t& len) {
  if (site.length() == 0 || !buffer) {
    return S_FAIL;
  }

  String question = "";
  if (create_question(site, question) != S_OK) {
    return S_FAIL;
  }

  char* p = buffer;
  memcpy(p, (char*)&_request_header, sizeof(dns_request_header));
  p += sizeof(dns_request_header);
  memcpy(p, question.c_str(), question.length());
  p += question.length() + 2;
  *(p - 1) = 0;
  *(uint16_t*)p = ntohs(QUERY_TYPE);
  p += 2;
  *(uint16_t*)p = ntohs(QUERY_CLASS);
  p += 2;
  len = p - buffer;

  return S_OK;
}

RES_CODE Dns::create_question(String& site, String& question) {
  if (site.length() > MAX_NAME) {
    question = "";
    return S_FAIL;
  }

  question = " ";
  question += site;
  int pre = 0;
  int cur = 1;
  while (cur < question.length()) {
    if (question[cur] != '.') {
      cur++;
    } else {
      if (cur - pre - 1 > MAX_LABEL || cur - pre - 1 <= 0) {
        question = "";
        return S_FAIL;
      }

      question[pre] = cur - pre - 1;
      pre = cur;
      cur++;
    }
  }

  if (cur - pre - 1 > MAX_LABEL || cur - pre - 1 <= 0) {
    question = "";
    return S_FAIL;
  }

  question[pre] = cur - pre - 1;

  return S_OK;
}

RES_CODE Dns::send_request(char* buffer, uint32_t len) {
  if (_fd < 0 || !buffer) {
    return S_FAIL;
  }

  if (len == 0) {
    return S_OK;
  }

  char* p = buffer;

  while (1) {
    int ret = sendto(_fd, p, len, 0, 
        (struct sockaddr*)&_addr_info, sizeof(struct sockaddr));
    if (ret <= 0) {
      return S_FAIL;
    } else if (ret == len) {
      return S_OK;
    } else {
      len -= ret;
      p += ret;
    }
  }

  return S_OK;
}

RES_CODE Dns::extract_response(Vector<String>& ips) {
  if (_fd < 0) {
    return S_FAIL;
  }

  int ret;
  char buffer[MAX_BUFFER];
  char* p = buffer;
  int len = 12; // dns packet header's length

  while (1) {
    ret = recvfrom(_fd, p, len, 0, NULL, NULL);
    if (ret <= 0) {
      return S_FAIL;
    } else if (ret == len) {
      break;
    } else {
      len -= ret;
      p += ret;
    }
  }

  uint16_t id = ntohs((uint16_t)(*p));
  if (id != ID) {
    return S_FAIL;
  }

  p += 2;
  uint16_t flag = ntohs((uint16_t)(*p));
  if (!IS_RESPONSE(flag)) {
    return S_FAIL;
  }

  p += 2;

  return S_OK;
}

RES_CODE Dns::query(String& site, Vector<String>& ips) {
  if (site.length() <= 0) {
    return S_FAIL;
  }

  if (_fd < 0) {
    if (create_connection() != S_OK) {
      close_connection();
      return S_FAIL;
    }
  }

  uint32_t len = 0;
  char buffer[MAX_BUFFER];

  if (generate_request(site, buffer, len) != S_OK) {
    return S_FAIL;
  }

  if (send_request(buffer, len) != S_OK) {
    close_connection();
    return S_FAIL;
  }

  if (extract_response(ips) != S_OK) {
    close_connection();
    return S_FAIL;
  }

  return S_OK;
}

_END_MYJFM_NAMESPACE_

