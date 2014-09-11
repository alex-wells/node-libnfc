#include "device.hh"
#include "target.hh"
#include "util.hh"


namespace nfc {

  template<class T>
  v8::Local<v8::FunctionTemplate>
  Device::Initialize() {
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(New<T>);
    tpl->SetClassName(v8::String::NewSymbol("Device"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    v8::Local<v8::ObjectTemplate> proto = tpl->PrototypeTemplate();
    proto->SetAccessor(v8::String::NewSymbol("available"), GetAvailable);
    proto->SetAccessor(v8::String::NewSymbol("name"), GetName);
    proto->Set(v8::String::NewSymbol("abortCommand"),
               v8::FunctionTemplate::New(AbortCommand)->GetFunction());
    proto->Set(v8::String::NewSymbol("close"),
               v8::FunctionTemplate::New(Close)->GetFunction());

    return tpl;
  }


  template<class T>
  v8::Handle<v8::Object>
  Device::Create(v8::Handle<v8::Value> connstring) {
    v8::HandleScope scope;
    const int argc = 1;
    v8::Handle<v8::Value> argv[argc] = {connstring};
    return scope.Close(T::constructor->NewInstance(argc, argv));
  }


  template<class T>
  T &
  Device::Unwrap(v8::Handle<v8::Value> value) {
    // Cannot throw exceptions here, so an assertion is the best
    // we can do (during development).
    assert(value->IsObject());
    return *node::ObjectWrap::Unwrap<T>(value.As<v8::Object>());
  }


  Device &
  Device::Unwrap(v8::Handle<v8::Value> value) {
    return Device::Unwrap<Device>(value);
  }


  Device::Device(nfc_device *device_)
    : device(device_)
  {
    uv_rwlock_init(&device_lock);
  }


  Device::~Device() {
    close();
    uv_rwlock_destroy(&device_lock);
  }


  void
  Device::close() {
    if (device) {
      WrLock lock(device_lock);
      nfc_close(device);
      device = NULL;
    }
  }


  bool
  Device::available() {
    RdLock lock(device_lock);
    return bool(device);
  }


  std::string
  Device::name() {
    RdLock lock(device_lock);
    return device ? nfc_device_get_name(device) : "";
  }


  bool
  Device::abort_command() {
    RdLock lock(device_lock);
    return device && nfc_abort_command(device) == 0;
  }


  template<class T>
  v8::Handle<v8::Value>
  Device::New(const v8::Arguments &args) {
    v8::HandleScope scope;

    if (args.IsConstructCall()) {
      nfc_device *device = NULL;
      if (args[0]->IsString()) {
        v8::String::AsciiValue raw(args[0]);
        device = nfc_open(nfc::context, *raw);
      }
      else {
        device = nfc_open(nfc::context, NULL);
      }
      Device *obj = new T(device);
      obj->Wrap(args.This());
      return args.This();
    }
    else {
      const int argc = 1;
      v8::Handle<v8::Value> argv[argc] = {args[0]};
      return scope.Close(T::constructor->NewInstance(argc, argv));
    }
  }


  v8::Handle<v8::Value>
  Device::GetAvailable(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::HandleScope scope;
    Device &obj = Unwrap(info.This());
    return scope.Close(v8::Boolean::New(obj.available()));
  }


  v8::Handle<v8::Value>
  Device::GetName(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::HandleScope scope;
    Device &obj = Unwrap(info.This());
    std::string name = obj.name();
    return scope.Close(v8::String::New(name.data(), name.length()));
  }


  v8::Handle<v8::Value>
  Device::AbortCommand(const v8::Arguments &args) {
    v8::HandleScope scope;
    Device &obj = Unwrap(args.This());
    return scope.Close(v8::Boolean::New(obj.abort_command()));
  }


  v8::Handle<v8::Value>
  Device::Close(const v8::Arguments &args) {
    v8::HandleScope scope;
    Device &obj = Unwrap(args.This());
    obj.close();
    return scope.Close(v8::Undefined());
  }

}


namespace nfc {

  void
  InitiatorDevice::Initialize() {
    v8::Local<v8::FunctionTemplate> tpl = Device::Initialize<InitiatorDevice>();

    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("selectPassiveTarget"),
                                  v8::FunctionTemplate::New(SelectPassiveTarget)->GetFunction());

    constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
  }


  v8::Handle<v8::Object>
  InitiatorDevice::Create(v8::Handle<v8::Value> connstring) {
    return Device::Create<InitiatorDevice>(connstring);
  }


  InitiatorDevice &
  InitiatorDevice::Unwrap(v8::Handle<v8::Value> value) {
    return Device::Unwrap<InitiatorDevice>(value);
  }


  InitiatorDevice::InitiatorDevice(nfc_device *device_)
    : Device(device_)
  {
    if (available()) {
      // Put device into initiator mode, if available.
      if (nfc_initiator_init(device) != 0) {
        // Putting into initiator mode failed: close it.
        close();
      }
    }
  }


  bool
  InitiatorDevice::select_passive_target(nfc_target &target) {
    RdLock lock(device_lock);

    if (!device) {
      return false;
    }

    nfc_modulation modulation = {
      .nmt = NMT_ISO14443A,
      .nbr = NBR_106,
    };

    if (nfc_initiator_select_passive_target(device, modulation, NULL, 0, &target) != 1) {
      // Did not get exactly one target.
      return false;
    }

    return true;
  }


  InitiatorDevice::SelectPassiveTargetData::SelectPassiveTargetData(v8::Handle<v8::Object> device_, v8::Handle<v8::Function> callback_) {
    req.data = this;
    v8::HandleScope scope;
    device = v8::Persistent<v8::Object>::New(device_);
    raw_device = &Unwrap(device);
    callback = v8::Persistent<v8::Function>::New(callback_);
  }


  void
  InitiatorDevice::async_select_passive_target(uv_work_t *req) {
    SelectPassiveTargetData *data = static_cast<SelectPassiveTargetData *>(req->data);
    data->success = data->raw_device->select_passive_target(data->target);
  }


  void
  InitiatorDevice::after_select_passive_target(uv_work_t *req, int status) {
    SelectPassiveTargetData *data = static_cast<SelectPassiveTargetData *>(req->data);
    v8::HandleScope scope;

    v8::Handle<v8::Value> target = v8::Null();
    if (data->success) {
      target = Target::Create(data->device, data->target);
    }

    const int argc = 1;
    v8::Handle<v8::Value> argv[argc] = {target};
    node::MakeCallback(data->device, data->callback, argc, argv);

    delete data;
  }


  v8::Handle<v8::Value>
  InitiatorDevice::SelectPassiveTarget(const v8::Arguments &args) {
    v8::HandleScope scope;
    if (!args[0]->IsFunction()) {
      v8::ThrowException(v8::Exception::TypeError(v8::String::New("expected callback function")));
      return scope.Close(v8::Undefined());
    }
    SelectPassiveTargetData *data = new SelectPassiveTargetData(args.This(), args[0].As<v8::Function>());
    uv_queue_work(uv_default_loop(), &data->req, async_select_passive_target, after_select_passive_target);
    return scope.Close(v8::Undefined());
  }


  v8::Persistent<v8::Function> InitiatorDevice::constructor;

}
