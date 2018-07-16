#include "utils.h"
#include "onetoken_trade_api.h"
#include "rest_trade.h"

namespace onetoken {

OneTokenTradeApi::~OneTokenTradeApi() {}

void OneTokenTradeApi::Init(const std::string &ot_key,
                            const std::string &ot_secret) {
  // TODO: generate authentation key
  RestTradeHandler->Init(ot_key, ot_secret);
  RestTradeHandler->SetUserInterface(user_interface_);
  RestTradeHandler->SetBaseUrl("https://1token.trade/api/v1/trade");
}

void OneTokenTradeApi::GetAccountInfo(const std::string &exchange,
                                      const std::string &account_name) {
  RestTradeHandler->tasks_.emplace_back(std::make_shared<std::thread>(std::bind(
      &RestTrade::GetAccountInfo, RestTradeHandler, exchange, account_name)));
}

void OneTokenTradeApi::GetOrders(
    const std::string &exchange, const std::string &account_name,
    const std::string &contract, const std::string &state,
    const std::vector<RequestOrderInfo> *order_info) {
  RestTradeHandler->tasks_.emplace_back(std::make_shared<std::thread>(std::bind(
      &RestTrade::GetOrders, RestTradeHandler, 
      exchange, account_name, contract, state, order_info)));
}

void OneTokenTradeApi::InsertOrder(const std::string &exchange,
                                   const std::string &account_name,
                                   const RequestOrderInfo *order_info) {
  RestTradeHandler->tasks_.emplace_back(std::make_shared<std::thread>(
      std::bind(&RestTrade::InsertOrder, RestTradeHandler, exchange,
                account_name, order_info)));
}

}  // namespace onetoken