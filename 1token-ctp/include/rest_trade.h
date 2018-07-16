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

  void GetAccountInfo(const std::string &exchange,
                      const std::string &account_name);

  void GetOrders(const std::string &exchange, const std::string &account_name,
                 const std::string &contract, const std::string &state,
                 const std::vector<RequestOrderInfo> *order_info);

  void InsertOrder(const std::string &exchange, const std::string &account_name,
                   const RequestOrderInfo *order_info);

  void SetUserInterface(UserInterface *user_interface) {
    user_interface_ = user_interface;
  }

  void SetBaseUrl(const std::string &base_url) { base_url_ = base_url; }

 public:
  std::list<std::shared_ptr<std::thread> > tasks_;

 private:
  UserInterface *user_interface_;
  std::string base_url_;
  UserInfo user_info_;
};
}  // namespace onetoken

#define RestTradeHandler Singleton<RestTrade>::GetInstance()