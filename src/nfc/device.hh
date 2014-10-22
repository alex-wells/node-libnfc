#ifndef NFC_DEVICE_HH
#define NFC_DEVICE_HH

#include "context.hh"
#include "util.hh"
#include <nfc/nfc.h>


namespace nfc {

  class RawDevice:
    public RawObject<RawDevice, nfc_device>
  {
  public:
    RawDevice(nfc_device *device = NULL);

    static void destroy(nfc_device *device);
  };


  class Device:
    public nfc::ObjectWrap<Device>
  {
  protected:
    RawContext context;
    RawDevice device;

  public:
    Device(RawContext context, RawDevice device);

    bool close();
    bool set_idle();

    std::string name();
    std::string connstring();

    bool set_as_initiator();

    // initiator functions
    bool poll_target(nfc_target &target);
    bool is_present(const nfc_target &target);

  public:
    static v8::Handle<v8::Value> Construct(RawContext context, RawDevice device);

  public:
    static v8::Persistent<v8::Function> constructor;

    static Device *Create(const v8::Arguments &args);
    static void Initialize(v8::Handle<v8::Object> exports);
    static v8::Handle<v8::Value> CheckNew(v8::Handle<v8::Value> instance);

    static v8::Handle<v8::Value> GetName(v8::Local<v8::String> property, const v8::AccessorInfo &info);
    static v8::Handle<v8::Value> GetConnstring(v8::Local<v8::String> property, const v8::AccessorInfo &info);

    static v8::Handle<v8::Value> Close(const v8::Arguments &args);
    static v8::Handle<v8::Value> SetIdle(const v8::Arguments &args);

    static v8::Handle<v8::Value> PollTarget(const v8::Arguments &args);
    static v8::Handle<v8::Value> IsPresent(const v8::Arguments &args);

  protected:
    struct PollTargetData;
    static void RunPollTarget(Device &instance, PollTargetData &data);
    static v8::Handle<v8::Value> AfterPollTarget(v8::Handle<v8::Object> instance, PollTargetData &data);
  };

}

#endif
