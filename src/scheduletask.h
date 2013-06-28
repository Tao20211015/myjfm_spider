#ifndef _SCHEDULETASK_H_
#define _SCHEDULETASK_H_

#include "config.h"
#include "task.h"

_START_MYJFM_NAMESPACE_

class Scheduletask : public Task {
public:
  Scheduletask(int);
  ~Scheduletask();
  virtual RES_CODE operator()(void* arg = NULL);

private:
  int _id;
};

_END_MYJFM_NAMESPACE_

#endif

