#ifndef _EXTRACTOR_TASK_H_
#define _EXTRACTOR_TASK_H_

#include "config.h"
#include "task.h"

_START_MYJFM_NAMESPACE_

class ExtractorTask : public Task {
public:
  ExtractorTask(int id);
  ~ExtractorTask();
  virtual RES_CODE operator()(void* arg = NULL);

private:
  int _id;
};

_END_MYJFM_NAMESPACE_

#endif

