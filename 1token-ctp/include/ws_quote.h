#pragma once
#include <mutex>
#include <unordered_map>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include "error_code.h"
#include "quote.h"
#include "rapidjson/document.h"
#include "singleton.h"

typedef websocketpp::client<websocketpp::config::asio_tls_client> WSSClient;
typedef std::shared_ptr<asio::ssl::context> ContextPtr;
typedef websocketpp::config::asio_client::message_type::ptr MessagePtr;

namespace onetoken {
enum Status {
  CLOSED,
  CONNECTING,
  CONNECTED
};

struct WebSocketInfo {
  Status status;
  std::string url;
  WSSClient client;
  WSSClient::connection_ptr connection;
  std::thread runner;
  std::string ping_message;
  bool authorized;
};

class WSQuote : public Quote {
 public:
  WSQuote(const std::string &id);
  virtual ~WSQuote() = 0;
  void Connect(std::string &url);
  // void Reconnect();
  bool Send(std::string &message);
  bool Send(const char *message);

  void Ping();
  ContextPtr TLSInit(websocketpp::connection_hdl hdl);
  virtual void OnOpen(WSSClient *c, websocketpp::connection_hdl hdl);
  virtual void OnFail(WSSClient *c, websocketpp::connection_hdl hdl);
  virtual void OnClose(WSSClient *c, websocketpp::connection_hdl hdl);
  virtual void OnPongMessage(websocketpp::connection_hdl hdl, std::string msg);
  virtual void OnTimeout(websocketpp::connection_hdl hdl, std::string msg);
  virtual void OnMessage(websocketpp::connection_hdl hdl, MessagePtr msg);

  void SetEnableGzip(bool enable_gzip) { enable_gzip_ = enable_gzip; }

  Status GetStatus() { return ws_info_.status; }

 protected:
  WebSocketInfo ws_info_;
  bool enable_gzip_;
  std::string id_;
};
}  // namespace onetoken