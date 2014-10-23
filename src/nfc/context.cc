#include "context.hh"
#include "device.hh"


namespace nfc {

  RawContext
  RawContext::initialize() {
    nfc_context *context;
    nfc_init(&context);
    return context;
  }


  RawContext::RawContext(nfc_context *context)
    : RawObject(context)
  {
  }


  void
  RawContext::destroy(nfc_context *context) {
    nfc_exit(context);
  }


  Context::Context()
    : context(RawContext::initialize())
  {
  }


  std::string
  Context::version() {
    return nfc_version();
  }


  std::vector<std::string>
  Context::devices() {
    nfc_context *context = this->context.get();
    if (!context) {
      return std::vector<std::string>();
    }
    for (size_t alloc = 1;;) {
      nfc_connstring devices[alloc];
      size_t count = nfc_list_devices(context, devices, alloc);
      if (count <= alloc) {
        // We were able to get all devices.
        return std::vector<std::string>(devices, devices + count);
      }
      alloc = count;
    }
  }


  RawDevice
  Context::open(const std::string &connstring) {
    nfc_context *context = this->context.get();
    if (!context) {
      return RawDevice();
    }
    return RawDevice(nfc_open(context, connstring.length() ? connstring.c_str() : NULL));
  }


  v8::Persistent<v8::Function> Context::constructor;


  void
  Context::Initialize(v8::Handle<v8::Object> exports) {
    v8::HandleScope scope;
    v8::Local<v8::ObjectTemplate> proto;
    v8::Local<v8::FunctionTemplate> tpl;
    Prepare(tpl, proto);

    proto->SetAccessor(v8::String::NewSymbol("version"), GetVersion);

    proto->Set(v8::String::NewSymbol("getDevices"), v8::FunctionTemplate::New(GetDevices)->GetFunction());
    proto->Set(v8::String::NewSymbol("open"), v8::FunctionTemplate::New(Open)->GetFunction());

    Install("Context", exports, tpl);
  }


  v8::Handle<v8::Value>
  Context::CheckNew(v8::Handle<v8::Value> instance) {
    if (!*Unwrap(instance).context) {
      return v8::ThrowException(v8::Exception::Error(v8::String::New("unable to initialize libnfc")));
    }
    return instance;
  }


  v8::Handle<v8::Value>
  Context::GetVersion(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::HandleScope scope;
    return scope.Close(toV8(Unwrap(info.This()).version()));
  }


  struct Context::GetDevicesData {
    std::vector<std::string> devices;
  };


  v8::Handle<v8::Value>
  Context::GetDevices(const v8::Arguments &args) {
    return AsyncRunner<Context, GetDevicesData>::Schedule(RunGetDevices, AfterGetDevices, args.This(), args[0]);
  }


  void
  Context::RunGetDevices(Context &instance, GetDevicesData &data) {
    data.devices = instance.devices();
  }


  v8::Handle<v8::Value>
  Context::AfterGetDevices(v8::Handle<v8::Object> instance, GetDevicesData &data) {
    v8::HandleScope scope;
    return scope.Close(toV8(data.devices));
  }


  struct Context::OpenData {
    std::string connstring;
    RawDevice device;

    OpenData(v8::Handle<v8::Value> connstring_)
      : connstring(connstring_->BooleanValue() ? fromV8<std::string>(connstring_) : "") {}
  };


  v8::Handle<v8::Value>
  Context::Open(const v8::Arguments &args) {
    return AsyncRunner<Context, OpenData>::Schedule(RunOpen, AfterOpen, args.This(), args[1], args[0]);
  }


  void
  Context::RunOpen(Context &instance, OpenData &data) {
    data.device = instance.open(data.connstring);
  }


  v8::Handle<v8::Value>
  Context::AfterOpen(v8::Handle<v8::Object> instance, OpenData &data) {
    v8::HandleScope scope;
    // In case open fails, Device::Construct will throw the exception.
    return scope.Close(Device::Construct(Unwrap(instance).context, data.device));
  }

}
