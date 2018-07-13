#include <iostream>
#include <thread>
#include "error_code.h"
#include "rest_quote.h"

namespace onetoken {

void RestQuote::SendRequest(RestType type, const std::string &uri) {
  utility::string_t url = utility::conversions::to_string_t(base_url_);
  url += utility::conversions::to_string_t(uri);

  std::wcout << url << std::endl;
  web::http::client::http_client raw_client(url);
  web::http::http_request request(web::http::methods::GET);
  request.headers().add(L"Content-Type", L"application/json;charset=utf-8");
  switch (type) {
    case RESTTYPE_SINGLE_TICK:
      raw_client.request(request).then(std::bind(
          &RestQuote::HandleSingleTickResponse, this, std::placeholders::_1));
      break;
    case RESTTYPE_TICKS:
      raw_client.request(request).then(std::bind(
          &RestQuote::HandleTicksResponse, this, std::placeholders::_1));
      break;
    case RESTTYPE_ZHUBI:
      raw_client.request(request).then(std::bind(
          &RestQuote::HandleZhubiResponse, this, std::placeholders::_1));
      break;
    default:
      HandleError(UNRECOGNIZED_REQ, "unknown request type");
      break;
  }
}

void RestQuote::HandleTicksResponse(web::http::http_response response) {
  concurrency::streams::stringstreambuf buf;
  response.body().read_to_end(buf).wait();
  auto resp = buf.collection();

  MarketResponseMessage message;
  message.header.req_type = REQ_REST;
  rapidjson::Document doc;
  if (resp.empty() || doc.Parse(resp).HasParseError()) {
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

      // use exchange
      auto contract = message.tick_list[0].contract;
      contract = contract.substr(0, contract.find_first_of("/"));
      auto contract_iter = contract_seq_map_.find(contract);
      uint32_t seq = 0;
      if (contract_iter == contract_seq_map_.end()) {
        contract_seq_map_.emplace(contract, seq);
      } else {
        seq = contract_seq_map_[contract]++;
      }
      message.header.seq = seq;
    } else {
      message.header.resp_type = RESP_ERROR;
    }
  }

  user_interface_->OnMarketDataResponse(&message);
}

void RestQuote::HandleSingleTickResponse(web::http::http_response response) {
  concurrency::streams::stringstreambuf buf;
  response.body().read_to_end(buf).wait();
  auto resp = buf.collection();

  MarketResponseMessage message;
  message.header.req_type = REQ_REST;
  rapidjson::Document doc;
  if (resp.empty() || doc.Parse(resp).HasParseError()) {
    HandleError(RESP_MESSAGE_FORMAT_ERROR, "Parse resp data failed.");
    return;
  }

  auto ret = ParseTickData(doc, message);
  if (ret == SUCCESS) {
    message.header.resp_type = RESP_TICK;

    auto contract = message.tick_list[0].contract;
    auto contract_iter = contract_seq_map_.find(contract);
    uint32_t seq = 0;
    if (contract_iter == contract_seq_map_.end()) {
      contract_seq_map_.emplace(contract, seq);
    } else {
      seq = contract_seq_map_[contract]++;
    }
    message.header.seq = seq;
  } else {
    HandleError(RESP_MESSAGE_FORMAT_ERROR, "Tick data maybe wrong.");
    return;
  }

  message.header.error_code = ret;
  message.header.version = 1;
  user_interface_->OnMarketDataResponse(&message);
}

void RestQuote::HandleZhubiResponse(web::http::http_response response) {
  concurrency::streams::stringstreambuf buf;
  response.body().read_to_end(buf).wait();
  auto resp = buf.collection();
  MarketResponseMessage message;
  message.header.req_type = REQ_REST;
  rapidjson::Document doc;
  if (resp.empty() || doc.Parse(resp).HasParseError()) {
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

    auto contract = message.zhubi_list[0].contract;
    auto contract_iter = contract_seq_map_.find(contract);
    uint32_t seq = 0;
    if (contract_iter == contract_seq_map_.end()) {
      contract_seq_map_.emplace(contract, seq);
    } else {
      seq = contract_seq_map_[contract]++;
    }
    message.header.seq = seq;
  } else {
    HandleError(RESP_MESSAGE_FORMAT_ERROR, "Tick data maybe wrong.");
    return;
  }

  message.header.error_code = ret;
  message.header.version = 1;
  user_interface_->OnMarketDataResponse(&message);
}

}  // namespace onetoken