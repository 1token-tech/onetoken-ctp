#pragma once
#include <unordered_map>
#include "rapidjson/document.h"
#include "structs.h"
#include "onetoken_interface.h"
#include "error_code.h"

namespace onetoken {
class Quote {
 public:
  virtual ErrorCode ParseTickData(const rapidjson::Value &data,
                          MarketResponseMessage &message);
  virtual ErrorCode ParseZhubiData(const rapidjson::Value &data,
                                   MarketResponseMessage &message);
  virtual void HandleError(ErrorCode error_code, const std::string &info);
  static std::string UrlEncode(std::string s);

  void SetUserInterface(UserInterface *user_interface) {
    user_interface_ = user_interface;
  }

 protected:
  UserInterface *user_interface_;
  std::unordered_map<std::string, uint32_t> contract_seq_map_;
};
}  // namespace onetoken