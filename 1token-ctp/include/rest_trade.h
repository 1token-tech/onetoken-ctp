#pragma once
#include <list>
#include <thread>
#include "cpprest/http_client.h"
#include "error_code.h"
#include "onetoken_interface.h"
#include "singleton.h"

namespace onetoken {

struct UserInfo {
  std::string ot_key;
  std::string ot_secret;
};
class RestTrade {
 public:
  RestTrade() {}

  void Init(const std::string &ot_key, const std::string &ot_secret);
  virtual void HandleError(ErrorCode error_code, ReqType type,
                           const std::string &info);

  void GetAccountInfo(const TradeBaseInfo &base_info);

  void GetOrders(const TradeBaseInfo &base_info,
                 const std::vector<RequestOrderInfo> &order_info,
                 bool is_client_oid);

  void PlaceOrder(const TradeBaseInfo &base_info,
                   const RequestOrderInfo &order_info);

  void CancelOrder(const TradeBaseInfo &base_info,
                   const std::vector<RequestOrderInfo> &order_info,
                   bool is_client_oid);

  // Currently not usable
  void CancelAllOrders(const TradeBaseInfo &base_info);

  void ParseResponse(ReqType type, const TradeBaseInfo &base_info, web::http::http_response response);

  void SetUserInterface(UserInterface *user_interface) {
    user_interface_ = user_interface;
  }

  void SetBaseUrl(const std::string &base_url) { base_url_ = base_url; }

 public:
  std::list<std::shared_ptr<std::thread>> tasks_;

 private:
  UserInterface *user_interface_;
  std::string base_url_;
  UserInfo user_info_;
};
}  // namespace onetoken

#define RestTradeHandler Singleton<RestTrade>::GetInstance()