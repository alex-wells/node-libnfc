#ifndef NFC_UTIL_HH
#define NFC_UTIL_HH

#include <uv.h>


namespace nfc {

  class RdLock {
  public:
    RdLock(uv_rwlock_t &lock);
    ~RdLock();

  private:
    RdLock(const RdLock &other);
    RdLock &operator=(const RdLock &other);

  private:
    uv_rwlock_t &lock;
  };


  class WrLock {
  public:
    WrLock(uv_rwlock_t &lock);
    ~WrLock();

  private:
    WrLock(const WrLock &other);
    WrLock &operator=(const WrLock &other);

  private:
    uv_rwlock_t &lock;
  };

}

#endif
