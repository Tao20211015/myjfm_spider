#ifndef _SHAREDPOINTER_H_
#define _SHAREDPOINTER_H_

// every class inherited from this class must 
// have the inc_ref and dec_ref function
template <class T>
class Sharedpointer {
#define INC_REF do { \
  if (_p) { \
    _p->inc_ref(); \
  } \
} while (0)
public:
  Sharedpointer(const Sharedpointer& p) : _p(p._p) {
    INC_REF;
  }

  explicit Sharedpointer(T* t = NULL) : _p(t) {
    INC_REF;
  }

  template <class U>
  Sharedpointer(const Sharedpointer<U>& childp) : _p(childp.getp()) {
    INC_REF;
  }

  Sharedpointer& operator=(const Sharedpointer& r) {
    if (_p != r._p) {
      if (_p) {
        bool flag = true;
        if (_p.dec_ref(&flag) == 0 && flag) {
          delete _p;
        }
      }
      if ((_p = r._p) != NULL) {
        _p->inc_ref();
      }
    }
  }

  ~Sharedpointer() {
    bool flag = true;
    if (_p != NULL) {
      if (_p->dec_ref(&flag) == 0) {
        if (flag) {
          delete _p;
        }
        _p = NULL;
      }
    }
  }

  T* getp() const {
    return _p;
  }

  T* operator->() const {
    return _p;
  }

  T& operator*() const {
    return *_p;
  }
private:
  T* _p;
#undef INC_REF
};

#endif

