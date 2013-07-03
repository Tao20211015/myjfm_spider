#ifndef _DOWNLOADER_TASK_H_
#define _DOWNLOADER_TASK_H_

#include "config.h"
#include "task.h"
#include "shared_pointer.h"

_START_MYJFM_NAMESPACE_

class DownloaderTask : public Task {
public:
  DownloaderTask(int id);
  ~DownloaderTask();
  virtual RES_CODE operator()(void* arg = NULL);

private:
  int _id;
  SharedPointer<SQueue<SharedPointer<Url> > > _url_queue;
};

_END_MYJFM_NAMESPACE_

#endif

