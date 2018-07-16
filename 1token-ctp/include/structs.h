#pragma once
#include <iostream>
#include <sstream>
#include <vector>

namespace onetoken {
enum ReqType {
  REQ_REST,
  REQ_WS_TICK_AND_ZHUBI,
  REQ_WS_CANDLE,
  REQ_ACCOUNT_INFO,
  REQ_GET_ORDERS,
  REQ_CREATE_ORDER,
  REQ_CANCEL_ORDER,
  REQ_GET_TRANS
};
enum RespType { RESP_TICK, RESP_ZHUBI, RESP_ERROR };

struct MessageHeader {
  uint32_t version;
  uint32_t seq;
  ReqType req_type;
  RespType resp_type;
  uint32_t error_code;

  // TODO: delete later
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

struct Ticker {
  std::vector<std::pair<double, double>> bids;
  std::vector<std::pair<double, double>> asks;
  std::string contract;
  double last;
  double amount;
  double volume;
  std::string time;
  std::string exchange_time;

  // TODO: delete later
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

  // TODO: delete later
  std::string ToString() const {
    std::ostringstream ss;
    ss << "contract: " << contract << ", price: " << price
       << ", amount: " << amount << ", side: " << side << ", time: " << time
       << ", exchange_time: " << exchange_time << " }";
    return ss.str();
  }
};

typedef std::vector<Ticker> TickList;
typedef std::vector<Zhubi> ZhubiList;

struct MarketResponseMessage {
  MessageHeader header;
  TickList tick_list;
  ZhubiList zhubi_list;

  // TODO: delete later
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
    }
    return ss.str();
  }
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