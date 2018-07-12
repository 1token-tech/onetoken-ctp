#pragma once
#include "ws_quote.h"

namespace onetoken {
class WSQuoteCandle : public WSQuote {
 public:
  WSQuoteCandle() : WSQuote("candle-ws") { req_type_ = REQ_WS_CANDLE; }

  virtual void OnOpen(WSSClient *c, websocketpp::connection_hdl hdl);
  virtual void OnMessage(websocketpp::connection_hdl hdl, MessagePtr msg);
};
}  // namespace onetoken
#define WSQuoteCandleHandler Singleton<WSQuoteCandle>::GetInstance()