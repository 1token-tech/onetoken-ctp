#include "quote.h"

namespace onetoken {
ErrorCode Quote::ParseTickData(const rapidjson::Value &data,
                                   MarketResponseMessage &message) {
  if (!data.IsObject()) {
    return RESP_MESSAGE_FORMAT_ERROR;
  }

  std::string contract;
  Ticker ticker;
  if (data.HasMember("contract")) {
    contract = data["contract"].GetString();
    ticker.contract = contract;
  } else {
    return RESP_MESSAGE_FORMAT_ERROR;
  }

  if (data.HasMember("last")) {
    ticker.last = data["last"].GetDouble();
  } else {
    return RESP_MESSAGE_FORMAT_ERROR;
  }

  if (data.HasMember("amount")) {
    ticker.amount = data["amount"].GetDouble();
  } else {
    return RESP_MESSAGE_FORMAT_ERROR;
  }

  if (data.HasMember("volume")) {
    ticker.volume = data["volume"].GetDouble();
  } else {
    return RESP_MESSAGE_FORMAT_ERROR;
  }

  if (data.HasMember("time")) {
    ticker.time = data["time"].GetString();
  } else {
    return RESP_MESSAGE_FORMAT_ERROR;
  }

  if (data.HasMember("exchange_time")) {
    ticker.exchange_time = data["exchange_time"].GetString();
  }

  if (data.HasMember("asks")) {
    for (auto &iter : data["asks"].GetArray()) {
      auto ask = iter.GetObject();
      auto price = ask["price"].GetDouble();
      auto volume = ask["volume"].GetDouble();
      ticker.asks.push_back(std::make_pair(price, volume));
    }
  }

  if (data.HasMember("bids")) {
    for (auto &iter : data["bids"].GetArray()) {
      auto bid = iter.GetObject();
      auto price = bid["price"].GetDouble();
      auto volume = bid["volume"].GetDouble();
      ticker.bids.push_back(std::make_pair(price, volume));
    }
  }

  message.tick_list.push_back(ticker);

  return SUCCESS;
}

ErrorCode Quote::ParseZhubiData(const rapidjson::Value &data,
                                  MarketResponseMessage &message) {
  if (!data.IsArray()) {
    return RESP_MESSAGE_FORMAT_ERROR;
  }

  for (auto const &zb_data : data.GetArray()) {
    Zhubi zhubi;
    std::string contract;
    if (zb_data.HasMember("contract")) {
      contract = zb_data["contract"].GetString();
      zhubi.contract = contract;
    } else {
      return RESP_MESSAGE_FORMAT_ERROR;
    }

    if (zb_data.HasMember("amount")) {
      zhubi.amount = zb_data["amount"].GetDouble();
    } else {
      return RESP_MESSAGE_FORMAT_ERROR;
    }

    if (zb_data.HasMember("price")) {
      zhubi.price = zb_data["price"].GetDouble();
    } else {
      return RESP_MESSAGE_FORMAT_ERROR;
    }

    if (zb_data.HasMember("bs")) {
      zhubi.side = zb_data["bs"].GetString();
    } else {
      return RESP_MESSAGE_FORMAT_ERROR;
    }

    if (zb_data.HasMember("time")) {
      zhubi.time = zb_data["time"].GetString();
    } else {
      return RESP_MESSAGE_FORMAT_ERROR;
    }

    if (zb_data.HasMember("exchange_time")) {
      zhubi.exchange_time = zb_data["exchange_time"].GetString();
    }

    message.zhubi_list.push_back(zhubi);
  }

  return SUCCESS;
}

void ToHex(unsigned char c, unsigned char &hex1, unsigned char &hex2) {
  hex1 = c / 16;
  hex2 = c % 16;
  hex1 += hex1 <= 9 ? '0' : 'a' - 10;
  hex2 += hex2 <= 9 ? '0' : 'a' - 10;
}

std::string Quote::UrlEncode(std::string s) {
  const char *str = s.c_str();
  std::vector<char> v(s.size());
  v.clear();
  for (size_t i = 0, l = s.size(); i < l; i++) {
    char c = str[i];
    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z') || c == '-' || c == '_' || c == '.' ||
        c == '!' || c == '*' || c == '(' || c == ')') {
      v.push_back(c);
    } else if (c == ' ') {
      v.push_back('+');
    } else {
      v.push_back('%');
      unsigned char d1, d2;
      ToHex(c, d1, d2);
      v.push_back(d1);
      v.push_back(d2);
    }
  }
  return std::string(v.cbegin(), v.cend());
}

void Quote::HandleError(ErrorCode error_code, const std::string &info) {
  ErrorMessage message;
  message.header.version = 1;
  message.header.req_type = REQ_REST;
  message.header.resp_type = RESP_ERROR;
  message.header.error_code = error_code;
  message.info = info;
  user_interface_->OnErrorResponse(&message);
}

}