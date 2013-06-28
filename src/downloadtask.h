#ifndef _DOWNLOADTASK_H_
#define _DOWNLOADTASK_H_

#include "config.h"
#include "task.h"
#include "sharedpointer.h"

_START_MYJFM_NAMESPACE_

class Downloadtask : public Task {
public:
  Downloadtask(int id);
  ~Downloadtask();
  virtual RES_CODE operator()(void* arg = NULL);

private:
  int _id;
  Sharedpointer<Squeue<Sharedpointer<Url> > > _url_queue;
};

_END_MYJFM_NAMESPACE_

#endif

