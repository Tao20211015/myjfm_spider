/*******************************************************************************
 * url.h - the url class
 * it includes the protocol(HTTP), the site's name, the port, the requested file
 * name, the arguments, etc.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _URL_H_
#define _URL_H_

#include "config.h"
#include "shared.h"
//#include "site.h"
#include "md5.h"
#include "shared_pointer.h"

_START_MYJFM_NAMESPACE_

class Url : public Shared {
public:
  Url(const String& url = "");
  ~Url() {}

  RES_CODE is_valid(int&);

  RES_CODE get_protocol(Protocol&);
  //void set_protocol(Protocol);

  RES_CODE get_site(String&);
  //RES_CODE get_site(SharedPointer<Site>&);
  //void set_site(Site&);

  RES_CODE get_port(String&);
  //void set_port(String&);

  RES_CODE get_file(String&);
  //void set_file(String&);

  RES_CODE get_args(String&);
  //void set_args(String&);

  RES_CODE get_md5(MD5&);

  RES_CODE get_retries(int&);

  RES_CODE inc_retries();

private:
  String _raw_url;
  volatile int _is_valid;
  Protocol _protocol;
  String _site;
  String _port;
  //SharedPointer<Site> _site;
  String _file;
  String _args;

  int has_get_md5;
  MD5 _md5;

  int _retries;
};

_END_MYJFM_NAMESPACE_

#endif

