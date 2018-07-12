#pragma once
#include <unordered_map>
#include "http_client.h"
#include "onetoken_interface.h"
#include "quote.h"
#include "rapidjson/document.h"

namespace onetoken {
enum RestType {
  RESTTYPE_SINGLE_TICK,
  RESTTYPE_TICKS,
  RESTTYPE_ZHUBI,
  RESTTYPE_CANDLE
};

class RestQuote : public Quote {
 public:
  RestQuote() { req_type_ = REQ_REST; }
  void SendRequest(RestType type, const std::string &uri);
  void Process(RestType type, const std::string &url);
  void HandleTicksResponse(const std::string &resp);
  void HandleSingleTickResponse(const std::string &resp);
  void HandleZhubiResponse(const std::string &resp);
  void SetBaseUrl(const std::string &base_url) { base_url_ = base_url; }

 private:
  std::string base_url_;
};
}  // namespace onetoken

#define RESTQuoteHandler Singleton<RestQuote>::GetInstance()