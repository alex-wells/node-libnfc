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
    if (!device.dismiss()) {
      return false;
    }
    context = NULL;
    return true;
  }


  bool
  Device::set_idle() {
    nfc_device *device = this->device.get();
    return device && !nfc_idle(device);
  }


  std::string
  Device::name() {
    nfc_device *device = this->device.get();
    return device ? nfc_device_get_name(device) : "";
  }


  std::string
  Device::connstring() {
    nfc_device *device = this->device.get();
    return device ? nfc_device_get_connstring(device) : "";
  }


  bool
  Device::set_as_initiator() {
    nfc_device *device = this->device.get();
    return device && !nfc_initiator_init(device);
  }


  int
  Device::poll_target(nfc_target &target) {
    nfc_device *device = this->device.get();
    if (!device) {
      return NFC_EIO;
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
    int result = nfc_initiator_poll_target(device, modulations, modulations_count,
                                           poll_count, poll_period, &target);
    return result < 0 ? result : (result ? 1 : 0);
  }


  int
  Device::is_present(const nfc_target &target) {
    nfc_device *device = this->device.get();
    if (!device) {
      return NFC_EIO;
    }
    return nfc_initiator_target_is_present(device, &target);
  }


  int
  Device::transceive(const std::vector<uint8_t> &transmit, std::vector<uint8_t> &receive) {
    const int timeout = -1;  // timeout in ms (-1 is default)
    nfc_device *device = this->device.get();
    if (!device) {
      return NFC_EIO;
    }
    int result = nfc_initiator_transceive_bytes(device, transmit.data(), transmit.size(),
                                                receive.data(), receive.size(), timeout);
    receive.resize(result < 0 ? 0 : size_t(result));
    return result;
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
    proto->Set(v8::String::NewSymbol("transceive"), v8::FunctionTemplate::New(Transceive)->GetFunction());
    proto->Set(v8::String::NewSymbol("isPresent"), v8::FunctionTemplate::New(IsPresent)->GetFunction());

    Install("Device", exports, tpl);
  }


  v8::Handle<v8::Value>
  Device::CheckNew(v8::Handle<v8::Value> instance) {
    if (!*Unwrap(instance).device) {
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


  struct Device::PollTargetData {
    bool error;
    bool got_target;
    nfc_target target;
  };


  v8::Handle<v8::Value>
  Device::PollTarget(const v8::Arguments &args) {
    return AsyncRunner<Device, PollTargetData>::Schedule
      (RunPollTarget, AfterPollTarget, args.This(), args[0]);
  }


  void
  Device::RunPollTarget(Device &instance, PollTargetData &data) {
    int result = instance.poll_target(data.target);
    data.got_target = result > 0;
    data.error = result < 0;
  }


  v8::Handle<v8::Value>
  Device::AfterPollTarget(v8::Handle<v8::Object> instance, PollTargetData &data) {
    v8::HandleScope scope;
    if (!data.error) {
      return scope.Close(data.got_target ? Target::Construct(data.target) : toV8(null));
    }
    return v8::ThrowException(v8::Exception::Error(v8::String::New("unable to poll for targets")));
  }


  struct Device::TransceiveData {
    std::vector<uint8_t> transmit;
    std::vector<uint8_t> receive;
    bool error;

    TransceiveData(v8::Handle<v8::Value> transmit_, v8::Handle<v8::Value> receive_capacity_)
      : transmit(fromV8<std::vector<uint8_t> >(transmit_)), receive(fromV8<size_t>(receive_capacity_)) {}
  };


  v8::Handle<v8::Value>
  Device::Transceive(const v8::Arguments &args) {
    return AsyncRunner<Device, TransceiveData>::Schedule
      (RunTransceive, AfterTransceive, args.This(), args[2], TransceiveData(args[0], args[1]));
  }


  void
  Device::RunTransceive(Device &instance, TransceiveData &data) {
    data.error = instance.transceive(data.transmit, data.receive) < 0;
  }


  v8::Handle<v8::Value>
  Device::AfterTransceive(v8::Handle<v8::Object> instance, TransceiveData &data) {
    v8::HandleScope scope;
    if (!data.error) {
      return scope.Close(toV8(data.receive));
    }
    return v8::ThrowException(v8::Exception::Error(v8::String::New("unable to transceive data")));
  }


  struct Device::GetIsPresentData {
    nfc_target target;
    bool is_present;

    GetIsPresentData(v8::Handle<v8::Value> target_)
      : target(Target::Unwrap(target_).target) {}
  };


  v8::Handle<v8::Value>
  Device::IsPresent(const v8::Arguments &args) {
    return AsyncRunner<Device, GetIsPresentData>::Schedule
      (RunGetIsPresent, AfterGetIsPresent, args.This(), args[1], args[0]);
  }


  void
  Device::RunGetIsPresent(Device &instance, GetIsPresentData &data) {
    int result = instance.is_present(data.target);
    data.is_present = !result;
  }


  v8::Handle<v8::Value>
  Device::AfterGetIsPresent(v8::Handle<v8::Object> instance, GetIsPresentData &data) {
    v8::HandleScope scope;
    return scope.Close(toV8(data.is_present));
  }

}
