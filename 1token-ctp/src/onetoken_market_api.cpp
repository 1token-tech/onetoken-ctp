#include <unordered_map>
#include <time.h>
#include "onetoken_market_api.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rest_quote.h"
#include "ws_quote.h"

namespace onetoken {

OneTokenMarketApi::~OneTokenMarketApi() {}

void OneTokenMarketApi::WSInit(bool enable_gzip) {
  WSQuoteHandler->SetUserInterface(user_interface_);
  WSQuoteHandler->SetEnableGzip(enable_gzip);

  std::string url = "wss://1token.trade/api/v1/ws/tick";
  WSQuoteHandler->Connect(WSTYPE_TICK, url);

  url = "wss://1token.trade/api/v1/ws/candle";
  WSQuoteHandler->Connect(WSTYPE_CANDLE, url);
}

void OneTokenMarketApi::WSLogin() {
  rapidjson::Document document;
  auto &allocator = document.GetAllocator();
  rapidjson::Value root(rapidjson::kObjectType);

  root.AddMember("uri", "auth", allocator);
  root.AddMember("sample-rate", 0, allocator);
  rapidjson::StringBuffer buf;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
  root.Accept(writer);
  WSQuoteHandler->Send(WSTYPE_TICK, buf.GetString());
  WSQuoteHandler->Send(WSTYPE_CANDLE, buf.GetString());
}

void OneTokenMarketApi::RESTInit() {
  // TODO: generate authentation key
  RESTQuoteHandler->SetUserInterface(user_interface_);
  RESTQuoteHandler->SetBaseUrl("https://1token.trade/api/v1");
  //MessageHeader header;
  //user_interface_->OnInit(&header);
}

void OneTokenMarketApi::GetTicks(const std::string &exchange) {
  std::string uri = "/quote/ticks?exchange=";
  uri += Quote::UrlEncode(exchange);
  RESTQuoteHandler->SendRequest(onetoken::RESTTYPE_TICKS, uri);
}

void OneTokenMarketApi::GetSingleTick(const std::string &exchange,
                                      const std::string &contract) {
  std::string uri = "/quote/single-tick/";
  uri += Quote::UrlEncode(exchange);
  uri += "/";
  uri += Quote::UrlEncode(contract);
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
  uri += Quote::UrlEncode(contract);
  uri += "&since=";
  uri += Quote::UrlEncode(since);
  uri += "&until=";
  uri += Quote::UrlEncode(until);
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
    std::cout << "sub tick data for " << contract << ": " << buf.GetString()
              << std::endl;
    WSQuoteHandler->Send(WSTYPE_TICK, buf.GetString());
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
  std::cout << "sub tick data: " << buf.GetString() << std::endl;
  WSQuoteHandler->Send(WSTYPE_TICK, buf.GetString());
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
    std::cout << "sub trade data for " << contract << ": " << buf.GetString()
              << std::endl;
    WSQuoteHandler->Send(WSTYPE_TICK, buf.GetString());
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
  std::cout << "sub trade data: " << buf.GetString() << std::endl;
  WSQuoteHandler->Send(WSTYPE_TICK, buf.GetString());
}
}  // namespace onetoken