#pragma once
#include "onetoken_interface.h"

namespace onetoken {
class CLASSINDLL_CLASS_DECL OneTokenMarketApi {
 public:
  OneTokenMarketApi(UserInterface *user_interface)
      : user_interface_(user_interface) {}
  ~OneTokenMarketApi();

  void WSInit(bool enable_gzip);
  void WSTickLogin();
  void WSClose();
  void SubscribeTickData(const std::vector<std::string> &contracts);
  void SubscribeTradeData(const std::vector<std::string> &contracts);
  void SubscribeMarketData(const std::vector<std::string> &contracts);
  void UnsubscribeTickData(const std::vector<std::string> &contracts);
  void UnsubscribeTradeData(const std::vector<std::string> &contracts);
  void UnsubscribeMarketData(const std::vector<std::string> &contracts);

  void RESTInit();
  void GetTicks(const std::string &exchange);
  void GetSingleTick(const std::string &exchange, const std::string &contract);
  void GetZhubiList(const std::string &contract, const std::string &since,
                    const std::string &until, uint32_t size);
  void Join();

  UserInterface *user_interface_;
};
}  // namespace onetoken