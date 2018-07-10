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

enum WebSocketType { WSTYPE_TICK, WSTYPE_CANDLE, WSTYPE_MAX };
enum Status { CLOSED, CONNECTING, CONNECTED };

struct WebSocketInfo {
  Status status;
  std::string url;
  WSSClient client;
  WSSClient::connection_ptr connection;
  websocketpp::lib::shared_ptr<websocketpp::lib::thread> runner;
  bool authorized;
  bool is_gzip;
};

class WSQuote : public Quote {
 public:
  WSQuote();
  void Connect(WebSocketType type, std::string &url);
  // void Reconnect();
  void Send(WebSocketType type, std::string &message);
  void Send(WebSocketType type, const char *message);

  ContextPtr TLSInit(websocketpp::connection_hdl hdl);
  void OnOpen(WSSClient *c, websocketpp::connection_hdl hdl);
  void OnTickOrZhubiMessage(websocketpp::connection_hdl hdl, MessagePtr msg);
  void OnCandleMessage(websocketpp::connection_hdl hdl, MessagePtr msg);
  void SetEnableGzip(bool enable_gzip) { enable_gzip_ = enable_gzip; }

 private:
  WebSocketInfo websocket_list_[WSTYPE_MAX];
  std::mutex mutex_;
  bool enable_gzip_;
};
}  // namespace onetoken

#define WSQuoteHandler Singleton<WSQuote>::GetInstance()