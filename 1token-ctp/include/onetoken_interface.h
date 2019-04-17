#pragma once
#include "structs.h"

#ifdef _WIN32
#ifdef _CLASSINDLL
#define CLASSINDLL_CLASS_DECL __declspec(dllexport)
#else
#define CLASSINDLL_CLASS_DECL __declspec(dllimport)
#endif
#else
#define CLASSINDLL_CLASS_DECL
#endif

namespace onetoken {
class UserInterface {
 public:
  virtual void OnInit(const ControlMessage *message) = 0;
  virtual void OnLogin(const ControlMessage *message) = 0;
  virtual void OnTickDataResponse(const MarketResponseMessage *message) = 0;
  virtual void OnZhubiDataResponse(const MarketResponseMessage *message) = 0;
  virtual void OnCandleDataResponse(const MarketResponseMessage *message) = 0;
  virtual void OnErrorResponse(const ControlMessage *message) = 0;

  virtual void OnGetAccountInfo(const TradeResponseMessage *message) = 0;
  virtual void OnGetOrders(const TradeResponseMessage *message) = 0;
  virtual void OnPlaceOrder(const TradeResponseMessage *message) = 0;
  virtual void OnCancelOrder(const TradeResponseMessage *message) = 0;
  virtual void OnGetTrans(const TradeResponseMessage *message) = 0;
};
}  // namespace onetoken