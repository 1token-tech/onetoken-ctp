#pragma once
#include <unordered_map>
#include "onetoken_interface.h"
#include "quote.h"
#include "singleton.h"
#include "rapidjson/document.h"
#include "cpprest/http_client.h"

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
  void HandleTicksResponse(web::http::http_response response);
  void HandleSingleTickResponse(web::http::http_response response);
  void HandleZhubiResponse(web::http::http_response response);
  void SetBaseUrl(const std::string &base_url) { base_url_ = base_url; }

 private:
  std::string base_url_;
};
}  // namespace onetoken

#define RESTQuoteHandler Singleton<RestQuote>::GetInstance()