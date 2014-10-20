#include "device.hh"


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
  }


  void
  Device::close() {
    device.dismiss();
    context = NULL;
  }


  std::string
  Device::name() {
    return device ? nfc_device_get_name(*device) : "";
  }


  std::string
  Device::connstring() {
    return device ? nfc_device_get_connstring(*device) : "";
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
    Device &device = Unwrap(args.This());
    bool was_set = device.device;
    device.close();
    return scope.Close(v8::Boolean::New(was_set && !device.device));
  }

}
