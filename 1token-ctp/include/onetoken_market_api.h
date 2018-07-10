#pragma once
#include "onetoken_interface.h"

namespace onetoken {
class OneTokenMarketApi {
 public:
  OneTokenMarketApi(UserInterface *user_interface)
      : user_interface_(user_interface) {}
  ~OneTokenMarketApi();

  void WSInit(bool enable_gzip);
  void WSLogin();
  void RESTInit();
  void SubscribeTickData(const std::string &contract);
  void SubscribeTickData(const std::vector<std::string> &contracts);
  void SubscribeTradeData(const std::string &contract);
  void SubscribeTradeData(const std::vector<std::string> &contracts);

  void GetTicks(const std::string &exchange);
  void GetSingleTick(const std::string &exchange, const std::string &contract);
  void GetZhubiList(const std::string &contract, const std::string &since,
                    const std::string &until, uint32_t size);

  UserInterface *user_interface_;
};
}  // namespace onetoken