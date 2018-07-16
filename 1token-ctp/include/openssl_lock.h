#pragma once
#include <vector>
#include <mutex>
#include "openssl/crypto.h"

static std::shared_ptr<std::vector<std::mutex> > gLocks;  // สýื้

class SSLLock {
 public:
  static unsigned long ThreadIdCallback() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    uint64_t id = std::stoull(ss.str());
    return id;
  }
  static void LockingCallback(int mode, int type, const char *file,
                                 int line) {
    if (mode & CRYPTO_LOCK) {
      (*gLocks)[type].lock();
    } else {
      (*gLocks)[type].unlock();
    }
  }
  static void ThreadSafetySetup() {
    gLocks.reset(new std::vector<std::mutex>(CRYPTO_num_locks()));
    CRYPTO_set_id_callback(SSLLock::ThreadIdCallback);
    CRYPTO_set_locking_callback(SSLLock::LockingCallback);
  }
  static void ThreadSafetyCleanup() {
    CRYPTO_set_id_callback(nullptr);
    CRYPTO_set_locking_callback(nullptr);
    gLocks.reset();
  }
};