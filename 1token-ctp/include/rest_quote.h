#pragma once
#include <thread>
#include <unordered_map>
#include "cpprest/http_client.h"
#include "onetoken_interface.h"
#include "quote.h"
#include "rapidjson/document.h"
#include "singleton.h"
#include "thread_pool.h"

namespace onetoken {
enum RestType {
  RESTTYPE_SINGLE_TICK,
  RESTTYPE_TICKS,
  RESTTYPE_ZHUBI,
  RESTTYPE_CANDLE
};

class RestQuote : public Quote {
 public:
  RestQuote() {
    seq_ = 0;
    req_type_ = REQ_REST;
    thread_pool_.set_thread_num(8);
  }
  void Init();
  void SendRequest(RestType type, const std::string &uri);
  void Process(RestType type, std::string url);
  void Join();
  void HandleTicksResponse(const std::string &resp);
  void HandleSingleTickResponse(const std::string &resp);
  void HandleZhubiResponse(const std::string &resp);
  void HandleCandlesResponse(const std::string &resp);
  void SetBaseUrl(const std::string &base_url) { base_url_ = base_url; }

 private:
  std::string base_url_;
  ThreadPool thread_pool_;
};
}  // namespace onetoken

#define RESTQuoteHandler Singleton<RestQuote>::GetInstance()
