#pragma once
#include "onetoken_interface.h"

namespace onetoken {
class CLASSINDLL_CLASS_DECL OneTokenTradeApi {
 public:
  OneTokenTradeApi(UserInterface *user_interface)
      : user_interface_(user_interface) {}

  void Init(const std::string &ot_key, const std::string &ot_secret);
  void GetAccountInfo(const std::string &exchange, const std::string &account_name);
  ~OneTokenTradeApi();
 private:
  UserInterface *user_interface_;
};
}  // namespace onetoken