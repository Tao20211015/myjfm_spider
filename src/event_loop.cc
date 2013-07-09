#include <string.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "event_loop.h"

_START_MYJFM_NAMESPACE_

EventLoop::EventLoop() : 
  _state(CONSTRUCTED), 
  _stop(1) {
  _events = new Event[MAX_EPOLL_FD];
  _epoll_events = new struct epoll_event[MAX_EPOLL_FD];
  memset(_epoll_events, 0, sizeof(struct epoll_event) * MAX_EPOLL_FD);
}

EventLoop::~EventLoop() {
  delete [] _events;
  delete [] _epoll_events;
}

RES_CODE EventLoop::init() {
  if (_state != CONSTRUCTED) {
    return S_FAIL;
  }

  _epoll_fd = epoll_create(MAX_EPOLL_FD);
  if (_epoll_fd == -1) {
    return S_EPOLL_CREATE_FAILED;
  }

  _state = INITIALIZED;
  return S_OK;
}

RES_CODE EventLoop::destroy() {
  if (_state != INITIALIZED && 
      _state != LOOP) {
    return S_FAIL;
  }

  close(_epoll_fd);
  _state = DESTROYED;
  _stop = 1;

  return S_OK;
}

RES_CODE EventLoop::add_event(Event* event) {
  if (_state != INITIALIZED && _state != LOOP) {
    return S_FAIL;
  }
  
  if (!event) {
    return S_FAIL;
  }
  
  int fd = event->_event_fd;
  unsigned int mask = event->_mask;
  int op;
  
  if (_events[fd]._mask == EVENT_DUMMY) {
    op = EPOLL_CTL_ADD;
  } else {
    op = EPOLL_CTL_MOD;
    mask |= _events[fd]._mask;
  }
  
  _events[fd]._mask = mask;                                                
  
  struct epoll_event tmp_event;
  memset(&tmp_event.data, 0, sizeof(tmp_event.data));
  tmp_event.data.fd = fd;
  tmp_event.events = 0;
  
  if (mask & EVENT_READ) {
    tmp_event.events |= EPOLLIN;

    if (event->_use_read_callback) {
      _events[fd]._use_read_callback = event->_use_read_callback;
      _events[fd]._read_callback = event->_read_callback;
    } else {
      _events[fd]._read_callback_func = event->_read_callback_func;
      _events[fd]._read_arg = event->_read_arg;
    }
  }
  
  if (mask & EVENT_WRITE) {
    tmp_event.events |= EPOLLOUT;

    if (event->_use_write_callback) {
      _events[fd]._use_write_callback = event->_use_write_callback;
      _events[fd]._write_callback = event->_write_callback;
    } else {
      _events[fd]._write_callback_func = event->_write_callback_func;
      _events[fd]._write_arg = event->_write_arg;                     
    }
  }

  if (mask & EVENT_ERROR) {                                               
    tmp_event.events |= EPOLLERR;

    if (event->_use_error_callback) {
      _events[fd]._use_error_callback = event->_use_error_callback;
      _events[fd]._error_callback = event->_error_callback;
    } else {
      _events[fd]._error_callback_func = event->_error_callback_func;
      _events[fd]._error_arg = event->_error_arg;                     
    }
  }

  if (!epoll_ctl(_epoll_fd, op, fd, &tmp_event)) {
    return S_FAIL;
  }

  return S_OK;
}

RES_CODE EventLoop::del_event(Event* event) {
  if (_state != INITIALIZED && _state != LOOP) {
    return S_FAIL;
  }

  if (!event) {
    return S_FAIL;
  }

  int fd = event->_event_fd;
  unsigned int mask = _events[fd]._mask & (~(event->_mask));
  _events[fd]._mask = mask;
  struct epoll_event tmp_event;
  tmp_event.data.fd = fd;
  tmp_event.events = 0;

  if (mask & EVENT_READ) {
    tmp_event.events |= EPOLLIN;
  }

  if (mask & EVENT_WRITE) {
    tmp_event.events |= EPOLLOUT;
  }

  int op;

  if (mask != EVENT_DUMMY) {
    op = EPOLL_CTL_MOD;
  } else {
    op = EPOLL_CTL_DEL;
  }

  if (!epoll_ctl(_epoll_fd, op, fd, &tmp_event)) {
    return S_FAIL;
  }

  return S_OK;
}

RES_CODE EventLoop::loop() {
  if (_state != INITIALIZED) {
    return S_FAIL;
  }

  _state = LOOP;
  _stop = false;

  while (!_stop) {
    int j;
    int num_events = epoll_wait(_epoll_fd, _epoll_events, MAX_EPOLL_FD, 5);

    for (j = 0; j < num_events; j++) {
      struct epoll_event* event = _epoll_events + j;
      int fd = event->data.fd;

      if (event->events & EPOLLIN) {
        if (_events[fd]._use_read_callback) {
          (*(_events[fd]._read_callback))(this);
        } else if (_events[fd]._read_callback_func) {
          (_events[fd]._read_callback_func)(this, fd, _events[fd]._read_arg);
        }
      }

      if (event->events & EPOLLOUT) {
        if (_events[fd]._use_write_callback) {
          (*(_events[fd]._write_callback))(this);
        } else if (_events[fd]._write_callback_func) {
          (_events[fd]._write_callback_func)(this, fd, _events[fd]._write_arg);
        }
      }

      if (event->events & EPOLLERR) {
        if (_events[fd]._use_error_callback) {
          (*(_events[fd]._error_callback))(this);
        } else if (_events[fd]._error_callback_func) {
          (_events[fd]._error_callback_func)(this, fd, _events[fd]._error_arg);
        }
      }
    }
  }

  _state = DESTROYED;
  return S_OK;
}

RES_CODE EventLoop::stop() {
  _stop = 1;
  return S_OK;
}

int EventLoop::is_stopped() {
  return _stop;
}

_END_MYJFM_NAMESPACE_

