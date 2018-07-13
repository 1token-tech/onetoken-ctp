#include <sstream>
#include <websocketpp/common/thread.hpp>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "ws_quote.h"

using namespace websocketpp::lib::placeholders;

namespace onetoken {
WSQuote::WSQuote(const std::string &id) {
  ws_info_.status = CLOSED;
  ws_info_.authorized = false;
  id_ = id;

  ws_info_.client.clear_access_channels(websocketpp::log::alevel::all);
  ws_info_.client.clear_error_channels(websocketpp::log::elevel::all);
  ws_info_.client.init_asio();
  ws_info_.client.start_perpetual();

  ws_info_.client.set_tls_init_handler(
      websocketpp::lib::bind(&WSQuote::TLSInit, this, _1));

  ws_info_.client.set_open_handler(
      websocketpp::lib::bind(&WSQuote::OnOpen, this, &ws_info_.client, _1));

  ws_info_.client.set_fail_handler(
      websocketpp::lib::bind(&WSQuote::OnFail, this, &ws_info_.client, _1));

  ws_info_.client.set_close_handler(
      websocketpp::lib::bind(&WSQuote::OnOpen, this, &ws_info_.client, _1));

  ws_info_.client.set_pong_handler(
      websocketpp::lib::bind(&WSQuote::OnPongMessage, this, _1, _2));

  ws_info_.client.set_pong_timeout_handler(
      websocketpp::lib::bind(&WSQuote::OnTimeout, this, _1, _2));

  ws_info_.client.set_message_handler(
      websocketpp::lib::bind(&WSQuote::OnMessage, this, _1, _2));

  rapidjson::Document document;
  rapidjson::StringBuffer buf;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
  auto &allocator = document.GetAllocator();
  rapidjson::Value root(rapidjson::kObjectType);
  root.AddMember("uri", "ping", allocator);
  root.Accept(writer);
  ws_info_.ping_message = buf.GetString();

  ws_info_.runner = std::thread(std::bind(&WSSClient::run, &ws_info_.client));
}

WSQuote::~WSQuote() {}

void WSQuote::Connect(std::string &url) {
  ws_info_.url = url;
  if (enable_gzip_) {
    ws_info_.url += "?gzip=true";
  }

  websocketpp::lib::error_code err;
  ws_info_.connection = ws_info_.client.get_connection(ws_info_.url, err);
  if (err) {
    std::ostringstream ss;
    ss << "connect to ws failed: " << url;
    HandleError(CONNECT_FAILED, ss.str());
    return;
  }
  ws_info_.client.connect(ws_info_.connection);
  ws_info_.status = CONNECTING;
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

bool WSQuote::Send(std::string &message) { return Send(message.c_str()); }

bool WSQuote::Send(const char *message) {
  if (ws_info_.status != CONNECTED) {
    HandleError(CONNECT_FAILED, "not connected");
    return false;
  }
  websocketpp::lib::error_code err;
  ws_info_.client.send(ws_info_.connection->get_handle(), message,
                       websocketpp::frame::opcode::text, err);
  if (err) {
    std::cout << "send message fail. errcode: " << err << std::endl;
    return false;
  } else {
    return true;
  }
}

void WSQuote::OnOpen(WSSClient *c, websocketpp::connection_hdl hdl) {
  ws_info_.status = CONNECTED;

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

void WSQuote::OnFail(WSSClient *c, websocketpp::connection_hdl hdl) {
  std::cout << id_ << "failed" << std::endl;
}

void WSQuote::OnClose(WSSClient *c, websocketpp::connection_hdl hdl) {
  ws_info_.status = CLOSED;
  HandleError(SERVER_CLOSED, "connection closed");
}

void WSQuote::Ping() {
  while (ws_info_.status == CONNECTED) {
    try {
      std::cout << id_ << " ping: " << ws_info_.ping_message << std::endl;
      bool ret = Send(ws_info_.ping_message);
#ifdef _WIN32
      Sleep(5000);
#else
      sleep(5);
#endif
    } catch (...) {
      break;
    }
  }
  std::cout << id_ << "stop ping" << std::endl;
}

void WSQuote::OnPongMessage(websocketpp::connection_hdl hdl, std::string msg) {
  // do something
  std::cout << "pong message: " << msg << std::endl;
}

void WSQuote::OnTimeout(websocketpp::connection_hdl hdl, std::string msg) {
  std::cout << "timeout message: " << msg << std::endl;
}

void WSQuote::OnMessage(websocketpp::connection_hdl hdl, MessagePtr msg) {
  std::cout << "resp message: " << msg->get_payload() << std::endl;
}
}  // namespace onetoken