/*******************************************************************************
 * url.h - the url class
 * it includes the protocol(HTTP), the site's name, the port, the requested file
 * name, etc.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _URL_H_
#define _URL_H_

#include <stdint.h>

#include "config.h"
#include "shared.h"
#include "md5.h"
#include "shared_pointer.h"

_START_MYJFM_NAMESPACE_

class Url : public Shared {
public:
  Url(const String& url = "");
  ~Url() {}

  RES_CODE is_valid(bool&);

  RES_CODE get_protocol(Protocol&);

  RES_CODE get_ip(uint32_t&);

  RES_CODE get_site(String&);

  RES_CODE get_port(String&);

  RES_CODE get_file(String&);

  RES_CODE get_md5(MD5&);

  RES_CODE get_retries(uint32_t&);

  RES_CODE inc_retries();

private:
  String _raw_url;
  volatile bool _is_valid;
  Protocol _protocol;
  uint32_t _ip;
  String _site;
  String _port;
  String _file;

  bool has_get_md5;
  MD5 _md5;

  uint32_t _retries;
};

_END_MYJFM_NAMESPACE_

#endif

