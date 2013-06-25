#include "config.h"
#include "global.h"
#include "hash.h"
#include "sharedpointer.h"
#include <assert.h>

typedef _MYJFM_NAMESPACE_::Stringhashfunction Stringhashfunction;
typedef _MYJFM_NAMESPACE_::Hash<String, Stringhashfunction> Stringhash;
typedef _MYJFM_NAMESPACE_::Sharedpointer<Stringhash> Sharedpointerhash;

int main() {
  Sharedpointerhash htablep(new Stringhash(10));
  //Stringhash htable(10);
  String str("hashtable");
  assert(htablep->is_exist(str) == 0);
  assert(htablep->insert(str) == 0);
  assert(htablep->is_exist(str) == 1);
  return 0;
}

