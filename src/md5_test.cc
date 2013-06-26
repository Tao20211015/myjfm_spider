#include "config.h"
#include "md5.h"
#include <assert.h>

int main() {
  _MYJFM_NAMESPACE_::MD5caculator md5_1(String(""));
  assert(md5_1.to_string() == "d41d8cd98f00b204e9800998ecf8427e");

  _MYJFM_NAMESPACE_::MD5caculator md5_2(String("a"));
  assert(md5_2.to_string() == "0cc175b9c0f1b6a831c399e269772661");

  _MYJFM_NAMESPACE_::MD5caculator md5_3(String("message digest"));
  assert(md5_3.to_string() == "f96b697d7cb7938d525a2f31aaf161d0");

  return 0;
}

