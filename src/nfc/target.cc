#include "target.hh"


namespace nfc {

  Target::Target(const nfc_target &target_)
    : target(target_)
  {
  }


  std::string
  Target::modulation_type() const {
    switch (target.nm.nmt) {
    case NMT_ISO14443A:
      return "iso14443a";
    case NMT_JEWEL:
      return "jewel";
    case NMT_ISO14443B:
      return "iso14443b";
    case NMT_ISO14443BI:
      return "iso14443bi";
    case NMT_ISO14443B2SR:
      return "iso14443b2sr";
    case NMT_ISO14443B2CT:
      return "iso14443b2ct";
    case NMT_FELICA:
      return "felica";
    case NMT_DEP:
      return "dep";
    }
    return std::string();
  }


  unsigned
  Target::baud_rate() const {
    switch (target.nm.nbr) {
    case NBR_UNDEFINED:
      return 0;
    case NBR_106:
      return 106;
    case NBR_212:
      return 212;
    case NBR_424:
      return 424;
    case NBR_847:
      return 847;
    }
    return 0;
  }


  std::string
  Target::modulation_type_string() const {
    return str_nfc_modulation_type(target.nm.nmt);
  }


  std::string
  Target::baud_rate_string() const {
    return str_nfc_baud_rate(target.nm.nbr);
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

    proto->SetAccessor(v8::String::NewSymbol("modulationType"), GetModulationType);
    proto->SetAccessor(v8::String::NewSymbol("baudRate"), GetBaudRate);
    proto->SetAccessor(v8::String::NewSymbol("info"), GetInfo);

    proto->SetAccessor(v8::String::NewSymbol("modulationTypeString"), GetModulationTypeString);
    proto->SetAccessor(v8::String::NewSymbol("baudRateString"), GetBaudRateString);

    Install("Target", exports, tpl);
  }


  v8::Handle<v8::Value>
  Target::GetModulationType(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::HandleScope scope;
    return scope.Close(toV8(Unwrap(info.This()).modulation_type()));
  }


  v8::Handle<v8::Value>
  Target::GetBaudRate(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::HandleScope scope;
    return scope.Close(toV8(Unwrap(info.This()).baud_rate()));
  }


  v8::Handle<v8::Value>
  Target::GetInfo(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::HandleScope scope;
    const nfc_target &target = Unwrap(info.This()).target;
    v8::Handle<v8::Object> result = v8::Object::New();
    switch (target.nm.nmt) {
    case NMT_ISO14443A:
      result->Set(v8::String::NewSymbol("atqa"),
                  toV8(std::vector<uint8_t>(target.nti.nai.abtAtqa, target.nti.nai.abtAtqa + 2)));
      result->Set(v8::String::NewSymbol("sak"), toV8(target.nti.nai.btSak));
      result->Set(v8::String::NewSymbol("uid"),
                  toV8(std::vector<uint8_t>(target.nti.nai.abtUid, target.nti.nai.abtUid + target.nti.nai.szUidLen)));
      result->Set(v8::String::NewSymbol("ats"),
                  toV8(std::vector<uint8_t>(target.nti.nai.abtAts, target.nti.nai.abtAts + target.nti.nai.szAtsLen)));
      break;
    case NMT_JEWEL:
      result->Set(v8::String::NewSymbol("sensRes"),
                  toV8(std::vector<uint8_t>(target.nti.nji.btSensRes, target.nti.nji.btSensRes + 2)));
      result->Set(v8::String::NewSymbol("id"),
                  toV8(std::vector<uint8_t>(target.nti.nji.btId, target.nti.nji.btId + 4)));
      break;
    case NMT_ISO14443B:
      result->Set(v8::String::NewSymbol("pupi"),
                  toV8(std::vector<uint8_t>(target.nti.nbi.abtPupi, target.nti.nbi.abtPupi + 4)));
      result->Set(v8::String::NewSymbol("applicationData"),
                  toV8(std::vector<uint8_t>(target.nti.nbi.abtApplicationData, target.nti.nbi.abtApplicationData + 4)));
      result->Set(v8::String::NewSymbol("protocolInfo"),
                  toV8(std::vector<uint8_t>(target.nti.nbi.abtProtocolInfo, target.nti.nbi.abtProtocolInfo + 3)));
      result->Set(v8::String::NewSymbol("cardIdentifier"), toV8(target.nti.nbi.ui8CardIdentifier));
      break;
    case NMT_ISO14443BI:
      result->Set(v8::String::NewSymbol("div"),
                  toV8(std::vector<uint8_t>(target.nti.nii.abtDIV, target.nti.nii.abtDIV + 4)));
      result->Set(v8::String::NewSymbol("verLog"), toV8(target.nti.nii.btVerLog));
      result->Set(v8::String::NewSymbol("config"), toV8(target.nti.nii.btConfig));
      result->Set(v8::String::NewSymbol("atr"),
                  toV8(std::vector<uint8_t>(target.nti.nii.abtAtr, target.nti.nii.abtAtr + target.nti.nii.szAtrLen)));
      break;
    case NMT_ISO14443B2SR:
      result->Set(v8::String::NewSymbol("uid"),
                  toV8(std::vector<uint8_t>(target.nti.nsi.abtUID, target.nti.nsi.abtUID + 8)));
      break;
    case NMT_ISO14443B2CT:
      result->Set(v8::String::NewSymbol("uid"),
                  toV8(std::vector<uint8_t>(target.nti.nci.abtUID, target.nti.nci.abtUID + 4)));
      result->Set(v8::String::NewSymbol("prodCode"), toV8(target.nti.nci.btProdCode));
      result->Set(v8::String::NewSymbol("fabCode"), toV8(target.nti.nci.btFabCode));
      break;
    case NMT_FELICA:
      result->Set(v8::String::NewSymbol("len"), toV8(target.nti.nfi.szLen));
      result->Set(v8::String::NewSymbol("resCode"), toV8(target.nti.nfi.btResCode));
      result->Set(v8::String::NewSymbol("id"),
                  toV8(std::vector<uint8_t>(target.nti.nfi.abtId, target.nti.nfi.abtId + 8)));
      result->Set(v8::String::NewSymbol("pad"),
                  toV8(std::vector<uint8_t>(target.nti.nfi.abtPad, target.nti.nfi.abtPad + 8)));
      result->Set(v8::String::NewSymbol("sysCode"),
                  toV8(std::vector<uint8_t>(target.nti.nfi.abtSysCode, target.nti.nfi.abtSysCode + 2)));
      break;
    case NMT_DEP:
      result->Set(v8::String::NewSymbol("nfcid3"),
                  toV8(std::vector<uint8_t>(target.nti.ndi.abtNFCID3, target.nti.ndi.abtNFCID3 + 10)));
      result->Set(v8::String::NewSymbol("did"), toV8(target.nti.ndi.btDID));
      result->Set(v8::String::NewSymbol("bs"), toV8(target.nti.ndi.btBS));
      result->Set(v8::String::NewSymbol("br"), toV8(target.nti.ndi.btBR));
      result->Set(v8::String::NewSymbol("to"), toV8(target.nti.ndi.btTO));
      result->Set(v8::String::NewSymbol("pp"), toV8(target.nti.ndi.btPP));
      result->Set(v8::String::NewSymbol("gb"),
                  toV8(std::vector<uint8_t>(target.nti.ndi.abtGB, target.nti.ndi.abtGB + target.nti.ndi.szGB)));
      {
        v8::Handle<v8::Value> mode;
        switch (target.nti.ndi.ndm) {
        case NDM_UNDEFINED: mode = v8::String::NewSymbol("undefined"); break;
        case NDM_PASSIVE: mode = v8::String::NewSymbol("passive"); break;
        case NDM_ACTIVE: mode = v8::String::NewSymbol("active"); break;
        default: mode = v8::Undefined(); break;
        }
        result->Set(v8::String::NewSymbol("mode"), mode);
      }
      break;
    default:
      return scope.Close(v8::Undefined());
    }
    return scope.Close(result);
  }


  v8::Handle<v8::Value>
  Target::GetModulationTypeString(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::HandleScope scope;
    return scope.Close(toV8(Unwrap(info.This()).modulation_type_string()));
  }


  v8::Handle<v8::Value>
  Target::GetBaudRateString(v8::Local<v8::String> property, const v8::AccessorInfo &info) {
    v8::HandleScope scope;
    return scope.Close(toV8(Unwrap(info.This()).baud_rate_string()));
  }

}
