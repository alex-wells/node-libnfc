#include "nfc/context.hh"
#include "nfc/device.hh"
#include "nfc/target.hh"
#include <node.h>


void
Initialize(v8::Handle<v8::Object> exports) {
  nfc::Context::Initialize(exports);
  nfc::Device::Initialize(exports);
  nfc::Target::Initialize(exports);
}


NODE_MODULE(nfc, Initialize)
