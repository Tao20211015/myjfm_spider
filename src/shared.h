#ifndef _SHARED_H_
#define _SHARED_H_

#include "config.h"
#include "mutex.h"

_START_MYJFM_NAMESPACE_

// the class inherited from this class can obtain the atomic inc and dec feature
class Shared {
public:
  virtual ~Shared() {};
  void add_ref();
  unsigned int dec_ref(bool* flag = NULL);
private:
  unsigned int _count;
  Mutex _mutex;
  // only can be inherited
  Shared();
  Shared& operator=(const Shared&);
};

_END_MYJFM_NAMESPACE_

#endif

