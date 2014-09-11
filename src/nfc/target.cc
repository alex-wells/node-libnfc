#include "target.hh"
#include "util.hh"
#include <iomanip>
#include <memory>
#include <sstream>


namespace nfc {

  std::string print_hex(const uint8_t data[], bool padding = false) {
    std::stringstream result;
    for (size_t i = 0; i < sizeof(data) / sizeof(data[0]); ++i) {
      if (padding && i) {
        result << " ";
      }
      result << std::hex << std::setfill('0') << std::setw(2) << unsigned(data[i]);
    }
    return result.str();
  }


  std::ostream &operator<<(std::ostream &out, const Target &target) {
    switch (target->nm.nmt) {
    case NMT_ISO14443A:
      out << "ISO/IEC 14443A (UID: " << print_hex(target->nti.nai.abtUid, true) << ")"; break;
    case NMT_JEWEL:
      out << "Innovision Jewel (ID: " << print_hex(target->nti.nji.btId, true) << ")"; break;
    case NMT_ISO14443B:
      out << "ISO/IEC 14443B"; break;
    case NMT_ISO14443BI:
      out << "ISO/IEC 14443B'"; break;
    case NMT_ISO14443B2SR:
      out << "ISO/IEC 14443-2B ST SRx"; break;
    case NMT_ISO14443B2CT:
      out << "ISO/IEC 14443-2B ASK CTx"; break;
    case NMT_FELICA:
      out << "Felica"; break;
    case NMT_DEP:
      out << "DEP"; break;
    default:
      out << "?"; break;
    }
    out << " (@ ";
    switch (target->nm.nbr) {
    case NBR_UNDEFINED:
      out << "undefined"; break;
    case NBR_106:
      out << "106 kbps"; break;
    case NBR_212:
      out << "212 kbps"; break;
    case NBR_424:
      out << "424 kbps"; break;
    case NBR_847:
      out << "847 kbps"; break;
    default:
      out << "?"; break;
    }
    out << ")";
    return out;
  }


  void
  Target::Initialize() {
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(New);
    tpl->SetClassName(v8::String::NewSymbol("Target"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    v8::Local<v8::ObjectTemplate> proto = tpl->PrototypeTemplate();
    proto->SetAccessor(v8::String::NewSymbol("isPresent"), GetIsPresent);
    proto->SetAccessor(v8::String::NewSymbol("id"), GetId);
    proto->Set(v8::String::NewSymbol("toString"),
               v8::FunctionTemplate::New(ToString)->GetFunction());

    constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());
  }


  v8::Handle<v8::Object>
  Target::Create(v8::Handle<v8::Object> device, nfc_target target) {
    std::auto_ptr<nfc_target> target_ptr(new nfc_target(target));
    v8::HandleScope scope;
    const int argc = 2;
    v8::Handle<v8::Value> argv[argc] = {
      device,
      v8::External::Wrap(target_ptr.get()),
    };
    return scope.Close(constructor->NewInstance(argc, argv));
  }


  Target &
  Target::Unwrap(v8::Handle<v8::Value> value) {
    // Cannot throw exceptions here, so an assertion is the best
    // we can do (during development).
    assert(value->IsObject());
    return *node::ObjectWrap::Unwrap<Target>(value.As<v8::Object>());
  }


  Target::Target(v8::Handle<v8::Object> device, nfc_target target_)
    : target(target_)
  {
    v8::HandleScope scope;
    device_handle = v8::Persistent<v8::Object>::New(device);
  }


  bool
  Target::is_present() {
    Device &device = this->device();
    RdLock lock(device.device_lock);
    return device.device && nfc_initiator_target_is_present(device.device, &target) == 0;
  }


  std::string
  Target::id() const {
    switch (target.nm.nmt) {
    case NMT_ISO14443A:
      return print_hex(target.nti.nai.abtUid);
    case NMT_JEWEL:
      return print_hex(target.nti.nji.btId);
    default:
      return "";
    }
  }


  const nfc_target *
  Target::operator->() const {
    return &target;
  }


  Device &
  Target::device() {
    return Device::Unwrap(device_handle);
  }


  v8::Handle<v8::Value>
  Target::New(const v8::Arguments &args) {
    v8::HandleScope scope;

    if (args.IsConstructCall()) {
      nfc_target *target = static_cast<nfc_target *>(v8::External::Unwrap(args[1]));
      Target *obj = new Target(args[0].As<v8::Object>(), *target);
      obj->Wrap(args.This());
      return args.This();
    }
    else {
      const int argc = 2;
      v8::Handle<v8::Value> argv[argc] = {args[0], args[1]};
      return scope.Close(constructor->NewInstance(argc, argv));
    }
  }


  v8::Handle<v8::Value>
  Target::GetIsPresent(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::HandleScope scope;
    Target &obj = Unwrap(info.This());
    return scope.Close(v8::Boolean::New(obj.is_present()));
  }


  v8::Handle<v8::Value>
  Target::GetId(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::HandleScope scope;
    Target &obj = Unwrap(info.This());
    std::string id = obj.id();
    if (id.empty()) {
      v8::ThrowException(v8::Exception::Error(v8::String::New("no id found")));
      return scope.Close(v8::Undefined());
    }
    else {
      return scope.Close(v8::String::New(id.data(), id.length()));
    }
  }


  v8::Handle<v8::Value>
  Target::ToString(const v8::Arguments &args) {
    v8::HandleScope scope;
    Target &obj = Unwrap(args.This());
    std::stringstream result;
    result << obj;
    std::string result_str(result.str());
    return scope.Close(v8::String::New(result_str.data(), result_str.length()));
  }


  v8::Persistent<v8::Function> Target::constructor;

}
