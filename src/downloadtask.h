#ifndef _DOWNLOADTASK_H_
#define _DOWNLOADTASK_H_

#include "config.h"
#include "task.h"

_START_MYJFM_NAMESPACE_

class Downloadtask : public Task {
public:
  Downloadtask(int id);
  ~Downloadtask();
  virtual int operator()(void* arg = NULL);

private:
  int _id;
};

_END_MYJFM_NAMESPACE_

#endif

