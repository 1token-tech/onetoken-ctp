#include <iostream>
#include <vector>
#include "error_code.h"
#include "onetoken_interface.h"
#include "onetoken_market_api.h"

using namespace std;
class CustomUserInterface;

class CustomUserInterface : public onetoken::UserInterface {
 public:
  virtual void OnInit(const onetoken::ControlMessage *message) {
    cout << "init!! " << message->ToString() << endl;
    if (message->header.error_code == onetoken::SUCCESS) {
      if (message->header.req_type == onetoken::REQ_WS_TICK_AND_ZHUBI) {
        api_->WSTickLogin();
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
        api_->SubscribeTickData(c);
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

  void SetApi(onetoken::OneTokenMarketApi *api) { api_ = api; }

 private:
  onetoken::OneTokenMarketApi *api_;
};

int main() {
  CustomUserInterface *test_interface = new CustomUserInterface();
  onetoken::OneTokenMarketApi api(test_interface);
  test_interface->SetApi(&api);
  // api.WSInit(true);
  api.RESTInit();
  api.GetZhubiList("huobip/btc.usdt", "2018-07-10T12:55:00+08:00",
                   "2018-07-10T13:00:00+08:00", 5);
  api.Join();

  cin.get();
  return 0;
}
