#include "config.h"
#include "global.h"
#include "hash.h"
#include <assert.h>

typedef _MYJFM_NAMESPACE_::Stringhashfunction Stringhashfunction;
typedef _MYJFM_NAMESPACE_::Hash<String, Stringhashfunction> Stringhash;

int main() {
  Stringhash htable(10);
  assert(htable.is_exist(CHARS2STR("hashtable")) == 0);
  assert(htable.insert(CHARS2STR("hashtable")) == 0);
  assert(htable.is_exist(CHARS2STR("hashtable")) == 1);
  return 0;
}

