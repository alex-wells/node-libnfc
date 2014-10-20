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
    for (size_t alloc = 1;;) {
      nfc_connstring devices[alloc];
      size_t count = nfc_list_devices(*context, devices, alloc);
      if (count <= alloc) {
        // We were able to get all devices.
        return std::vector<std::string>(devices, devices + count);
      }
      alloc = count;
    }
  }


  RawDevice
  Context::open(const std::string &connstring) {
    return RawDevice(nfc_open(*context, connstring.length() ? connstring.c_str() : NULL));
  }


  v8::Persistent<v8::Function> Context::constructor;


  void
  Context::Initialize(v8::Handle<v8::Object> exports) {
    v8::HandleScope scope;
    v8::Local<v8::ObjectTemplate> proto;
    v8::Local<v8::FunctionTemplate> tpl;
    Prepare(tpl, proto);

    proto->SetAccessor(v8::String::NewSymbol("version"), GetVersion);
    proto->SetAccessor(v8::String::NewSymbol("devices"), GetDevices);

    proto->Set(v8::String::NewSymbol("open"), v8::FunctionTemplate::New(Open)->GetFunction());

    Install("Context", exports, tpl);
  }


  v8::Handle<v8::Value>
  Context::CheckNew(v8::Handle<v8::Value> instance) {
    if (!Unwrap(instance).context) {
      return v8::ThrowException(v8::Exception::Error(v8::String::New("unable to initialize libnfc")));
    }
    return instance;
  }


  v8::Handle<v8::Value>
  Context::GetVersion(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::HandleScope scope;
    return scope.Close(toV8(Unwrap(info.This()).version()));
  }


  v8::Handle<v8::Value>
  Context::GetDevices(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::HandleScope scope;
    return scope.Close(toV8(Unwrap(info.This()).devices()));
  }


  v8::Handle<v8::Value>
  Context::Open(const v8::Arguments &args) {
    v8::HandleScope scope;
    Context &context = Unwrap(args.This());
    return scope.Close(Device::Construct(context.context,
                                         context.open(args[0]->BooleanValue() ? fromV8<std::string>(args[0]) : "")));
  }

}
