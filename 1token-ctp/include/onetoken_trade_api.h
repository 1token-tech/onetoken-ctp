#pragma once
#include "onetoken_interface.h"

namespace onetoken {
class CLASSINDLL_CLASS_DECL OneTokenTradeApi {
 public:
  OneTokenTradeApi(UserInterface *user_interface)
      : user_interface_(user_interface) {}

  void Init(const std::string &ot_key, const std::string &ot_secret);
  void GetAccountInfo(const std::string &exchange,
                      const std::string &account_name);
  void GetOrders(const std::string &exchange, const std::string &account_name,
                 const std::string &contract, const std::string &state,
                 const std::vector<RequestOrderInfo> *order_info);
  void InsertOrder(const std::string &exchange, const std::string &account_name,
                   const RequestOrderInfo *order_info);
  void DeleteOrder(const std::string &exchange, const std::string &account_name,
                   const std::vector<RequestOrderInfo> *order_info);
  void DeleteAllOrders(const std::string &exchange, const std::string &account_name,
                   const RequestOrderInfo *order_info);

  //TODO: check order status

  ~OneTokenTradeApi();

 private:
  UserInterface *user_interface_;
};
}  // namespace onetoken