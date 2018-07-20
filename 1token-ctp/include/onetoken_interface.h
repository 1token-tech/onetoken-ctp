#pragma once
#include "structs.h"

namespace onetoken {
class UserInterface {
 public:
  virtual void OnInit(const ControlMessage *message) = 0;
  virtual void OnLogin(const ControlMessage *message) = 0;
  virtual void OnMarketDataResponse(const MarketResponseMessage *message) = 0;
  virtual void OnErrorResponse(const ControlMessage *message) = 0;

  virtual void OnGetAccountInfo(const TradeResponseMessage *message) = 0;
  virtual void OnGetOrders(const TradeResponseMessage *message) = 0;
  virtual void OnPlaceOrder(const TradeResponseMessage *message) = 0;
  virtual void OnCancelOrder(const TradeResponseMessage *message) = 0;
};
}  // namespace onetoken