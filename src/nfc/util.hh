#ifndef NFC_UTIL_HH
#define NFC_UTIL_HH

#include "type_traits.hh"
#include <node.h>
#include <string>
#include <uv.h>
#include <vector>


namespace nfc {

  template<class S, typename T>
  class RawObject;


  class Lock {
    friend class RdLock;
    friend class WrLock;

    mutable uv_rwlock_t lock;

  public:
    Lock();
    ~Lock();

  private:
    // non-copyable
    Lock(const Lock &);
    Lock &operator=(const Lock &);
  };


  class RdLock {
    uv_rwlock_t &lock;

  public:
    RdLock(const Lock &lock);
    ~RdLock();

  private:
    // non-copyable
    RdLock(const RdLock &);
    RdLock &operator=(const RdLock &);
  };


  class WrLock {
    uv_rwlock_t &lock;

  public:
    WrLock(const Lock &lock);
    ~WrLock();

  private:
    // non-copyable
    WrLock(const WrLock &);
    WrLock &operator=(const WrLock &);
  };


  template<class S, typename T>
  class RawObject {
    Lock local_lock;

    size_t *refs;
    Lock *lock;
    T **value;

  public:
    RawObject(T *value);
    ~RawObject();

    void swap(RawObject &other);
    RawObject(const RawObject &other);
    RawObject &operator=(const RawObject &other);

    bool dismiss();

    T *get();
    const T *get() const;
    T *operator*();
    const T *operator*() const;

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

    static v8::Handle<v8::Value> New(const v8::Arguments &args);

    static v8::Handle<v8::Value> Construct();
    template<typename A0>
    static v8::Handle<v8::Value> Construct(const A0 &a0);
    template<typename A0, typename A1>
    static v8::Handle<v8::Value> Construct(const A0 &a0, const A1 &a1);

  public:
    static T &Unwrap(v8::Handle<v8::Value> value);
  };


  template<class T, typename D>
  class AsyncRunner {
  protected:
    typedef void (*run_handler_t)(T &instance, D &data);
    typedef v8::Handle<v8::Value> (*after_handler_t)(v8::Handle<v8::Object> instance, D &data);

    struct Descriptor {
      Descriptor(run_handler_t run_handler, after_handler_t after_handler,
                 v8::Handle<v8::Object> instance, v8::Handle<v8::Function> callback, const D &data);
      uv_work_t req;
      run_handler_t run_handler;
      after_handler_t after_handler;
      v8::Persistent<v8::Object> instance;
      v8::Persistent<v8::Function> callback;
      T &raw_instance;
      D data;
    };

    static void run_async(uv_work_t *req);
    static void after_async(uv_work_t *req, int status);

  public:
    static v8::Handle<v8::Value> Schedule(run_handler_t run_handler, after_handler_t after_handler,
                                          v8::Handle<v8::Value> instance, v8::Handle<v8::Value> callback, const D &data = D());
  };


  extern struct Null {} null;


  v8::Handle<v8::Value> toV8(bool value);
  v8::Handle<v8::Value> toV8(const Null &value);
  v8::Handle<v8::Value> toV8(const std::string &value);

  template<typename T>
  v8::Handle<v8::Value> toV8(const std::vector<T> &value);

  template<typename T>
  typename enable_if<is_signed<T>::value, v8::Handle<v8::Value> >::type toV8(T value);

  template<typename T>
  typename enable_if<is_unsigned<T>::value, v8::Handle<v8::Value> >::type toV8(T value);


  template<typename T>
  T fromV8(v8::Handle<v8::Value> value);


  template<typename T>
  v8::Handle<v8::Value> toExternal(const T &value);


  template<typename T>
  T fromExternal(v8::Handle<v8::Value> value);

}


#include "util.ii"

#endif
