#pragma once
#include "ws_quote.h"

namespace onetoken {
class WSQuoteTick : public WSQuote {
 public:
  WSQuoteTick() : WSQuote("tick-ws") { req_type_ = REQ_WS_TICK_AND_ZHUBI; }
  virtual void OnMessage(websocketpp::connection_hdl hdl, MessagePtr msg);
};
}  // namespace onetoken

#define WSQuoteTickHandler Singleton<WSQuoteTick>::GetInstance()