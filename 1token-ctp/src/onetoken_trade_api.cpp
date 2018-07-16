#include "trade_client.h"
#include "utils.h"
#include "onetoken_trade_api.h"

namespace onetoken {

OneTokenTradeApi::~OneTokenTradeApi() {}

void OneTokenTradeApi::Init(const std::string &ot_key,
                                const std::string &ot_secret) {
  // TODO: generate authentation key
  TradeHandler->Init(ot_key, ot_secret);
  TradeHandler->SetUserInterface(user_interface_);
  TradeHandler->SetBaseUrl("https://1token.trade/api/v1/trade");
  srand(time(NULL));
}

void OneTokenTradeApi::GetAccountInfo(const std::string &exchange,
                                      const std::string &account_name) {
  TradeHandler->tasks_.emplace_back(std::make_shared<std::thread>(std::bind(
      &TradeClient::GetAccountInfo, TradeHandler, exchange, account_name)));
}

}  // namespace onetoken