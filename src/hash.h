#ifndef _HASH_H_
#define _HASH_H_

#include "config.h"
#include "shared.h"
#include "rwlock.h"

_START_MYJFM_NAMESPACE_

typedef unsigned long hash_size_type;

static const int _num_primes = 28;
static const unsigned long  _prime_list[_num_primes] = {
  53,         97,           193,          389,        769, 
  1543,       3079,         6151,         12289,      24593, 
  49157,      98317,        196613,       393241,     786433, 
  1572869,    3145739,      6291469,      12582917,   25165843, 
  50331653,   100663319,    201326611,    402653189,  805306457, 
  1610612741, 3221225473ul, 4294967291ul
};

// the pure virtual functor class
template <class T>
class Hashfunction {
public:
  virtual hash_size_type operator()(T&) = 0;
  virtual ~Hashfunction() {}
};

class Charphashfunction : Hashfunction<char*> {
public:
  virtual hash_size_type operator()(char* s) {
    if (!s) {
      return 0;
    }

    hash_size_type h = 0;

    while (*s) {
      h = 5 * h + *s++;
    }

    return h;
  }
  
  virtual ~Charphashfunction() {}
};

class Stringhashfunction : Hashfunction<String> {
public:
  hash_size_type operator()(String& s) {
    hash_size_type h = 0;

    int i = 0;
    for (; i < s.length(); ++i) {
      h = 5 * h + s[i];
    }

    return h;
  }

  virtual ~Stringhashfunction() {}
};

class Charhashfunction : Hashfunction<char> {
public:
  virtual hash_size_type operator()(char& s) {
    return hash_size_type(s);
  }

  virtual ~Charhashfunction() {}
};

class Ucharhashfunction : Hashfunction<unsigned char> {
  public:
    virtual hash_size_type operator()(unsigned char& s) {
      return hash_size_type(s);
    }

    virtual ~Ucharhashfunction() {}
};

class Shorthashfunction : Hashfunction<short> {
  public:
    virtual hash_size_type operator()(short& s) {
      return hash_size_type(s);
    }

    virtual ~Shorthashfunction() {}
};

class Ushorthashfunction : Hashfunction<unsigned short> {
  public:
    virtual hash_size_type operator()(unsigned short& s) {
      return hash_size_type(s);
    }

    virtual ~Ushorthashfunction() {}
};

class Inthashfunction : Hashfunction<int> {
  public:
    virtual hash_size_type operator()(int& s) {
      return hash_size_type(s);
    }

    virtual ~Inthashfunction() {}
};

class Uinthashfunction : Hashfunction<unsigned int> {
  public:
    virtual hash_size_type operator()(unsigned int& s) {
      return hash_size_type(s);
    }

    virtual ~Uinthashfunction() {}
};

class Longhashfunction : Hashfunction<long> {
  public:
    virtual hash_size_type operator()(long& s) {
      return hash_size_type(s);
    }

    virtual ~Longhashfunction() {}
};

class Ulonghashfunction : Hashfunction<unsigned long> {
  public:
    virtual hash_size_type operator()(unsigned long& s) {
      return hash_size_type(s);
    }

    virtual ~Ulonghashfunction() {}
};

template <class T>
class _hash_list_node {
public:
  _hash_list_node(const T& value) {
    _next = NULL;
    // copy constructor
    _value = new T(value);
  }

  ~_hash_list_node() {
    _next = NULL;
    delete _value;
  }
  
  _hash_list_node *_next;
  T* _value;
};

// the class T should has overloaded the operator==
// and
// should also have the copy constructor
// and
// the functor class Hashfunc should has overloaded the operator()
template <class T, class Hashfunc>
class Hash : public Shared {
  typedef _hash_list_node<T> Hashnode;
  typedef Vector<Hashnode*> Buckets;

public:
  Hash(hash_size_type n) {
    int i = 0;
    for (i = 0; i < _num_primes; ++i) {
      if (_prime_list[i] > n) {
        _buckets_size = _prime_list[i];
        break;
      }
    }

    if (i == _num_primes) {
      _buckets_size = _prime_list[_num_primes - 1];
    }

    _buckets.resize(_buckets_size, (Hashnode*)0);
    _rwlocks = new RWlock[_buckets_size];
  }

  ~Hash() {
    int i;
    for (i = 0; i < _buckets_size; ++i) {
      _rwlocks[i].wrlock();
      Hashnode* p = _buckets[i];

      while (p) {
        Hashnode* q = p;
        p = p->_next;
        delete q;
      }

      _buckets[i] = NULL;
      _rwlocks[i].unlock();
    }

    _buckets.clear();
    _buckets_size = 0;
    delete []_rwlocks;
  }
  
  // for consistency
  RES_CODE size(hash_size_type& s) {
    s = _buckets_size;

    return S_OK;
  }

  RES_CODE insert(T& value) {
    hash_size_type key = get_key(value);
    if (_buckets[key] == NULL) {
      _buckets[key] = new Hashnode(value);

      return S_OK;
    }

    Hashnode* nodep = _buckets[key];

    while (nodep) {
      if (*(nodep->_value) == value) {
        return S_ALREADY_EXIST;
      }

      nodep = nodep->_next;
    }

    nodep = new Hashnode(value);
    nodep->_next = _buckets[key]->_next;
    _buckets[key] = nodep;

    return S_OK;
  }

  RES_CODE insert_safe(T& value) {
    hash_size_type key = get_key(value);

    _rwlocks[key].wrlock();

    if (_buckets[key] == NULL) {
      _buckets[key] = new Hashnode(value);
      _rwlocks[key].unlock();
      return S_OK;
    }

    Hashnode* nodep = _buckets[key];
    while (nodep) {
      if (*(nodep->_value) == value) {
        _rwlocks[key].unlock();
        return S_ALREADY_EXIST;
      }

      nodep = nodep->_next;
    }

    nodep = new Hashnode(value);
    nodep->_next = _buckets[key]->_next;
    _buckets[key] = nodep;

    _rwlocks[key].unlock();
    return S_OK;
  }

  RES_CODE is_exist(T& value) {
    hash_size_type key = get_key(value);

    if (_buckets[key] == NULL) {
      return S_NOT_EXIST;
    }

    Hashnode* nodep = _buckets[key];
    while (nodep) {
      if (*(nodep->_value) == value) {
        return S_EXIST;
      }

      nodep = nodep->_next;
    }

    return S_NOT_EXIST;
  }

  RES_CODE is_exist_safe(T& value) {
    hash_size_type key = get_key(value);
    _rwlocks[key].rdlock();

    if (_buckets[key] == NULL) {
      _rwlocks[key].unlock();
      return S_NOT_EXIST;
    }

    Hashnode* nodep = _buckets[key];

    while (nodep) {
      if (*(nodep->_value) == value) {
        _rwlocks[key].unlock();
        return S_EXIST;
      }
      nodep = nodep->_next;
    }

    _rwlocks[key].unlock();
    return S_NOT_EXIST;
  }

private:
  Hashfunc _hasher;

  hash_size_type get_key(T& value) {
    return get_key(bucket_num(value));
  }

  hash_size_type get_key(hash_size_type bn) {
    return bn % _buckets_size;
  }

  hash_size_type bucket_num(T& value) {
    return _hasher(value);
  }

  Buckets _buckets;
  RWlock* _rwlocks;

  hash_size_type _buckets_size;
};

_END_MYJFM_NAMESPACE_

#endif

