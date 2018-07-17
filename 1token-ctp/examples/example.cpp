#include <time.h>
#include <iostream>
#include <mutex>
#include <vector>
#include "error_code.h"
#include "onetoken_interface.h"
#include "onetoken_market_api.h"
#include "onetoken_trade_api.h"

using namespace std;
class CustomUserInterface;

class CustomUserInterface : public onetoken::UserInterface {
 public:
  virtual void OnInit(const onetoken::ControlMessage *message) {
    cout << "init!! " << message->ToString() << endl;
    if (message->header.error_code == onetoken::SUCCESS) {
      if (message->header.req_type == onetoken::REQ_WS_TICK_AND_ZHUBI) {
        market_api_->WSTickLogin();
      } else if (message->header.req_type == onetoken::REQ_WS_CANDLE) {
        // do something
      }
    }
  }
  virtual void OnLogin(const onetoken::ControlMessage *message) {
    cout << "login!!" << endl;
    if (message->header.error_code == onetoken::SUCCESS) {
      if (message->header.req_type == onetoken::REQ_WS_TICK_AND_ZHUBI) {
        std::vector<std::string> c{"huobip/btc.usdt"};
        market_api_->SubscribeTickData(c);
        // api_->SubscribeTradeData(c);
      }
    }
  }

  virtual void OnMarketDataResponse(
      const onetoken::MarketResponseMessage *message) {
    cout << "data resp:" << endl << message->ToString() << endl;
  }

  virtual void OnErrorResponse(const onetoken::ControlMessage *message) {
    cout << "err resp:" << message->info << endl;
  }

  virtual void OnGetAccountInfo() {}

  virtual void OnGetOrders() {}

  virtual void OnPlaceOrder(const onetoken::TradeResponseMessage *message) {
    cout << "OnPlaceOrder!" << endl;
    for (auto &order : message->order_info) {
      onetoken::RequestOrderInfo o;
      bool has_oid = false;
      if (!order.exchange_oid.empty()) {
        o.exchange_oid = order.exchange_oid;
        has_oid = true;
      } else if (!order.client_oid.empty()) {
        o.client_oid = order.client_oid;
        has_oid = true;
      }

      if (has_oid) {
        mutex_.lock();
        vec_.push_back(o);
        mutex_.unlock();
      }
    }
  }

  virtual void OnCancelOrder(const onetoken::TradeResponseMessage *message) {
    cout << "OnCancelOrder!" << endl;
    for (auto &order : message->order_info) {
      cout << "cancel: " << order.exchange_oid << endl;
    }
  }

  void SetMarketApi(onetoken::OneTokenMarketApi *api) { market_api_ = api; }
  void SetTradeApi(onetoken::OneTokenTradeApi *api) { trade_api_ = api; }

  std::vector<onetoken::RequestOrderInfo> vec_;

 private:
  onetoken::OneTokenMarketApi *market_api_;
  onetoken::OneTokenTradeApi *trade_api_;
  std::mutex mutex_;
};

int main() {
  CustomUserInterface *test_interface = new CustomUserInterface();
  onetoken::OneTokenMarketApi api(test_interface);
  test_interface->SetMarketApi(&api);
  // api.WSInit(true);
  api.RESTInit();
  // api.GetZhubiList("huobip/btc.usdt", "2018-07-10T12:55:00+08:00",
  //                 "2018-07-10T13:00:00+08:00", 5);
  // api.Join();

  onetoken::OneTokenTradeApi trade_api(test_interface);
  test_interface->SetTradeApi(&trade_api);
  trade_api.Init("ZrGwKMAc-bq03zHS0-LR9VXmKX-d0roXFPu",
                 "T7IyC9sd-hqUDNLAo-odcC3t9e-qXI1DmPJ");
  // trade_api.GetAccountInfo("huobip", "jerry");
  onetoken::TradeBaseInfo base_info;
  base_info.exchange = "huobip";
  base_info.account_name = "jerry";
  onetoken::RequestOrderInfo order_info;
  order_info.contract = "huobip/eth.btc";
  order_info.amount = 0.001;
  order_info.price = 0.00001;
  order_info.bs = "b";
  trade_api.PlaceOrder(base_info, order_info);

  trade_api.Join();

  trade_api.CancelOrder(base_info, test_interface->vec_);
  base_info.contract = "huobip/eth.btc";
  // trade_api.CancelAllOrders(base_info);

  trade_api.Join();

  cin.get();
  return 0;
}
