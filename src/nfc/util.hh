#ifndef NFC_UTIL_HH
#define NFC_UTIL_HH

#include <node.h>
#include <string>
#include <uv.h>
#include <vector>


namespace nfc {

  template<class S, typename T>
  class RawObject {
    size_t *refs;
    T **value;

  public:
    RawObject(T *value);
    ~RawObject();

    void swap(RawObject &other);
    RawObject(const RawObject &other);
    RawObject &operator=(const RawObject &other);

    void dismiss();

    T *get();
    const T *get() const;
    T *operator*();
    const T *operator*() const;

    bool is_valid() const;
    operator bool() const;

  protected:
    static void destroy(T *value);
  };


  template<class T>
  class ObjectWrap:
    public node::ObjectWrap
  {
  protected:
    static void Prepare(v8::Handle<v8::FunctionTemplate> &tpl, v8::Local<v8::ObjectTemplate> &proto);
    static void Install(const char name[], v8::Handle<v8::Object> &exports, v8::Local<v8::FunctionTemplate> &tpl);

    static T *Create(const v8::Arguments &args);
    template<typename A0>
    static T *Create(const v8::Arguments &args);
    template<typename A0, typename A1>
    static T *Create(const v8::Arguments &args);

    static v8::Handle<v8::Value> CheckNew(v8::Handle<v8::Value> instance);

    static T &Unwrap(v8::Handle<v8::Value> value);

    static v8::Handle<v8::Value> New(const v8::Arguments &args);

    static v8::Handle<v8::Value> Construct();
    template<typename A0>
    static v8::Handle<v8::Value> Construct(const A0 &a0);
    template<typename A0, typename A1>
    static v8::Handle<v8::Value> Construct(const A0 &a0, const A1 &a1);
  };


  class Lock {
  public:
    Lock();
    ~Lock();

    operator uv_rwlock_t &() { return lock; }
    operator const uv_rwlock_t &() const { return lock; }

  private:
    // non-copyable
    Lock(const Lock &);
    Lock &operator=(const Lock &);

  private:
    uv_rwlock_t lock;
  };


  class RdLock {
  public:
    RdLock(uv_rwlock_t &lock);
    ~RdLock();

  private:
    // non-copyable
    RdLock(const RdLock &);
    RdLock &operator=(const RdLock &);

  private:
    uv_rwlock_t &lock;
  };


  class WrLock {
  public:
    WrLock(uv_rwlock_t &lock);
    ~WrLock();

  private:
    // non-copyable
    WrLock(const WrLock &);
    WrLock &operator=(const WrLock &);

  private:
    uv_rwlock_t &lock;
  };


  template<typename T>
  v8::Handle<v8::Value> toV8(const T &value);

  template<typename T>
  T fromV8(v8::Handle<v8::Value> value);


  template<typename T>
  v8::Handle<v8::Value> toExternal(const T &value);

  template<typename T>
  T fromExternal(v8::Handle<v8::Value> value);

}


#include "util.ii"

#endif
