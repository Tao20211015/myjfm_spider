#include "config.h"
#include "global.h"
#include "site.h"
#include "md5.h"
#include "url.h"
#include <assert.h>

int main() {
  String seedurl = "http://www.baidu.com";
  _MYJFM_NAMESPACE_::Url url(seedurl);
  _MYJFM_NAMESPACE_::MD5 md5_1, md5_2;
  url.get_md5(md5_1);
  _MYJFM_NAMESPACE_::MD5caculator md5caculator("http://www.baidu.com");
  md5caculator.digest(md5_2);
  assert(md5_1 == md5_2);
  return 0;
}

