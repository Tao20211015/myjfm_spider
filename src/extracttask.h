#ifndef _EXTRACTTASK_H_
#define _EXTRACTTASK_H_

#include "config.h"
#include "task.h"

_START_MYJFM_NAMESPACE_

class Extracttask : public Task {
public:
  Extracttask(int id);
  ~Extracttask();
  virtual int operator()(void* arg = NULL);
private:
  int _id;
};

_END_MYJFM_NAMESPACE_

#endif

