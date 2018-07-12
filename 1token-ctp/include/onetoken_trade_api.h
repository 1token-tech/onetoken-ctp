#pragma once

#include "onetoken_interface.h"

namespace onetoken {
class OneTokenTradeApi {
 public:
  OneTokenTradeApi(UserInterface *user_interface)
      : user_interface_(user_interface) {}

  void RESTInit();
  void RESTLogin(const std::string &ot_key, const std::string &ot_secret);
  ~OneTokenTradeApi();
 private:
  UserInterface *user_interface_;
};
}  // namespace onetoken