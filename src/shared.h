#ifndef _SHARED_H_
#define _SHARED_H_

#include "mutex.h"

// the class inherited from this class can obtain the atomic inc and dec feature
class Shared {
public:
  virtual ~Shared() {};
  void add_ref();
  unsigned int dec_ref();
private:
  unsigned int _count;
  Mutex _mutex;
  // only can be inherited
  Shared();
  Shared& operator=(const Shared&);
};

#endif

