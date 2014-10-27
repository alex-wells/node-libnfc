#ifndef NFC_TARGET_HH
#define NFC_TARGET_HH

#include "util.hh"
#include <nfc/nfc.h>


namespace nfc {

  class Target:
    public nfc::ObjectWrap<Target>
  {
    friend class Device;

  public:
    nfc_target target;

  public:
    Target(const nfc_target &target);

    std::string modulation_type() const;
    unsigned baud_rate() const;

    std::string modulation_type_string() const;
    std::string baud_rate_string() const;

    std::string info_string(bool verbose) const;

  public:
    static v8::Handle<v8::Value> Construct(const nfc_target &target);

  public:
    static v8::Persistent<v8::Function> constructor;

    static Target *Create(const v8::Arguments &args);
    static void Initialize(v8::Handle<v8::Object> exports);

    static v8::Handle<v8::Value> GetModulationType(v8::Local<v8::String> property, const v8::AccessorInfo &info);
    static v8::Handle<v8::Value> GetBaudRate(v8::Local<v8::String> property, const v8::AccessorInfo &info);
    static v8::Handle<v8::Value> GetInfo(v8::Local<v8::String> property, const v8::AccessorInfo &info);

    static v8::Handle<v8::Value> GetModulationTypeString(v8::Local<v8::String> property, const v8::AccessorInfo &info);
    static v8::Handle<v8::Value> GetBaudRateString(v8::Local<v8::String> property, const v8::AccessorInfo &info);
    static v8::Handle<v8::Value> GetInfoString(v8::Local<v8::String> property, const v8::AccessorInfo &info);
  };

}

#endif
