#include <assert.h>

#include "config.h"
//#include "site.h"
#include "md5.h"
#include "url.h"

void md5_test() {
  String seedurl = "http://www.baidu.com:8080/hehe?haah&f=aa3";
  _MYJFM_NAMESPACE_::Url url(seedurl);
  _MYJFM_NAMESPACE_::MD5 md5_1, md5_2;
  url.get_md5(md5_1);
  _MYJFM_NAMESPACE_::MD5Caculator md5caculator("http://www.baidu.com:8080/hehe?haah&f=aa3");
  md5caculator.digest(md5_2);
  assert(md5_1 == md5_2);
}

void url_test() {
  String seedurl = "http://www.baidu.com/s?wd=hehe&rsv_spt=1&issp=1&rsv_bp=0&ie=utf-8&tn=baiduhome_pg&rsv_sug3=1";

  _MYJFM_NAMESPACE_::Url url(seedurl);

  bool valid = 0;
  url.is_valid(valid);
  assert(valid);

  Protocol proto;
  String site, port, file;
  url.get_protocol(proto);
  url.get_site(site);
  url.get_port(port);
  url.get_file(file);
  assert(proto == HTTP);
  assert(site == "www.baidu.com");
  assert(port == "80");
  assert(file == "s?wd=hehe&rsv_spt=1&issp=1&rsv_bp=0&ie=utf-8&tn=baiduhome_pg&rsv_sug3=1");
}

int main() {
  url_test();
  md5_test();
  return 0;
}

