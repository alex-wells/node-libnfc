#ifndef NFC_CONTEXT_HH
#define NFC_CONTEXT_HH

#include "util.hh"
#include <nfc/nfc.h>
#include <string>
#include <vector>


namespace nfc {

  class RawContext:
    public RawObject<RawContext, nfc_context>
  {
  public:
    static RawContext initialize();

    RawContext(nfc_context *context = NULL);

    static void destroy(nfc_context *context);
  };


  class RawDevice;

  class Context:
    public nfc::ObjectWrap<Context>
  {
  protected:
    RawContext context;

  public:
    Context();

    static std::string version();
    std::vector<std::string> devices();

    RawDevice open(const std::string &connstring = "");

  public:
    static v8::Persistent<v8::Function> constructor;

    static void Initialize(v8::Handle<v8::Object> exports);
    static v8::Handle<v8::Value> CheckNew(v8::Handle<v8::Value> instance);

    static v8::Handle<v8::Value> GetVersion(v8::Local<v8::String> property, const v8::AccessorInfo &info);
    static v8::Handle<v8::Value> GetDevices(v8::Local<v8::String> property, const v8::AccessorInfo &info);

    static v8::Handle<v8::Value> Open(const v8::Arguments &args);

  protected:
    struct OpenData;
    static void RunOpen(Context &instance, OpenData &data);
    static v8::Handle<v8::Value> AfterOpen(v8::Handle<v8::Object> instance, OpenData &data);
  };

}

#endif
