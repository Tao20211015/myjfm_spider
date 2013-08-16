/*******************************************************************************
 * hash.h - hashtable implementation
 * borrow ideas from STL hashtable
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _HASH_H_
#define _HASH_H_

#include <stdint.h>

#include "config.h"
#include "shared.h"
#include "rwlock.h"

_START_MYJFM_NAMESPACE_

typedef uint64_t hash_size_type;

static const uint32_t _num_primes = 28;
static const uint64_t  _prime_list[_num_primes] = {
  53,         97,           193,          389,        769, 
  1543,       3079,         6151,         12289,      24593, 
  49157,      98317,        196613,       393241,     786433, 
  1572869,    3145739,      6291469,      12582917,   25165843, 
  50331653,   100663319,    201326611,    402653189,  805306457, 
  1610612741, 3221225473ul, 4294967291ul
};

// the pure virtual functor class
template <class T>
class HashFunction {
public:
  virtual hash_size_type operator()(T&) = 0;
  virtual ~HashFunction() {}
};

class CharPHashFunction : HashFunction<char*> {
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
  
  virtual ~CharPHashFunction() {}
};

class StringHashFunction : HashFunction<String> {
public:
  hash_size_type operator()(String& s) {
    hash_size_type h = 0;

    uint32_t i = 0;
    for (; i < s.length(); ++i) {
      h = 5 * h + s[i];
    }

    return h;
  }

  virtual ~StringHashFunction() {}
};

class CharHashFunction : HashFunction<char> {
public:
  virtual hash_size_type operator()(char& s) {
    return hash_size_type(s);
  }

  virtual ~CharHashFunction() {}
};

class UcharHashFunction : HashFunction<unsigned char> {
  public:
    virtual hash_size_type operator()(unsigned char& s) {
      return hash_size_type(s);
    }

    virtual ~UcharHashFunction() {}
};

class ShortHashFunction : HashFunction<short> {
  public:
    virtual hash_size_type operator()(short& s) {
      return hash_size_type(s);
    }

    virtual ~ShortHashFunction() {}
};

class UshortHashFunction : HashFunction<unsigned short> {
  public:
    virtual hash_size_type operator()(unsigned short& s) {
      return hash_size_type(s);
    }

    virtual ~UshortHashFunction() {}
};

class IntHashFunction : HashFunction<int> {
  public:
    virtual hash_size_type operator()(int& s) {
      return hash_size_type(s);
    }

    virtual ~IntHashFunction() {}
};

class UintHashFunction : HashFunction<unsigned int> {
  public:
    virtual hash_size_type operator()(unsigned int& s) {
      return hash_size_type(s);
    }

    virtual ~UintHashFunction() {}
};

class LongHashFunction : HashFunction<long> {
  public:
    virtual hash_size_type operator()(long& s) {
      return hash_size_type(s);
    }

    virtual ~LongHashFunction() {}
};

class UlongHashFunction : HashFunction<unsigned long> {
  public:
    virtual hash_size_type operator()(unsigned long& s) {
      return hash_size_type(s);
    }

    virtual ~UlongHashFunction() {}
};

template <class T>
class HashListNode {
public:
  explicit HashListNode(const T& value) {
    _next = NULL;
    // copy constructor
    _value = new T(value);
  }

  ~HashListNode() {
    _next = NULL;
    delete _value;
  }
  
  HashListNode *_next;
  T* _value;
};

// the class T should has overloaded the operator==
// and
// should also have the copy constructor
// and
// the functor class HashFunc should has overloaded the operator()
template <class T, class HashFunc>
class Hash : public Shared {
  typedef HashListNode<T> HashNode;
  typedef Vector<HashNode*> Buckets;

public:
  explicit Hash(hash_size_type n) {
    uint32_t i = 0;
    for (i = 0; i < _num_primes; ++i) {
      if (_prime_list[i] > n) {
        _buckets_size = _prime_list[i];
        break;
      }
    }

    if (i == _num_primes) {
      _buckets_size = _prime_list[_num_primes - 1];
    }

    _buckets.resize(_buckets_size, (HashNode*)0);
    _rwlocks = new RWlock[_buckets_size];
  }

  ~Hash() {
    uint32_t i;
    for (i = 0; i < _buckets_size; ++i) {
      _rwlocks[i].wrlock();
      HashNode* p = _buckets[i];

      while (p) {
        HashNode* q = p;
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
      _buckets[key] = new HashNode(value);

      return S_OK;
    }

    HashNode* nodep = _buckets[key];

    while (nodep) {
      if (*(nodep->_value) == value) {
        return S_ALREADY_EXIST;
      }

      nodep = nodep->_next;
    }

    nodep = new HashNode(value);
    nodep->_next = _buckets[key]->_next;
    _buckets[key] = nodep;

    return S_OK;
  }

  RES_CODE insert_safe(T& value) {
    hash_size_type key = get_key(value);
    RWlock::WriteScopeGuard guard(_rwlocks + key);

    if (_buckets[key] == NULL) {
      _buckets[key] = new HashNode(value);
      return S_OK;
    }

    HashNode* nodep = _buckets[key];
    while (nodep) {
      if (*(nodep->_value) == value) {
        return S_ALREADY_EXIST;
      }

      nodep = nodep->_next;
    }

    nodep = new HashNode(value);
    nodep->_next = _buckets[key]->_next;
    _buckets[key] = nodep;

    return S_OK;
  }

  RES_CODE is_exist(T& value) {
    hash_size_type key = get_key(value);

    if (_buckets[key] == NULL) {
      return S_NOT_EXIST;
    }

    HashNode* nodep = _buckets[key];
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
    RWlock::ReadScopeGuard guard(_rwlocks + key);

    if (_buckets[key] == NULL) {
      return S_NOT_EXIST;
    }

    HashNode* nodep = _buckets[key];

    while (nodep) {
      if (*(nodep->_value) == value) {
        return S_EXIST;
      }
      nodep = nodep->_next;
    }

    return S_NOT_EXIST;
  }

private:
  HashFunc _hasher;

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

