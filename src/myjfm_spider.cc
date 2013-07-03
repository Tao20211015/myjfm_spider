#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "global.h"
#include "utility.h"
#include "downloader_task.h"
#include "extractor_task.h"
#include "scheduler_task.h"
#include "thread_pool.h"
#include "shared_pointer.h"

// the global variable
// contains many information of the server
// e.x. the configure options
// before access this object, you must invoke the
// init(config_file) member function to init the object
_MYJFM_NAMESPACE_::Global *glob = NULL;

typedef _MYJFM_NAMESPACE_::ThreadPool ThreadPool;
typedef _MYJFM_NAMESPACE_::SharedPointer<ThreadPool> ThreadPoolPtr;
typedef _MYJFM_NAMESPACE_::DownloaderTask DownloaderTask;
typedef _MYJFM_NAMESPACE_::SharedPointer<DownloaderTask> DownloaderTaskPtr;
typedef _MYJFM_NAMESPACE_::ExtractorTask ExtractorTask;
typedef _MYJFM_NAMESPACE_::SharedPointer<ExtractorTask> ExtractorTaskPtr;
typedef _MYJFM_NAMESPACE_::SchedulerTask SchedulerTask;
typedef _MYJFM_NAMESPACE_::SharedPointer<SchedulerTask> SchedulerTaskPtr;

RES_CODE usage(char *argv0) {
  Cerr << "[Usage] " << argv0 << " [-f configure_file_name]" << Endl;
  return S_OK;
}

static RES_CODE load_config(String cur_path, String config_file_name) {
  int res = access(config_file_name.c_str(), F_OK);

  if (res != 0) {
    Cerr << "[FATAL] The configure file '" << config_file_name 
      << "'is not exist." << Endl 
      << "[FATAL] You must specify the configure file using -f option or " << 
      "put the configure file into current directory." << Endl;
    exit(1);
  }

  // init the object
  return glob->init(cur_path, config_file_name);
}

RES_CODE parse_args(int argc, char *argv[]) {
#define MAX_BUF_LEN 1024
  char buffer[MAX_BUF_LEN];
  getcwd(buffer, MAX_BUF_LEN);
#undef MAX_BUF_LEN
  String cur_path(buffer);

  if (argc == 1) {
    String full_config_file_name = cur_path + "/myjfmspider.conf";
    load_config(cur_path, full_config_file_name);
  } else if (argc == 3 && !strcmp(argv[1], "-f")) {
    load_config(cur_path, 
        _MYJFM_NAMESPACE_::Utility::get_file_full_path(CHARS2STR(argv[2])));
  } else if (argc == 2 && 
      (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) {
    usage(argv[0]);
    exit(0);
  } else {
    usage(argv[0]);
    exit(1);
  }

  return S_OK;
}

int main(int argc, char *argv[]) {
  int i = 0;
  glob = new _MYJFM_NAMESPACE_::Global();

  parse_args(argc, argv);

  int downloader_num = 0;
  glob->get_downloader_num(downloader_num);

  int extractor_num = 0;
  glob->get_extractor_num(extractor_num);

  int scheduler_num = 0;
  glob->get_scheduler_num(scheduler_num);

  ThreadPoolPtr downloader_threadpool(new ThreadPool(downloader_num));
  downloader_threadpool->init();

  ThreadPoolPtr extractor_threadpool(new ThreadPool(extractor_num));
  extractor_threadpool->init();

  ThreadPoolPtr scheduler_threadpool(new ThreadPool(scheduler_num));
  scheduler_threadpool->init();

  for (i = 0; i < downloader_num; ++i) {
    DownloaderTaskPtr task(new DownloaderTask(i));
    downloader_threadpool->add_task(task);
  }

  for (i = 0; i < extractor_num; ++i) {
    ExtractorTaskPtr task(new ExtractorTask(i));
    extractor_threadpool->add_task(task);
  }

  for (i = 0; i < scheduler_num; ++i) {
    SchedulerTaskPtr task(new SchedulerTask(i));
    scheduler_threadpool->add_task(task);
  }

  sleep(1000);

  delete glob;

  return 0;
}

