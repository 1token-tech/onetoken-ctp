#include "onetoken_trade_api.h"
#include "rest_quote.h"
#include "utils.h"

namespace onetoken {

OneTokenTradeApi::~OneTokenTradeApi() {}

void OneTokenTradeApi::RESTInit(const std::string &ot_key,
                                const std::string &ot_secret) {
  // TODO: generate authentation key
  RESTQuoteHandler->SetUserInterface(user_interface_);
  RESTQuoteHandler->SetBaseUrl("https://1token.trade/api/v1/trade");
  srand(time(NULL));
}

void OneTokenTradeApi::GetAccountInfo() {
}

}  // namespace onetoken