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

void Quote::HandleError(ErrorCode error_code, const std::string &info) {
  ControlMessage message;
  message.header.version = 1;
  message.header.req_type = req_type_;
  message.header.resp_type = RESP_ERROR;
  message.header.error_code = error_code;
  message.info = info;
  user_interface_->OnErrorResponse(&message);
}

}