#ifndef NFC_TYPE_TRAITS_HH
#define NFC_TYPE_TRAITS_HH

#include <stdint.h>


namespace nfc {

  template<typename T, T v>
  struct integral_constant {
    typedef T value_type;
    static const T value = v;
  };


  struct true_type: integral_constant<bool, true> {};
  struct false_type: integral_constant<bool, false> {};


  template<typename T> struct is_signed: false_type {};
  template<> struct is_signed<int8_t>: true_type {};
  template<> struct is_signed<int16_t>: true_type {};
  template<> struct is_signed<int32_t>: true_type {};
  template<> struct is_signed<int64_t>: true_type {};


  template<typename T> struct is_unsigned: false_type {};
  template<> struct is_unsigned<uint8_t>: true_type {};
  template<> struct is_unsigned<uint16_t>: true_type {};
  template<> struct is_unsigned<uint32_t>: true_type {};
  template<> struct is_unsigned<uint64_t>: true_type {};


  template<bool B, class T = void> struct enable_if {};
  template<class T> struct enable_if<true, T> { typedef T type; };

}

#endif
