#include <assert.h>

#include "config.h"
#include "hash.h"
#include "sharedpointer.h"

typedef _MYJFM_NAMESPACE_::Stringhashfunction Stringhashfunction;
typedef _MYJFM_NAMESPACE_::Hash<String, Stringhashfunction> Stringhash;
typedef _MYJFM_NAMESPACE_::Sharedpointer<Stringhash> Sharedpointerhash;

int main() {
  Sharedpointerhash htablep(new Stringhash(10));
  //Stringhash htable(10);
  String str("hashtable");
  assert(htablep->is_exist(str) == S_NOT_EXIST);
  assert(htablep->insert(str) == S_OK);
  assert(htablep->is_exist(str) == S_EXIST);
  return 0;
}

