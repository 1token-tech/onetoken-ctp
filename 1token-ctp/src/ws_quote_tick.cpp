#include <gzip/decompress.hpp>
#include "ws_quote_tick.h"

namespace onetoken {
void WSQuoteTick::OnMessage(websocketpp::connection_hdl hdl, MessagePtr msg) {
  auto payload = msg->get_payload();

  if (enable_gzip_) {
    payload = gzip::decompress(payload.data(), payload.size());
  }

  MarketResponseMessage message;
  message.header.req_type = REQ_WS_TICK_AND_ZHUBI;
  rapidjson::Document doc;
  if (doc.Parse(payload.c_str()).HasParseError()) {
    HandleError(RESP_MESSAGE_FORMAT_ERROR, "parse resp data failed.");
    return;
  }

  std::string uri;
  if (doc.HasMember("uri")) {
    uri = doc["uri"].GetString();
  }

  if (uri == "auth") {
    ws_info_.authorized = true;
    if (user_interface_ != NULL) {
      ControlMessage message;
      message.header.error_code = SUCCESS;
      message.header.seq = 0;
      message.header.req_type = REQ_WS_TICK_AND_ZHUBI;
      message.header.resp_type = RESP_TICK;
      message.header.version = 1;
      message.info = id_;
      user_interface_->OnLogin(&message);
      std::thread ping_thread(std::bind(&WSQuote::Ping, this));
      ping_thread.detach();
    }
    return;
  } else if (!ws_info_.authorized) {
    HandleError(RESP_MESSAGE_FORMAT_ERROR, "unauthorized");
    return;
  }

  if (uri == "pong") {
    // do something
    return;
  }

  if (!doc.HasMember("data") && doc.HasMember("message")) {
    return;
  }

  if (uri == "single-tick-verbose") {
    if (doc.HasMember("data")) {
      auto &data = doc["data"];
      auto ret = ParseTickData(data, message);
      if (ret == SUCCESS) {
        message.header.resp_type = RESP_TICK;
        message.header.seq = seq_++;
      } else {
        HandleError(RESP_MESSAGE_FORMAT_ERROR, "tick data maybe wrong.");
        return;
      }

      message.header.error_code = ret;
      message.header.version = 1;
      user_interface_->OnMarketDataResponse(&message);
    }
  } else if (uri == "single-zhubi-verbose") {
    if (doc.HasMember("data")) {
      auto &data = doc["data"];
      auto ret = ParseZhubiData(data, message);
      if (ret == SUCCESS) {
        message.header.resp_type = RESP_ZHUBI;
        message.header.seq = seq_++;
      } else {
        HandleError(RESP_MESSAGE_FORMAT_ERROR, "zhubi data maybe wrong.");
        return;
      }

      message.header.error_code = ret;
      message.header.version = 1;
      user_interface_->OnMarketDataResponse(&message);
    }
  } else {
    HandleError(UNKNOWN_RESP_URI, "unknown resp uri");
  }
}
}
