#include "target.hh"


namespace nfc {

  Target::Target(const nfc_target &target_)
    : target(target_)
  {
  }


  v8::Handle<v8::Value>
  Target::Construct(const nfc_target &target) {
    return ObjectWrap::Construct(target);
  }


  Target *
  Target::Create(const v8::Arguments &args) {
    return ObjectWrap::Create<nfc_target>(args);
  }


  v8::Persistent<v8::Function> Target::constructor;


  void
  Target::Initialize(v8::Handle<v8::Object> exports) {
    v8::HandleScope scope;
    v8::Local<v8::ObjectTemplate> proto;
    v8::Local<v8::FunctionTemplate> tpl;
    Prepare(tpl, proto);

    Install("Target", exports, tpl);
  }

}
