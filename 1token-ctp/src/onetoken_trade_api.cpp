#include "utils.h"
#include "onetoken_trade_api.h"
#include "rest_trade.h"

namespace onetoken {

OneTokenTradeApi::~OneTokenTradeApi() {}

void OneTokenTradeApi::Init(const std::string &ot_key,
                            const std::string &ot_secret) {
  RestTradeHandler->Init(ot_key, ot_secret);
  RestTradeHandler->SetUserInterface(user_interface_);
  RestTradeHandler->SetBaseUrl("https://1token.trade/api/v1/trade");
}

ErrorCode OneTokenTradeApi::GetAccountInfo(const TradeBaseInfo &base_info) {
  RestTradeHandler->thread_pool().AddTask(std::bind(
      &RestTrade::GetAccountInfo, RestTradeHandler, std::ref(base_info)));
  return SUCCESS;
}

ErrorCode OneTokenTradeApi::GetOrders(const TradeBaseInfo &base_info) {
  static std::vector<RequestOrderInfo> reserved;
  return GetOrders(base_info, reserved);
}

ErrorCode OneTokenTradeApi::GetOrders(
    const TradeBaseInfo &base_info,
    const std::vector<RequestOrderInfo> &order_info) {
  bool has_client_oid = false;
  bool has_exchange_oid = false;
  for (auto const &order : order_info) {
    if (!order.client_oid.empty()) {
      if (has_exchange_oid) {
        return WRONG_PARAMETER;
      }
      if (!has_client_oid) {
        has_client_oid = true;
      }
    }
    if (!order.exchange_oid.empty()) {
      if (has_client_oid) {
        return WRONG_PARAMETER;
      }
      if (!has_exchange_oid) {
        has_exchange_oid = true;
      }
    }
  }

  if (order_info.size() > 9) {
    return WRONG_PARAMETER;
  }

  RestTradeHandler->thread_pool().AddTask(
      std::bind(&RestTrade::GetOrders, RestTradeHandler, std::ref(base_info),
                std::ref(order_info), has_client_oid));
  return SUCCESS;
}

ErrorCode OneTokenTradeApi::PlaceOrder(const TradeBaseInfo &base_info,
                                       const RequestOrderInfo &order_info) {
  RestTradeHandler->thread_pool().AddTask(
      std::bind(&RestTrade::PlaceOrder, RestTradeHandler, std::ref(base_info),
                std::ref(order_info)));
  return SUCCESS;
}

ErrorCode OneTokenTradeApi::CancelOrder(
    const TradeBaseInfo &base_info,
    const std::vector<RequestOrderInfo> &order_info) {
  if (order_info.empty()) {
    return WRONG_PARAMETER;
  }
  bool has_client_oid = false;
  bool has_exchange_oid = false;
  for (auto const &order : order_info) {
    if (!order.client_oid.empty()) {
      if (has_exchange_oid) {
        return WRONG_PARAMETER;
      }
      if (!has_client_oid) {
        has_client_oid = true;
      }
    }
    if (!order.exchange_oid.empty()) {
      if (has_client_oid) {
        return WRONG_PARAMETER;
      }
      if (!has_exchange_oid) {
        has_exchange_oid = true;
      }
    }
  }

  if ((!has_client_oid && !has_exchange_oid) || order_info.empty() || order_info.size() > 9) {
    return WRONG_PARAMETER;
  }

  RestTradeHandler->thread_pool().AddTask(
      std::bind(&RestTrade::CancelOrder, RestTradeHandler, std::ref(base_info),
                std::ref(order_info), has_client_oid));
  return SUCCESS;
}

ErrorCode OneTokenTradeApi::CancelAllOrders(const TradeBaseInfo &base_info) {
  RestTradeHandler->thread_pool().AddTask(std::bind(
      &RestTrade::CancelAllOrders, RestTradeHandler, std::ref(base_info)));
  return SUCCESS;
}

ErrorCode OneTokenTradeApi::GetTrans(const TradeBaseInfo &base_info) {
  RestTradeHandler->thread_pool().AddTask(std::bind(
      &RestTrade::GetTrans, RestTradeHandler, std::ref(base_info)));
  return SUCCESS;
}

void OneTokenTradeApi::Join() { RestTradeHandler->thread_pool().Join(); }
}  // namespace onetoken