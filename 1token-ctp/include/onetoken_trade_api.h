#pragma once
#include "utils.h"
#include "onetoken_interface.h"
#include "error_code.h"

namespace onetoken {
class CLASSINDLL_CLASS_DECL OneTokenTradeApi {
 public:
  OneTokenTradeApi(UserInterface *user_interface)
      : user_interface_(user_interface) {}

  void Init(const std::string &ot_key, const std::string &ot_secret);
  ErrorCode GetAccountInfo(const TradeBaseInfo &base_info);

  // max 9 orders, does not support both client oid and exchange oid
  //if provide more than 9 orders, only the first 9 orders will be processed.
  ErrorCode GetOrders(const TradeBaseInfo &base_info);
  ErrorCode GetOrders(const TradeBaseInfo &base_info,
                 const std::vector<RequestOrderInfo> &order_info);

  ErrorCode PlaceOrder(const TradeBaseInfo &base_info,
                   const RequestOrderInfo &order_info);

  //max 9 orders, does not support both client oid and exchange oid
  //if provide more than 9 orders, only the first 9 orders will be processed.
  ErrorCode CancelOrder(const TradeBaseInfo &base_info,
                   const std::vector<RequestOrderInfo> &order_info);

  ErrorCode CancelAllOrders(const TradeBaseInfo &base_info);

  ErrorCode GetTrans(const TradeBaseInfo &base_info);
  void Join();

  //TODO: check order status

  ~OneTokenTradeApi();

 private:
  UserInterface *user_interface_;
};
}  // namespace onetoken
