/*******************************************************************************
 * dns_cache.h - cache the mapping from site to ips
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _DNS_H_
#define _DNS_H_

#include <stdint.h>
#include <netinet/in.h>

#include "config.h"
#include "shared.h"

_START_MYJFM_NAMESPACE_

struct dns_request_header {
  uint16_t id;
  uint16_t flag;
  uint16_t query_num;
  uint16_t record_num;
  uint16_t auth_record_num;
  uint16_t extra_record_num;
};

class Dns : public Shared {
#define MAX_BUFFER 513
#define MAX_NAME 128
#define MAX_LABEL 63
#define PORT 53
#define ID 0x4a46
#define QUERY_TYPE 1
#define QUERY_CLASS 1
#define GET_QR(a) ((a & 0x8000) >> 15)
#define IS_REQUEST(a) ((GET_QR(a)) & 0x01)
#define IS_RESPONSE(a) !IS_REQUEST(a)

public:
  Dns();
  ~Dns();

  RES_CODE init(String&);
  RES_CODE query(String&, Vector<String>&);

private:
  RES_CODE create_connection();
  RES_CODE close_connection();
  RES_CODE generate_request(String&, char*, uint32_t&);
  RES_CODE create_question(String&, String&);
  RES_CODE send_request(char*, uint32_t);
  RES_CODE extract_response(Vector<String>&);

  // the udp socket fd
  int _fd;
  struct sockaddr_in _addr_info;
  dns_request_header _request_header;
};

_END_MYJFM_NAMESPACE_

#endif

