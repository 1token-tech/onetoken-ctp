#include <iostream>
#include <vector>
#include "error_code.h"
#include "onetoken_interface.h"
#include "onetoken_market_api.h"

using namespace std;
class CustomUserInterface;

class CustomUserInterface : public onetoken::UserInterface {
 public:
  virtual void OnInit(const onetoken::MessageHeader *message) {
    cout << "init!!" << endl;
    if (message->error_code == onetoken::SUCCESS) {
      api_->WSLogin();
    }
  }
  virtual void OnLogin(const onetoken::MessageHeader *message) {
    cout << "login!!" << endl;
    if (message->error_code == onetoken::SUCCESS) {
      std::vector<std::string> c{"huobip/btc.usdt"};
      api_->SubscribeTickData(c);
      api_->SubscribeTradeData(c);
    }
  }

  virtual void OnMarketDataResponse(
      const onetoken::MarketResponseMessage *message) {
    cout << "data resp:" << endl << message->ToString() << endl;
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
                   "2018-07-10T13:00:00+08:00", 10);
  cin.get();
  return 0;
}
