#include "nfc/context.hh"
#include "nfc/device.hh"
#include "nfc/target.hh"
#include <node.h>
#include <v8.h>


v8::Handle<v8::Value>
Open(const v8::Arguments &args) {
  v8::HandleScope scope;

  v8::Handle<v8::Value> connstring = v8::Null();
  if (args[0]->BooleanValue()) {
    connstring = args[0]->ToString();
  }

  return scope.Close(nfc::InitiatorDevice::Create(connstring));
}


v8::Handle<v8::Value>
GetVersion(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
  v8::HandleScope scope;
  return scope.Close(v8::String::New(nfc_version()));
}


void
Initialize(v8::Handle<v8::Object> exports) {
  nfc_init(&nfc::context);

  if (nfc::context) {
    nfc::InitiatorDevice::Initialize();
    nfc::Target::Initialize();

    exports->SetAccessor(v8::String::NewSymbol("version"), GetVersion);
    exports->Set(v8::String::NewSymbol("open"), v8::FunctionTemplate::New(Open)->GetFunction());
  }
  else {
    v8::ThrowException(v8::Exception::TypeError(v8::String::New("unable to initialize libnfc")));
  }
}


NODE_MODULE(nfc, Initialize)
