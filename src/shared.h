#ifndef _SHARED_H_
#define _SHARED_H_

#include "config.h"
#include "mutex.h"

_START_MYJFM_NAMESPACE_

// the class inherited from this class can obtain the atomic inc and dec feature
class Shared {
public:
  virtual ~Shared() {};
  RES_CODE add_ref();
  unsigned int dec_ref(bool* flag = NULL);

protected:
  Shared();

private:
  unsigned int _count;
  Mutex _mutex;
  // only can be inherited
  Shared(const Shared&);
  Shared& operator=(const Shared&);
};

_END_MYJFM_NAMESPACE_

#endif

