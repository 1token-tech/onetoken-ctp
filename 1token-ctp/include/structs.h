#pragma once
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace onetoken {
enum ReqType {
  REQ_REST,
  REQ_WS_TICK_AND_ZHUBI,
  REQ_WS_CANDLE,
  REQ_ACCOUNT_INFO,
  REQ_GET_ORDERS,
  REQ_PLACE_ORDER,
  REQ_CANCEL_ORDER,
  REQ_GET_TRANS
};
enum RespType { RESP_TICK, RESP_ZHUBI, RESP_CANDLE, RESP_ACCOUNT, RESP_ORDER, RESP_TRANS, RESP_ERROR };

enum SideType { BUY, SELL };

struct TradeBaseInfo {
  std::string exchange;
  std::string account_name;
  std::string contract;
  std::string state;
};

struct AccountInfo {
  struct Position {
    std::string contract;
    double total_amount;
    double available;
    double frozen;
    double market_value;
    double value_cny;
    std::string type;
  };
  double balance;
  double cash;
  double market_value;
  std::unordered_map<std::string, double> market_value_detail;
  std::vector<Position> positions;
};

struct RequestOrderInfo {
  std::string contract;
  std::string bs;
  double price;
  double amount;
  std::string client_oid;
  std::string exchange_oid;
};

struct ResponseOrderInfo {
  std::string account;
  std::string contract;
  std::string client_oid;
  std::string exchange_oid;
  std::string side;
  std::string entrust_time;
  double entrust_amount;
  double entrust_price;
  double dealt_amount;
  double average_dealt_price;
  double last_dealt_amount;
  std::string status;
  std::string update_time;
  std::string canceled_time;
  std::string closed_time;
  std::string ots_closed_time;
};

struct TransInfo {
  std::string account;
  std::string contract;
  std::string client_oid;
  std::string exchange_oid;
  std::string exchange_tid;
  std::string side;
  std::string dealt_time;
  double dealt_amount;
  double commission;
  std::string commission_currency;
};

struct Ticker {
  std::vector<std::pair<double, double>> bids;
  std::vector<std::pair<double, double>> asks;
  std::string contract;
  double last;
  double amount;
  double volume;
  std::string time;
  std::string exchange_time;

  std::string ToString() const {
    std::ostringstream ss;
    ss << "contract: " << contract << std::endl
       << "last: " << last << std::endl
       << "amount: " << amount << std::endl
       << "volume: " << volume << std::endl
       << "time: " << time << std::endl
       << "exchange_time: " << exchange_time << std::endl;
    return ss.str();
  }
};

struct Zhubi {
  std::string contract;
  double price;
  double amount;
  std::string side;
  std::string time;
  std::string exchange_time;

  std::string ToString() const {
    std::ostringstream ss;
    ss << "contract: " << contract << ", price: " << price
       << ", amount: " << amount << ", side: " << side << ", time: " << time
       << ", exchange_time: " << exchange_time << " }";
    return ss.str();
  }
};

struct Candle {
  std::string contract;
  double open;
  double close;
  double high;
  double low;
  double volume;
  double amount;
  std::string duration;
  std::string time;
};

typedef std::vector<Ticker> TickList;
typedef std::vector<Zhubi> ZhubiList;
typedef std::vector<Candle> CandleList;
typedef std::vector<ResponseOrderInfo> OrderList;
typedef std::vector<TransInfo> TransList;

struct MessageHeader {
  uint32_t version;
  uint64_t seq;
  ReqType req_type;
  RespType resp_type;
  uint32_t error_code;

  std::string ToString() const {
    std::ostringstream ss;
    ss << "version: " << version << std::endl
       << "seq: " << seq << std::endl
       << "req_type: " << req_type << std::endl
       << "resp_type: " << resp_type << std::endl
       << "error_code: " << error_code << std::endl;
    return ss.str();
  }
};

struct MarketResponseMessage {
  MessageHeader header;
  TickList tick_list;
  ZhubiList zhubi_list;
  CandleList candle_list;

  std::string ToString() const {
    std::ostringstream ss;
    ss << header.ToString() << std::endl;

    switch (header.resp_type) {
      case RESP_TICK:
        ss << "[" << std::endl;
        for (auto &tick : tick_list) {
          ss << tick.ToString() << "," << std::endl;
        }
        ss << "]" << std::endl;
        break;
        break;
      case RESP_ZHUBI:
        ss << "[" << std::endl;
        for (auto &zhubi : zhubi_list) {
          ss << zhubi.ToString() << "," << std::endl;
        }
        ss << "]" << std::endl;
        break;
      default:
        ss << "unsupported type" << std::endl;
    }
    return ss.str();
  }
};

struct TradeResponseMessage {
  MessageHeader header;
  TradeBaseInfo base_info;
  AccountInfo account_info;
  OrderList order_info;
  TransList trans_info;
};

struct ControlMessage {
  MessageHeader header;
  std::string info;

  std::string ToString() const {
    std::ostringstream ss;
    ss << header.ToString() << std::endl;
    ss << "info: " << info << std::endl;

    return ss.str();
  }
};
}  // namespace onetoken