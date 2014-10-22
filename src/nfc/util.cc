#include "util.hh"


namespace nfc {

  Lock::Lock() {
    uv_rwlock_init(&lock);
  }


  Lock::~Lock() {
    uv_rwlock_destroy(&lock);
  }


  RdLock::RdLock(const Lock &lock_)
    : lock(lock_.lock)
  {
    uv_rwlock_rdlock(&lock);
  }


  RdLock::~RdLock() {
    uv_rwlock_rdunlock(&lock);
  }


  WrLock::WrLock(const Lock &lock_)
    : lock(lock_.lock)
  {
    uv_rwlock_wrlock(&lock);
  }


  WrLock::~WrLock() {
    uv_rwlock_wrunlock(&lock);
  }

}
