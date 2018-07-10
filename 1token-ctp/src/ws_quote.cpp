#include <gzip/decompress.hpp>
#include <sstream>
#include <websocketpp/common/thread.hpp>
#include "ws_quote.h"

using namespace websocketpp::lib::placeholders;

namespace onetoken {
WSQuote::WSQuote() {
  for (int i = 0; i < WSTYPE_MAX; ++i) {
    WebSocketInfo &ws_info = websocket_list_[i];
    ws_info.status = CLOSED;
    ws_info.client.clear_access_channels(websocketpp::log::alevel::all);
    ws_info.client.clear_error_channels(websocketpp::log::elevel::all);
    ws_info.client.init_asio();
    ws_info.client.start_perpetual();
    ws_info.client.set_tls_init_handler(
        websocketpp::lib::bind(&WSQuote::TLSInit, this, _1));

    ws_info.client.set_open_handler(
        websocketpp::lib::bind(&WSQuote::OnOpen, this, &ws_info.client, _1));

    if (i == WSTYPE_TICK) {
      ws_info.client.set_message_handler(
          websocketpp::lib::bind(&WSQuote::OnTickOrZhubiMessage, this, _1, _2));
    }
    ws_info.authorized = false;
    ws_info.runner = websocketpp::lib::make_shared<websocketpp::lib::thread>(
        &WSSClient::run, &websocket_list_[i].client);
  }
}

void WSQuote::Connect(WebSocketType type, std::string &url) {
  auto &ws_info = websocket_list_[type];
  ws_info.url = url;
  if (enable_gzip_) {
    ws_info.url += "?gzip=true";
  }

  websocketpp::lib::error_code err;
  ws_info.connection = ws_info.client.get_connection(ws_info.url, err);
  if (err) {
    std::ostringstream ss;
    ss << "connect to ws failed: " << url;
    HandleError(CONNECT_FAILED, ss.str());
    return;
  }
  ws_info.client.connect(ws_info.connection);
  ws_info.status = CONNECTING;

  // TODO:send pingpong to detect
}

ContextPtr WSQuote::TLSInit(websocketpp::connection_hdl hdl) {
  // establishes a SSL connection
  ContextPtr ctx =
      std::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);
  try {
    ctx->set_options(
        asio::ssl::context::default_workarounds | asio::ssl::context::no_sslv2 |
        asio::ssl::context::no_sslv3 | asio::ssl::context::single_dh_use);
  } catch (std::exception &e) {
    std::cout << "Error in context pointer: " << e.what() << std::endl;
  }
  return ctx;
}

void WSQuote::Send(WebSocketType type, std::string &message) {
  Send(type, message.c_str());
}

void WSQuote::Send(WebSocketType type, const char *message) {
  auto &ws_info = websocket_list_[type];
  if (ws_info.status != CONNECTED) {
    return;
  }
  websocketpp::lib::error_code err;
  ws_info.client.send(ws_info.connection->get_handle(), message,
                      websocketpp::frame::opcode::text, err);
  if (err) {
    std::cout << "send message fail. errcode: " << err << std::endl;
  }
}

void WSQuote::OnOpen(WSSClient *c, websocketpp::connection_hdl hdl) {
  for (int i = 0; i < WSTYPE_MAX; ++i) {
    if (&websocket_list_[i].client == c) {
      websocket_list_[i].status = CONNECTED;
      break;
    }
  }

  int flag = CONNECTED;
  mutex_.lock();
  for (int i = 0; i < WSTYPE_MAX; ++i) {
    flag &= websocket_list_[i].status;
  }
  mutex_.unlock();
  if (flag && user_interface_ != NULL) {
    MessageHeader header;
    header.error_code = SUCCESS;
    header.seq = 0;
    header.version = 1;
    user_interface_->OnInit(&header);
  }
}

void WSQuote::OnTickOrZhubiMessage(websocketpp::connection_hdl hdl,
                                   MessagePtr msg) {
  auto payload = msg->get_payload();

  if (enable_gzip_) {
    payload = gzip::decompress(payload.data(), payload.size());
  }

  MarketResponseMessage message;
  message.header.req_type = REQ_WEBSOCKET;
  rapidjson::Document doc;
  if (doc.Parse(payload).HasParseError()) {
    HandleError(RESP_MESSAGE_FORMAT_ERROR, "parse resp data failed.");
    return;
  }

  std::string uri;
  if (doc.HasMember("uri")) {
    uri = doc["uri"].GetString();
  }

  if (uri == "auth") {
    websocket_list_[WSTYPE_TICK].authorized = true;
    if (user_interface_ != NULL) {
      MessageHeader header;
      header.error_code = SUCCESS;
      header.seq = 0;
      header.resp_type = RESP_TICK;
      header.version = 1;
      user_interface_->OnLogin(&header);
    }
    return;
  } else if (!websocket_list_[WSTYPE_TICK].authorized) {
    HandleError(RESP_MESSAGE_FORMAT_ERROR, "not authorized");
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
        HandleError(RESP_MESSAGE_FORMAT_ERROR, "zhubi data maybe wrong.");
        return;
      }

      message.header.error_code = ret;
      message.header.version = 1;
      user_interface_->OnMarketDataResponse(&message);
    }
  }
}

void WSQuote::OnCandleMessage(websocketpp::connection_hdl hdl, MessagePtr msg) {
}
}  // namespace onetoken