#ifndef NFC_TARGET_HH
#define NFC_TARGET_HH

#include "util.hh"
#include <nfc/nfc.h>


namespace nfc {

  class Target:
    public nfc::ObjectWrap<Target>
  {
    friend class Device;

  protected:
    nfc_target target;

  public:
    Target(const nfc_target &target);

  public:
    static v8::Handle<v8::Value> Construct(const nfc_target &target);

  public:
    static v8::Persistent<v8::Function> constructor;

    static Target *Create(const v8::Arguments &args);
    static void Initialize(v8::Handle<v8::Object> exports);
  };

}

#endif
