#include <time.h>
#include <unordered_map>
#include "onetoken_market_api.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rest_quote.h"
#include "utils.h"
#include "ws_quote_tick.h"
#include "ws_quote_candle.h"

namespace onetoken {

OneTokenMarketApi::~OneTokenMarketApi() {}

void OneTokenMarketApi::WSInit(bool enable_gzip) {
  std::string url = "wss://1token.trade/api/v1/ws/tick";
  WSQuoteTickHandler->SetUserInterface(user_interface_);
  WSQuoteTickHandler->SetEnableGzip(enable_gzip);
  WSQuoteTickHandler->Connect(url);

  url = "wss://1token.trade/api/v1/ws/candle";
  WSQuoteCandleHandler->SetUserInterface(user_interface_);
  WSQuoteCandleHandler->SetEnableGzip(enable_gzip);
  WSQuoteCandleHandler->Connect(url);
}

void OneTokenMarketApi::WSTickLogin() {
  rapidjson::Document document;
  auto &allocator = document.GetAllocator();
  rapidjson::Value root(rapidjson::kObjectType);

  root.AddMember("uri", "auth", allocator);
  root.AddMember("sample-rate", 0, allocator);
  rapidjson::StringBuffer buf;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
  root.Accept(writer);

  WSQuoteTickHandler->Send(buf.GetString());
}

void OneTokenMarketApi::RESTInit() {
  // TODO: generate authentation key
  RESTQuoteHandler->SetUserInterface(user_interface_);
  RESTQuoteHandler->SetBaseUrl("https://1token.trade/api/v1");
  std::string encrypted = utils::BinaryToHex(utils::HmacSha256Encode("abcde", "POST/huobip/zannb/orders1531299706129025{\"contract\": \"huobip/btc.usdt\", \"price\": 1, \"bs\": \"b\", \"amount\": 0.6}"));
  std::cout << encrypted << std::endl;
  // MessageHeader header;
  // user_interface_->OnInit(&header);
}

void OneTokenMarketApi::GetTicks(const std::string &exchange) {
  std::string uri = "/quote/ticks?exchange=";
  uri += utils::UrlEncode(exchange);
  RESTQuoteHandler->SendRequest(onetoken::RESTTYPE_TICKS, uri);
}

void OneTokenMarketApi::GetSingleTick(const std::string &exchange,
                                      const std::string &contract) {
  std::string uri = "/quote/single-tick/";
  uri += utils::UrlEncode(exchange);
  uri += "/";
  uri += utils::UrlEncode(contract);
  RESTQuoteHandler->SendRequest(onetoken::RESTTYPE_SINGLE_TICK, uri);
}

void OneTokenMarketApi::GetZhubiList(const std::string &contract,
                                     const std::string &since,
                                     const std::string &until, uint32_t size) {
  if (size > 2000) {
    printf("size must <= 2000");
    return;
  }
  std::string uri = "/quote/zhubi?contract=";
  uri += utils::UrlEncode(contract);
  uri += "&since=";
  uri += utils::UrlEncode(since);
  uri += "&until=";
  uri += utils::UrlEncode(until);
  uri += "&size=";
  uri += std::to_string(size);

  RESTQuoteHandler->SendRequest(onetoken::RESTTYPE_ZHUBI, uri);
}

void OneTokenMarketApi::SubscribeTickData(
    const std::vector<std::string> &contracts) {
  rapidjson::Document document;
  rapidjson::StringBuffer buf;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
  auto &allocator = document.GetAllocator();
  rapidjson::Value root(rapidjson::kObjectType);

  root.AddMember("uri", "subscribe-single-tick-verbose", allocator);

  bool first = true;
  for (auto const &contract : contracts) {
    if (first) {
      first = false;
      root.AddMember("contract", contract, allocator);
    } else {
      root["contract"].SetString(contract, allocator);
    }
    buf.Clear();
    root.Accept(writer);
    WSQuoteTickHandler->Send(buf.GetString());
  }
}

void OneTokenMarketApi::SubscribeTickData(const std::string &contract) {
  rapidjson::Document document;
  auto &allocator = document.GetAllocator();
  rapidjson::Value root(rapidjson::kObjectType);
  root.AddMember("uri", "subscribe-single-tick-verbose", allocator);
  root.AddMember("contract", contract, allocator);
  rapidjson::StringBuffer buf;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
  root.Accept(writer);
  WSQuoteTickHandler->Send(buf.GetString());
}

void OneTokenMarketApi::SubscribeTradeData(
    const std::vector<std::string> &contracts) {
  rapidjson::Document document;
  rapidjson::StringBuffer buf;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
  auto &allocator = document.GetAllocator();
  rapidjson::Value root(rapidjson::kObjectType);

  root.AddMember("uri", "subscribe-single-zhubi-verbose", allocator);

  bool first = true;
  for (auto const &contract : contracts) {
    if (first) {
      first = false;
      root.AddMember("contract", contract, allocator);
    } else {
      root["contract"].SetString(contract, allocator);
    }
    buf.Clear();
    root.Accept(writer);
    WSQuoteTickHandler->Send(buf.GetString());
  }
}

void OneTokenMarketApi::SubscribeTradeData(const std::string &contract) {
  rapidjson::Document document;
  auto &allocator = document.GetAllocator();
  rapidjson::Value root(rapidjson::kObjectType);
  root.AddMember("uri", "subscribe-single-zhubi-verbose", allocator);
  root.AddMember("contract", contract, allocator);
  rapidjson::StringBuffer buf;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
  root.Accept(writer);
  WSQuoteTickHandler->Send(buf.GetString());
}
}  // namespace onetoken