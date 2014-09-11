#ifndef NFC_DEVICE_HH
#define NFC_DEVICE_HH

#include "context.hh"
#include <node.h>
#include <string>


namespace nfc {

  class Device
    : public node::ObjectWrap
  {
  protected:
    template<class T>
    static v8::Local<v8::FunctionTemplate> Initialize();

    template<class T>
    static v8::Handle<v8::Object> Create(v8::Handle<v8::Value> connstring);

    template<class T>
    static T &Unwrap(v8::Handle<v8::Value> value);

  public:
    static Device &Unwrap(v8::Handle<v8::Value> value);

    explicit Device(nfc_device *device);
    virtual ~Device();

    void close();

    bool available();
    std::string name();
    bool abort_command();

  protected:
    nfc_device *device;

    friend class Target;

    uv_rwlock_t device_lock;

  private:
    template<class T>
    static v8::Handle<v8::Value> New(const v8::Arguments &args);

    static v8::Handle<v8::Value> GetAvailable(v8::Local<v8::String> property, const v8::AccessorInfo &info);
    static v8::Handle<v8::Value> GetName(v8::Local<v8::String> property, const v8::AccessorInfo &info);

    static v8::Handle<v8::Value> AbortCommand(const v8::Arguments &args);
    static v8::Handle<v8::Value> Close(const v8::Arguments &args);
  };


  class InitiatorDevice
    : public Device
  {
  public:
    static void Initialize();

    static v8::Handle<v8::Object> Create(v8::Handle<v8::Value> connstring);

  public:
    static InitiatorDevice &Unwrap(v8::Handle<v8::Value> value);

    explicit InitiatorDevice(nfc_device *device);

    bool select_passive_target(nfc_target &target);

  private:
    struct SelectPassiveTargetData {
      SelectPassiveTargetData(v8::Handle<v8::Object> device, v8::Handle<v8::Function> callback);
      uv_work_t req;
      InitiatorDevice *raw_device;
      v8::Persistent<v8::Object> device;
      v8::Persistent<v8::Function> callback;
      nfc_target target;
      bool success;
    };

    static void async_select_passive_target(uv_work_t *req);
    static void after_select_passive_target(uv_work_t *req, int status);

  private:
    static v8::Handle<v8::Value> SelectPassiveTarget(const v8::Arguments &args);

    static v8::Persistent<v8::Function> constructor;

    friend class Device;
  };

}

#endif
