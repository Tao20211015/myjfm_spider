#ifndef _URL_H_
#define _URL_H_

#include "config.h"
#include "shared.h"
#include "site.h"
#include "md5.h"

_START_MYJFM_NAMESPACE_

enum Protocol {
  DUMMY = 0x0, 
  HTTP = 0x1,
  FTP = 0x2
};

class Url : public Shared {
public:
  Url(const String& url = "");
  ~Url() {}

  Protocol get_protocol();
  //void set_protocol(Protocol);

  void get_site(Site&);
  //void set_site(Site&);

  void get_file(String&);
  //void set_file(String&);

  void get_args(String&);
  //void set_args(String&);

  int get_md5(MD5&);

private:
  Protocol _protocol;
  Site _site;
  String _file;
  String _args;

  int has_get_md5;
  MD5 _md5;
};

_END_MYJFM_NAMESPACE_

#endif

