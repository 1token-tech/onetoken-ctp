#include "onetoken_trade_api.h"
#include "rest_quote.h"
#include "utils.h"

namespace onetoken {

OneTokenTradeApi::~OneTokenTradeApi() {}

void OneTokenTradeApi::RESTInit() {
  // TODO: generate authentation key
  RESTQuoteHandler->SetUserInterface(user_interface_);
  RESTQuoteHandler->SetBaseUrl("https://1token.trade/api/v1/trade");
  std::string encrypted = utils::BinaryToHex(utils::HmacSha256Encode(
      "abcde",
      "POST/huobip/zannb/orders1531299706129025{\"contract\": "
      "\"huobip/btc.usdt\", \"price\": 1, \"bs\": \"b\", \"amount\": 0.6}"));
  std::cout << encrypted << std::endl;
}

}  // namespace onetoken