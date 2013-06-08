#ifndef _SMART_H_
#define _SMART_H_

#include "mutex.h"

// the class inherited from this class can obtain the atomic inc and dec feature
class Smart {
public:
  virtual ~Smart() {};
  void add_ref();
  unsigned int dec_ref();
private:
  unsigned int _count;
  Mutex _mutex;
  // only can be inherited
  Smart();
  Smart& operator=(const Smart&);
};

#endif

