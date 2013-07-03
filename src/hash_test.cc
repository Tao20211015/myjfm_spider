#include <assert.h>

#include "config.h"
#include "hash.h"
#include "shared_pointer.h"

typedef _MYJFM_NAMESPACE_::StringHashFunction StringHashFunction;
typedef _MYJFM_NAMESPACE_::Hash<String, StringHashFunction> StringHash;
typedef _MYJFM_NAMESPACE_::SharedPointer<StringHash> SharedPointerHash;

int main() {
  SharedPointerHash htablep(new StringHash(10));
  //StringHash htable(10);
  String str("hashtable");
  assert(htablep->is_exist(str) == S_NOT_EXIST);
  assert(htablep->insert(str) == S_OK);
  assert(htablep->is_exist(str) == S_EXIST);
  return 0;
}

