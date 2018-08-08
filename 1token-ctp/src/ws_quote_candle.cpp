#include <gzip/decompress.hpp>
#include "ws_quote_candle.h"

namespace onetoken {

void WSQuoteCandle::OnOpen(WSSClient *c, websocketpp::connection_hdl hdl) {
  ws_info_.status = CONNECTED;
  std::thread ping_thread(std::bind(&WSQuote::Ping, this));
  ping_thread.detach();

  if (user_interface_ != NULL) {
    ControlMessage message;
    message.header.error_code = SUCCESS;
    message.header.seq = 0;
    message.header.version = 1;
    message.header.req_type = req_type_;
    message.info = id_;
    user_interface_->OnInit(&message);
  }
}

void WSQuoteCandle::OnMessage(websocketpp::connection_hdl hdl, MessagePtr msg) {
  auto payload = msg->get_payload();

  if (enable_gzip_) {
    payload = gzip::decompress(payload.data(), payload.size());
  }

  MarketResponseMessage message;
  message.header.req_type = REQ_WS_CANDLE;
  rapidjson::Document doc;
  if (doc.Parse(payload.c_str()).HasParseError()) {
    HandleError(RESP_MESSAGE_FORMAT_ERROR, "parse resp data failed.");
    return;
  }

  std::string uri;
  if (doc.HasMember("uri")) {
    uri = doc["uri"].GetString();
  }

  std::cout << "candle:" << payload << std::endl;

  if (uri == "pong") {
    // do something
    return;

  }
  if (doc.HasMember("message")) {
    HandleError(RESP_FAILED_MESSAGE, doc["message"].GetString());
    return;
  }

  auto ret = ParseCandleData(doc, message);
  if (ret == SUCCESS) {
    message.header.resp_type = RESP_CANDLE;
    message.header.seq = seq_++;
  } else {
    HandleError(RESP_MESSAGE_FORMAT_ERROR, "Candle data maybe wrong.");
    return;
  }

  message.header.error_code = ret;
  message.header.version = 1;
  user_interface_->OnCandleDataResponse(&message);
}
}  // namespace onetoken
