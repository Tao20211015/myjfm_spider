#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include "config.h"
#include "shared_pointer.h"
#include "callback.h"

_START_MYJFM_NAMESPACE_

#define MAX_EPOLL_FD 65535

class EventLoop;

typedef void (*callback_func)(EventLoop*, int, void*);

enum _event_mask {
  EVENT_DUMMY = 0x0, 
  EVENT_READ = 0x1, 
  EVENT_WRITE = 0x2,  
  EVENT_ERROR = 0x4 
};

class Event {
public:
  int _event_fd;
  unsigned int _mask;

  int _use_read_callback;
  SharedPointer<Callback> _read_callback;
  callback_func _read_callback_func;
  void* _read_arg;

  int _use_write_callback;
  SharedPointer<Callback> _write_callback;
  callback_func _write_callback_func;
  void* _write_arg;

  int _use_error_callback;
  SharedPointer<Callback> _error_callback;
  callback_func _error_callback_func;
  void* _error_arg;

  Event() : 
    _event_fd(-1), 
    _mask(EVENT_DUMMY), 
    _use_read_callback(0), 
    _read_callback(SharedPointer<Callback>(NULL)), 
    _read_callback_func(NULL), 
    _read_arg(NULL), 
    _use_write_callback(0), 
    _write_callback(SharedPointer<Callback>(NULL)), 
    _write_callback_func(NULL), 
    _write_arg(NULL), 
    _use_error_callback(0), 
    _error_callback(SharedPointer<Callback>(NULL)), 
    _error_callback_func(NULL), 
    _error_arg(NULL) {
  }
};

class EventLoop : public Shared {
  enum _enum_state {
    UNCONSTRUCTED = 0x0, 
    CONSTRUCTED = 0x1,  
    INITIALIZED = 0x2, 
    LOOP = 0x4, 
    DESTROYED = 0x8 
  };

public:
  EventLoop();
  ~EventLoop();

  RES_CODE init();
  RES_CODE destroy();
  RES_CODE add_event(Event*);
  RES_CODE del_event(Event*);
  RES_CODE loop();
  RES_CODE stop();
  int is_stopped();

private:
  volatile _enum_state _state;
  volatile int _stop;
  int _epoll_fd;
  Event* _events;
  struct epoll_event* _epoll_events;

  EventLoop(const EventLoop&);
  EventLoop& operator=(const EventLoop&);
};

_END_MYJFM_NAMESPACE_

#endif

