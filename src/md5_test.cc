#include <assert.h>

#include "config.h"
#include "md5.h"

int main() {
  String str;

  _MYJFM_NAMESPACE_::MD5Caculator md5_1(String(""));
  md5_1.to_string(str);
  assert(str == "d41d8cd98f00b204e9800998ecf8427e");

  _MYJFM_NAMESPACE_::MD5Caculator md5_2(String("a"));
  md5_2.to_string(str);
  assert(str == "0cc175b9c0f1b6a831c399e269772661");

  _MYJFM_NAMESPACE_::MD5Caculator md5_3(String("message digest"));
  md5_3.to_string(str);
  assert(str == "f96b697d7cb7938d525a2f31aaf161d0");

  return 0;
}

