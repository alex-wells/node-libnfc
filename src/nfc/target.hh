#ifndef NFC_TARGET_HH
#define NFC_TARGET_HH

#include "context.hh"
#include "device.hh"
#include <node.h>


namespace nfc {

  class Target:
    public node::ObjectWrap
  {
  public:
    static void Initialize();

    static v8::Handle<v8::Object> Create(v8::Handle<v8::Object> device, nfc_target target);

  public:
    static Target &Unwrap(v8::Handle<v8::Value> value);

    explicit Target(v8::Handle<v8::Object> device, nfc_target target);

    Device &device();

    bool is_present();

    std::string id() const;

    const nfc_target *operator->() const;

  protected:
    v8::Persistent<v8::Object> device_handle;
    const nfc_target target;

  private:
    static v8::Handle<v8::Value> New(const v8::Arguments &args);

    static v8::Handle<v8::Value> GetIsPresent(v8::Local<v8::String> property, const v8::AccessorInfo &info);
    static v8::Handle<v8::Value> GetId(v8::Local<v8::String> property, const v8::AccessorInfo &info);

    static v8::Handle<v8::Value> ToString(const v8::Arguments &args);

    static v8::Persistent<v8::Function> constructor;
  };

}

#endif
