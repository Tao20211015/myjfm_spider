/*******************************************************************************
 * dns_cache.h - cache the mapping from site to ips
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _DNS_H_
#define _DNS_H_

#include <netinet/in.h>

#include "config.h"
#include "shared.h"

_START_MYJFM_NAMESPACE_

struct dns_request_header {
  unsigned short id;
  unsigned short flag;
  unsigned short query_num;
  unsigned short record_num;
  unsigned short auth_record_num;
  unsigned short extra_record_num;
};

class Dns : public Shared {
#define MAX_REQUEST 512
#define MAX_NAME 128
#define MAX_LABEL 63
#define PORT 53
#define ID 0x4a46
#define QUERY_TYPE 1
#define QUERY_CLASS 1

public:
  Dns();
  ~Dns();

  RES_CODE init(String&);
  RES_CODE query(String&, Vector<String>&);

private:
  RES_CODE create_connection();
  RES_CODE close_connection();
  RES_CODE generate_request(String&, char*, int&);
  RES_CODE create_question(String&, String&);
  RES_CODE send_request(char*, int);

  // the udp socket fd
  int _fd;
  struct sockaddr_in _addr_info;
  dns_request_header _request_header;
};

_END_MYJFM_NAMESPACE_

#endif

