#include <time.h>
#include <unordered_map>
#include "cpprest/http_client.h"
#include "onetoken_market_api.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rest_quote.h"
#include "utils.h"
#include "ws_quote_candle.h"
#include "ws_quote_tick.h"

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

void OneTokenMarketApi::WSClose() {
  WSQuoteTickHandler->Close();
  WSQuoteCandleHandler->Close();
}

void OneTokenMarketApi::RESTInit() {
  RESTQuoteHandler->SetUserInterface(user_interface_);
  RESTQuoteHandler->SetBaseUrl("https://1token.trade/api/v1/quote");
  RESTQuoteHandler->Init();
}

void OneTokenMarketApi::GetTicks(const std::string &exchange) {
  std::string uri = "/ticks?exchange=";
  uri += utils::UrlEncode(exchange);
  RESTQuoteHandler->SendRequest(onetoken::RESTTYPE_TICKS, uri);
}

void OneTokenMarketApi::GetSingleTick(const std::string &exchange,
                                      const std::string &contract) {
  std::string uri = "/single-tick/";
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
  std::string uri = "/zhubi?contract=";
  uri += utils::UrlEncode(contract);
  uri += "&since=";
  uri += utils::UrlEncode(since);
  uri += "&until=";
  uri += utils::UrlEncode(until);
  uri += "&size=";
  uri += std::to_string(size);

  RESTQuoteHandler->SendRequest(onetoken::RESTTYPE_ZHUBI, uri);
}

void OneTokenMarketApi::GetCandles(const std::string &contract,
                                 const std::string &duration,
                                   std::string &since,
                                   const std::string &until) {
  std::string uri = "/candles?contract=";
  uri += utils::UrlEncode(contract);
  uri += "&duration=";
  uri += utils::UrlEncode(duration);
  uri += "&since=";
  uri += utils::UrlEncode(since);
  if (!until.empty()) {
    uri += "&until=";
    uri += utils::UrlEncode(until);
  }

  RESTQuoteHandler->SendRequest(onetoken::RESTTYPE_CANDLE, uri);
}

void OneTokenMarketApi::Join() { RESTQuoteHandler->Join(); }

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
      root.AddMember("contract", rapidjson::StringRef(contract.c_str()),
                     allocator);
    } else {
      root["contract"].SetString(contract.c_str(), allocator);
    }
    buf.Clear();
    root.Accept(writer);
    WSQuoteTickHandler->Send(buf.GetString());
  }
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
      root.AddMember("contract", rapidjson::StringRef(contract.c_str()),
                     allocator);
    } else {
      root["contract"].SetString(contract.c_str(), allocator);
    }
    buf.Clear();
    root.Accept(writer);
    WSQuoteTickHandler->Send(buf.GetString());
  }
}

void OneTokenMarketApi::SubscribeCandleData(
    const std::vector<std::pair<std::string, std::string>> &req_candles) {
  rapidjson::Document document;
  rapidjson::StringBuffer buf;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
  auto &allocator = document.GetAllocator();
  rapidjson::Value root(rapidjson::kObjectType);

  bool first = true;
  for (auto const &candle : req_candles) {
    if (first) {
      first = false;
      root.AddMember("contract", rapidjson::StringRef(candle.first.c_str()),
                     allocator);
      root.AddMember("duration", rapidjson::StringRef(candle.second.c_str()),
                     allocator);
    } else {
      root["contract"].SetString(candle.first.c_str(), allocator);
      root["duration"].SetString(candle.second.c_str(), allocator);
    }
    buf.Clear();
    root.Accept(writer);
    WSQuoteCandleHandler->Send(buf.GetString());
  }
}

void OneTokenMarketApi::UnsubscribeTickData(
    const std::vector<std::string> &contracts) {
  rapidjson::Document document;
  rapidjson::StringBuffer buf;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
  auto &allocator = document.GetAllocator();
  rapidjson::Value root(rapidjson::kObjectType);

  root.AddMember("uri", "unsubscribe-single-tick-verbose", allocator);

  bool first = true;
  for (auto const &contract : contracts) {
    if (first) {
      first = false;
      root.AddMember("contract", rapidjson::StringRef(contract.c_str()),
                     allocator);
    } else {
      root["contract"].SetString(contract.c_str(), allocator);
    }
    buf.Clear();
    root.Accept(writer);
    WSQuoteTickHandler->Send(buf.GetString());
  }
}

void OneTokenMarketApi::UnsubscribeTradeData(
    const std::vector<std::string> &contracts) {
  rapidjson::Document document;
  rapidjson::StringBuffer buf;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
  auto &allocator = document.GetAllocator();
  rapidjson::Value root(rapidjson::kObjectType);

  root.AddMember("uri", "unsubscribe-single-zhubi-verbose", allocator);

  bool first = true;
  for (auto const &contract : contracts) {
    if (first) {
      first = false;
      root.AddMember("contract", rapidjson::StringRef(contract.c_str()),
                     allocator);
    } else {
      root["contract"].SetString(contract.c_str(), allocator);
    }
    buf.Clear();
    root.Accept(writer);
    WSQuoteTickHandler->Send(buf.GetString());
  }
}

}  // namespace onetoken
