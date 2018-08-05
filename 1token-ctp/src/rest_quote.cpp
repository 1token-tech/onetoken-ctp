#include <iostream>
#include "error_code.h"
#include "rest_quote.h"

namespace onetoken {

void RestQuote::SendRequest(RestType type, const std::string &uri) {
  std::string url = base_url_;
  url += uri;

  tasks_.emplace_back(std::make_shared<std::thread>(std::bind(&RestQuote::Process, this, type, url)));
}

void RestQuote::Process(RestType type, std::string url) {
  try {
    web::http::client::http_client_config config;
    config.set_timeout(utility::seconds(5));

    web::http::client::http_client raw_client(
        utility::conversions::to_string_t(url), config);
    web::http::http_request request(web::http::methods::GET);
    request.headers().add(U("Content-Type"),
                          U("application/json;charset=utf-8"));
    auto response = raw_client.request(request).get();
    concurrency::streams::stringstreambuf buf;
    response.body().read_to_end(buf).wait();
    auto resp = buf.collection();

    switch (type) {
      case RESTTYPE_SINGLE_TICK:
        HandleSingleTickResponse(resp);
        break;
      case RESTTYPE_TICKS:
        HandleTicksResponse(resp);
        break;
      case RESTTYPE_ZHUBI:
        HandleZhubiResponse(resp);
        break;
      default:
        HandleError(UNRECOGNIZED_REQ, "unknown request type");
        break;
    }
  } catch (...) {
    HandleError(CONNECT_FAILED, "connection failed");
  }
}

void RestQuote::Join() {
  for (auto &task : tasks_) {
    task->join();
  }
  tasks_.clear();
}

void RestQuote::HandleTicksResponse(const std::string &resp) {
  MarketResponseMessage message;
  message.header.req_type = REQ_REST;
  rapidjson::Document doc;
  if (resp.empty() || doc.Parse(resp.c_str()).HasParseError()) {
    message.header.resp_type = RESP_ERROR;
    message.header.error_code = RESP_MESSAGE_FORMAT_ERROR;
    user_interface_->OnMarketDataResponse(&message);
    return;
  }

  if (!doc.IsArray()) {
    message.header.resp_type = RESP_ERROR;
    message.header.error_code = RESP_MESSAGE_FORMAT_ERROR;
    user_interface_->OnMarketDataResponse(&message);
    return;
  }

  for (auto &tick_data : doc.GetArray()) {
    auto ret = ParseTickData(tick_data, message);
    if (ret == SUCCESS) {
      message.header.resp_type = RESP_TICK;
      message.header.seq = seq_++;
    } else {
      message.header.resp_type = RESP_ERROR;
    }
  }

  user_interface_->OnMarketDataResponse(&message);
}

void RestQuote::HandleSingleTickResponse(const std::string &resp) {
  MarketResponseMessage message;
  message.header.req_type = REQ_REST;
  rapidjson::Document doc;
  if (resp.empty() || doc.Parse(resp.c_str()).HasParseError()) {
    HandleError(RESP_MESSAGE_FORMAT_ERROR, "Parse resp data failed.");
    return;
  }

  auto ret = ParseTickData(doc, message);
  if (ret == SUCCESS) {
    message.header.resp_type = RESP_TICK;
    message.header.seq = seq_++;
  } else {
    HandleError(RESP_MESSAGE_FORMAT_ERROR, "Tick data maybe wrong.");
    return;
  }

  message.header.error_code = ret;
  message.header.version = 1;
  user_interface_->OnMarketDataResponse(&message);
}

void RestQuote::HandleZhubiResponse(const std::string &resp) {
  MarketResponseMessage message;
  message.header.req_type = REQ_REST;
  rapidjson::Document doc;
  if (resp.empty() || doc.Parse(resp.c_str()).HasParseError()) {
    HandleError(RESP_MESSAGE_FORMAT_ERROR, "Parse resp data failed.");
    return;
  }

  if (!doc.IsArray()) {
    HandleError(RESP_MESSAGE_FORMAT_ERROR, "Wrong data format.");
    return;
  }

  auto ret = ParseZhubiData(doc, message);
  if (ret == SUCCESS) {
    message.header.resp_type = RESP_ZHUBI;
    message.header.seq = ++seq_;
  } else {
    HandleError(RESP_MESSAGE_FORMAT_ERROR, "Tick data maybe wrong.");
    return;
  }

  message.header.error_code = ret;
  message.header.version = 1;
  user_interface_->OnMarketDataResponse(&message);
}

}  // namespace onetoken
