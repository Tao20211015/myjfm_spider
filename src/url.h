#ifndef _URL_H_
#define _URL_H_

#include "config.h"
#include "shared.h"
#include "site.h"
#include "md5.h"
#include "sharedpointer.h"

_START_MYJFM_NAMESPACE_

class Url : public Shared {
public:
  Url(const String& url = "");
  ~Url() {}

  RES_CODE get_protocol(Protocol&);
  //void set_protocol(Protocol);

  RES_CODE get_site(Sharedpointer<Site>&);
  //void set_site(Site&);

  RES_CODE get_file(String&);
  //void set_file(String&);

  RES_CODE get_args(String&);
  //void set_args(String&);

  RES_CODE get_md5(MD5&);

private:
  Protocol _protocol;
  Sharedpointer<Site> _site;
  String _file;
  String _args;

  int has_get_md5;
  MD5 _md5;
};

_END_MYJFM_NAMESPACE_

#endif

