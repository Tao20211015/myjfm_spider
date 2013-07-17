/*******************************************************************************
 * myjfm_spider.cc - the entry
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "config.h"
#include "log.h"
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
  char buffer[1024];
  getcwd(buffer, 1024);
  String cur_path(buffer);

  if (argc == 1) {
    String full_config_file_name = cur_path + "/myjfm_spider.conf";
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

void sig_handler(int sig) {
  LOG(INFO, "SIGTERM received, scheduling shutting down...");
  glob->set_to_be_shutdown(true);
}

void set_sigaction() {
  signal(SIGHUP, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);

  struct sigaction act;
  sigemptyset(&(act.sa_mask));
  act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
  act.sa_handler = sig_handler;

  sigaction (SIGTERM, &act, NULL);
  sigaction (SIGINT, &act, NULL);
}

void init_modules() {
  uint32_t i = 0;
  uint32_t downloader_num = 0;
  uint32_t extractor_num = 0;
  uint32_t scheduler_num = 0;

  glob->get_downloader_num(downloader_num);
  glob->get_extractor_num(extractor_num);
  glob->get_scheduler_num(scheduler_num);

  ThreadPoolPtr downloader_threadpool(new ThreadPool(downloader_num));
  ThreadPoolPtr extractor_threadpool(new ThreadPool(extractor_num));
  ThreadPoolPtr scheduler_threadpool(new ThreadPool(scheduler_num));

  String flag = "";

  if (glob->set_downloader_threadpool(downloader_threadpool) != S_OK) {
    flag = "downloader";
    goto failed;
  }

  if (glob->set_extractor_threadpool(extractor_threadpool) != S_OK) {
    flag = "extractor";
    goto failed;
  }

  if (glob->set_scheduler_threadpool(scheduler_threadpool) != S_OK) {
    flag = "scheduler";
    goto failed;
  }

  RES_CODE t;
  if ((t = downloader_threadpool->init()) != S_OK) {
    flag = "downloader";
    goto failed;
  }

  if (extractor_threadpool->init() != S_OK) {
    flag = "extractor";
    goto failed;
  }

  if (scheduler_threadpool->init() != S_OK) {
    flag = "scheduler";
    goto failed;
  }

  for (i = 0; i < downloader_num; ++i) {
    DownloaderTaskPtr task(new DownloaderTask(i));

    if (downloader_threadpool->add_task(task) != S_OK) {
      flag = "downloader";
      goto failed;
    }
  }

  for (i = 0; i < extractor_num; ++i) {
    ExtractorTaskPtr task(new ExtractorTask(i));

    if (extractor_threadpool->add_task(task) != S_OK) {
      flag = "extractor";
      goto failed;
    }
  }

  for (i = 0; i < scheduler_num; ++i) {
    SchedulerTaskPtr task(new SchedulerTask(i));

    if (scheduler_threadpool->add_task(task) != S_OK) {
      flag = "scheduler";
      goto failed;
    }
  }

  return;

failed:
  LOG(FATAL, 
      "Failed to start %s threadpool. Server is shutting down...", 
      flag.c_str());
  glob->set_to_be_shutdown(true);
}

// create the myjfm_spider.pid file in the current directory
void create_pid_file() {
  FILE* fp = NULL;
  String cur_path = "";
  glob->get_cur_path(cur_path);

  if (cur_path == "") {
    cur_path = "myjfm_spider.pid";
  } else {
    cur_path += "/myjfm_spider.pid";
  }
  
  fp = fopen(cur_path.c_str(), "w");

  if (fp) {
    fprintf(fp, "%d\n", (int)getpid());
    fclose(fp);
  } else {
    LOG(WARNING, 
        "cannot create the myjfm_spider.pid file in current directory");
  }
}

// delete the myjfm_spider.pid file
void delete_pid_file() {
  String cur_path = "";
  glob->get_cur_path(cur_path);

  if (cur_path == "") {
    cur_path = "myjfm_spider.pid";
  } else {
    cur_path += "/myjfm_spider.pid";
  }
  
  unlink(cur_path.c_str());
}

void init() {
  init_modules();

  create_pid_file();

  set_sigaction();
}

void shutdown() {
  LOG(INFO, "The server is shutting down...");

  ThreadPoolPtr downloader_threadpool;
  glob->get_downloader_threadpool(downloader_threadpool);

  if (!downloader_threadpool.is_null()) {
    downloader_threadpool->stop();
  }

  ThreadPoolPtr extractor_threadpool;
  glob->get_extractor_threadpool(extractor_threadpool);

  if (!extractor_threadpool.is_null()) {
    extractor_threadpool->stop();
  }

  ThreadPoolPtr scheduler_threadpool;
  glob->get_scheduler_threadpool(scheduler_threadpool);

  if (!scheduler_threadpool.is_null()) {
    scheduler_threadpool->stop();
  }

  delete_pid_file();

  // must destruct the glob last
  delete glob;
}

int main(int argc, char *argv[]) {
  glob = new _MYJFM_NAMESPACE_::Global();

  parse_args(argc, argv);

  init();

  // the main thread will sleep until the SIGTERM or SIGINT signal happenes
  while (!glob->get_to_be_shutdown()) {
    pause();
  }

  shutdown();
  return 0;
}

