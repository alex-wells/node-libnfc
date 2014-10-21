#include "device.hh"
#include "target.hh"


namespace nfc {

  RawDevice::RawDevice(nfc_device *device)
    : RawObject(device)
  {
  }


  void
  RawDevice::destroy(nfc_device *device) {
    nfc_close(device);
  }


  Device::Device(RawContext context_, RawDevice device_)
    : context(context_), device(device_)
  {
    // Consider all devices initiator.
    if (!set_as_initiator()) {
      close();
    }
  }


  bool
  Device::close() {
    if (!device) {
      return false;
    }
    device.dismiss();
    context = NULL;
    return true;
  }


  bool
  Device::set_idle() {
    return device && !nfc_idle(*device);
  }


  std::string
  Device::name() {
    return device ? nfc_device_get_name(*device) : "";
  }


  std::string
  Device::connstring() {
    return device ? nfc_device_get_connstring(*device) : "";
  }


  bool
  Device::set_as_initiator() {
    return device && !nfc_initiator_init(*device);
  }


  bool
  Device::poll_target(nfc_target &target) {
    if (!device) {
      return false;
    }
    const nfc_modulation modulations[] = {
      {.nmt = NMT_ISO14443A, .nbr = NBR_106},
      {.nmt = NMT_ISO14443B, .nbr = NBR_106},
      {.nmt = NMT_FELICA, .nbr = NBR_212},
      {.nmt = NMT_FELICA, .nbr = NBR_424},
      {.nmt = NMT_JEWEL, .nbr = NBR_106},
    };
    const size_t modulations_count = sizeof(modulations) / sizeof(modulations[0]);
    const uint8_t poll_period = 2;  // polling period (in units of 150 ms)
    const uint8_t poll_count = 2;  // number of polling attempts
    int result = nfc_initiator_poll_target(*device, modulations, modulations_count,
                                           poll_count, poll_period, &target);
    return result > 0;
  }


  bool
  Device::is_present(const nfc_target &target) {
    return device && !nfc_initiator_target_is_present(*device, &target);
  }


  v8::Handle<v8::Value>
  Device::Construct(RawContext context, RawDevice device) {
    return ObjectWrap::Construct(context, device);
  }


  Device *
  Device::Create(const v8::Arguments &args) {
    return ObjectWrap::Create<RawContext, RawDevice>(args);
  }


  v8::Persistent<v8::Function> Device::constructor;


  void
  Device::Initialize(v8::Handle<v8::Object> exports) {
    v8::HandleScope scope;
    v8::Local<v8::ObjectTemplate> proto;
    v8::Local<v8::FunctionTemplate> tpl;
    Prepare(tpl, proto);

    proto->SetAccessor(v8::String::NewSymbol("name"), GetName);
    proto->SetAccessor(v8::String::NewSymbol("connstring"), GetConnstring);

    proto->Set(v8::String::NewSymbol("close"), v8::FunctionTemplate::New(Close)->GetFunction());
    proto->Set(v8::String::NewSymbol("setIdle"), v8::FunctionTemplate::New(SetIdle)->GetFunction());

    proto->Set(v8::String::NewSymbol("pollTarget"), v8::FunctionTemplate::New(PollTarget)->GetFunction());
    proto->Set(v8::String::NewSymbol("isPresent"), v8::FunctionTemplate::New(IsPresent)->GetFunction());

    Install("Device", exports, tpl);
  }


  v8::Handle<v8::Value>
  Device::CheckNew(v8::Handle<v8::Value> instance) {
    if (!Unwrap(instance).device) {
      return v8::ThrowException(v8::Exception::Error(v8::String::New("unable to open device")));
    }
    return instance;
  }


  v8::Handle<v8::Value>
  Device::GetName(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::HandleScope scope;
    return scope.Close(toV8(Unwrap(info.This()).name()));
  }


  v8::Handle<v8::Value>
  Device::GetConnstring(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::HandleScope scope;
    return scope.Close(toV8(Unwrap(info.This()).connstring()));
  }


  v8::Handle<v8::Value>
  Device::Close(const v8::Arguments &args) {
    v8::HandleScope scope;
    return scope.Close(toV8(Unwrap(args.This()).close()));
  }


  v8::Handle<v8::Value>
  Device::SetIdle(const v8::Arguments &args) {
    v8::HandleScope scope;
    return scope.Close(toV8(Unwrap(args.This()).set_idle()));
  }


  // TODO: Make this asynchronous.
  v8::Handle<v8::Value>
  Device::PollTarget(const v8::Arguments &args) {
    v8::HandleScope scope;
    nfc_target target;
    return scope.Close(Unwrap(args.This()).poll_target(target) ? Target::Construct(target) : toV8(false));
  }


  v8::Handle<v8::Value>
  Device::IsPresent(const v8::Arguments &args) {
    v8::HandleScope scope;
    return scope.Close(toV8(Unwrap(args.This()).is_present(Target::Unwrap(args[0]).target)));
  }

}
