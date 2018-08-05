#include <algorithm>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rest_trade.h"
#include "utils.h"

namespace onetoken {

void SRand() {
#ifdef _WIN32
  SYSTEMTIME time;
  GetSystemTime(&time);
  long time_ms = time.wSecond * 1000 + time.wMilliseconds;
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  long time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
  srand(time_ms);
}

void RestTrade::Init(const std::string &ot_key, const std::string &ot_secret) {
  user_info_.ot_key = ot_key;
  user_info_.ot_secret = ot_secret;
}

void RestTrade::GetAccountInfo(const TradeBaseInfo &base_info) {
  SRand();
  std::string verb = "GET";
  std::string nonce = std::to_string(rand());
  std::string path = "/";
  path += utils::UrlEncode(base_info.exchange);
  path += "/";
  path += utils::UrlEncode(base_info.account_name);
  path += "/info";

  std::string base = verb;
  base += path;
  base += nonce;
  std::string sign = utils::HmacSha256Encode(user_info_.ot_secret, base);

  try {
    std::string url = base_url_;
    url += path;
    web::http::client::http_client_config config;
    config.set_timeout(utility::seconds(5));
    web::http::client::http_client raw_client(
        utility::conversions::to_string_t(url), config);
    web::http::http_request request(web::http::methods::GET);
    request.headers().add(U("Content-Type"), U("application/json"));
    request.headers().add(U("Api-Nonce"),
                          utility::conversions::to_string_t(nonce));
    request.headers().add(U("Api-Key"),
                          utility::conversions::to_string_t(user_info_.ot_key));
    request.headers().add(U("Api-Signature"),
                          utility::conversions::to_string_t(sign));
    raw_client.request(request)
        .then(std::bind(&RestTrade::ParseResponse, this, REQ_ACCOUNT_INFO,
                        base_info, std::placeholders::_1))
        .wait();
  } catch (...) {
    HandleError(CONNECT_FAILED, REQ_ACCOUNT_INFO, "get account info failed");
  }
}

void RestTrade::GetOrders(const TradeBaseInfo &base_info,
                          const std::vector<RequestOrderInfo> &order_info,
                          bool is_client_oid) {
  SRand();
  std::string verb = "GET";
  std::string nonce = std::to_string(rand());
  std::string path = "/";
  path += utils::UrlEncode(base_info.exchange);
  path += "/";
  path += utils::UrlEncode(base_info.account_name);
  path += "/orders";

  std::string base = verb;
  base += path;
  base += nonce;
  std::string sign = utils::HmacSha256Encode(user_info_.ot_secret, base);

  std::string oids;
  bool first = true;
  for (auto const &order : order_info) {
    if (first) {
      first = false;
    } else {
      oids += ",";
    }

    if (is_client_oid && !order.client_oid.empty()) {
      oids += order.client_oid;
    } else if (!is_client_oid && !order.exchange_oid.empty()) {
      oids += order.exchange_oid;
    }
  }

  std::string params = "?";
  if (!oids.empty()) {
    if (is_client_oid) {
      params += "client_oid=";
    } else {
      params += "exchange_oid=";
    }
    params += utils::UrlEncode(oids);
    params += "&";
  }
  if (!base_info.contract.empty()) {
    params += "contract=";
    params += utils::UrlEncode(base_info.contract);
    params += "&";
  }
  if (!base_info.state.empty()) {
    params += "state=";
    params += utils::UrlEncode(base_info.state);
    params += "&";
  }

  try {
    std::string url = base_url_;
    url += path;
    url += params;

    web::http::client::http_client_config config;
    config.set_timeout(utility::seconds(5));
    web::http::client::http_client raw_client(
        utility::conversions::to_string_t(url), config);
    web::http::http_request request(web::http::methods::GET);
    request.headers().add(U("Content-Type"), U("application/json"));
    request.headers().add(U("Api-Nonce"),
                          utility::conversions::to_string_t(nonce));
    request.headers().add(U("Api-Key"),
                          utility::conversions::to_string_t(user_info_.ot_key));
    request.headers().add(U("Api-Signature"),
                          utility::conversions::to_string_t(sign));
    raw_client.request(request)
        .then(std::bind(&RestTrade::ParseResponse, this, REQ_GET_ORDERS,
                        base_info, std::placeholders::_1))
        .wait();
  } catch (...) {
    HandleError(CONNECT_FAILED, REQ_GET_ORDERS, "get order info failed");
  }
}

void RestTrade::PlaceOrder(const TradeBaseInfo &base_info,
                           const RequestOrderInfo &order_info) {
  SRand();
  std::string verb = "POST";
  std::string nonce = std::to_string(rand());
  std::string path = "/";
  path += utils::UrlEncode(base_info.exchange);
  path += "/";
  path += utils::UrlEncode(base_info.account_name);
  path += "/orders";

  std::string base = verb;
  base += path;
  base += nonce;

  rapidjson::Document document;
  auto &allocator = document.GetAllocator();
  rapidjson::Value root(rapidjson::kObjectType);

  root.AddMember("contract", rapidjson::StringRef(order_info.contract.c_str()), allocator);
  root.AddMember("bs", rapidjson::StringRef(order_info.bs.c_str()), allocator);
  root.AddMember("price", order_info.price, allocator);
  root.AddMember("amount", order_info.amount, allocator);
  auto client_oid = order_info.client_oid;
  if (order_info.client_oid.empty()) {
    client_oid = utils::GenerateRandomId(order_info.contract);
  }
  root.AddMember("client_oid", rapidjson::StringRef(client_oid.c_str()), allocator);
  rapidjson::StringBuffer buf;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
  root.Accept(writer);
  auto body = buf.GetString();
  base += body;
  std::string sign = utils::HmacSha256Encode(user_info_.ot_secret, base);

  try {
    std::string url = base_url_;
    url += path;
    web::http::client::http_client_config config;
    config.set_timeout(utility::seconds(5));
    web::http::client::http_client raw_client(
        utility::conversions::to_string_t(url), config);
    web::http::http_request request(web::http::methods::POST);
    request.headers().add(U("Content-Type"), U("application/json"));
    request.headers().add(U("Api-Nonce"),
                          utility::conversions::to_string_t(nonce));
    request.headers().add(U("Api-Key"),
                          utility::conversions::to_string_t(user_info_.ot_key));
    request.headers().add(U("Api-Signature"),
                          utility::conversions::to_string_t(sign));
    request.set_body(body);
    raw_client.request(request)
        .then(std::bind(&RestTrade::ParseResponse, this, REQ_PLACE_ORDER,
                        base_info, std::placeholders::_1))
        .wait();
  } catch (...) {
    HandleError(CONNECT_FAILED, REQ_PLACE_ORDER, "place new order failed");
  }
}

void RestTrade::CancelOrder(const TradeBaseInfo &base_info,
                            const std::vector<RequestOrderInfo> &order_info,
                            bool is_client_oid) {
  SRand();
  std::string verb = "DELETE";
  std::string nonce = std::to_string(rand());
  std::string path = "/";
  path += utils::UrlEncode(base_info.exchange);
  path += "/";
  path += utils::UrlEncode(base_info.account_name);
  path += "/orders";

  std::string base = verb;
  base += path;
  base += nonce;
  std::string sign = utils::HmacSha256Encode(user_info_.ot_secret, base);

  std::string oids;
  bool first = true;
  for (auto const &order : order_info) {
    if (first) {
      first = false;
    } else {
      oids += ",";
    }

    if (is_client_oid && !order.client_oid.empty()) {
      oids += order.client_oid;
    } else if (!is_client_oid && !order.exchange_oid.empty()) {
      oids += order.exchange_oid;
    }
  }

  std::string params = "?";
  if (!oids.empty()) {
    if (is_client_oid) {
      params += "client_oid=";
    } else {
      params += "exchange_oid=";
    }
    params += utils::UrlEncode(oids);
    params += "&";
  }
  if (!base_info.contract.empty()) {
    params += "contract=";
    params += utils::UrlEncode(base_info.contract);
    params += "&";
  }
  if (!base_info.state.empty()) {
    params += "state=";
    params += utils::UrlEncode(base_info.state);
    params += "&";
  }

  try {
    std::string url = base_url_;
    url += path;
    url += params;
    web::http::client::http_client_config config;
    config.set_timeout(utility::seconds(5));
    web::http::client::http_client raw_client(
        utility::conversions::to_string_t(url), config);
    web::http::http_request request(web::http::methods::DEL);
    request.headers().add(U("Content-Type"), U("application/json"));
    request.headers().add(U("Api-Nonce"),
                          utility::conversions::to_string_t(nonce));
    request.headers().add(U("Api-Key"),
                          utility::conversions::to_string_t(user_info_.ot_key));
    request.headers().add(U("Api-Signature"),
                          utility::conversions::to_string_t(sign));
    raw_client.request(request)
        .then(std::bind(&RestTrade::ParseResponse, this, REQ_CANCEL_ORDER,
                        base_info, std::placeholders::_1))
        .wait();
  } catch (...) {
    HandleError(CONNECT_FAILED, REQ_CANCEL_ORDER, "cancel order failed");
  }
}

void RestTrade::CancelAllOrders(const TradeBaseInfo &base_info) {
  SRand();
  std::string verb = "DELETE";
  std::string nonce = std::to_string(rand());
  std::string path = "/";
  path += utils::UrlEncode(base_info.exchange);
  path += "/";
  path += utils::UrlEncode(base_info.account_name);
  path += "/orders/all";

  std::string base = verb;
  base += path;
  base += nonce;
  std::string sign = utils::HmacSha256Encode(user_info_.ot_secret, base);

  std::string params = "?";
  if (!base_info.contract.empty()) {
    params += "contract=";
    params += utils::UrlEncode(base_info.contract);
    params += "&";
  }
  try {
    std::string url = base_url_;
    url += path;
    url += params;
    web::http::client::http_client_config config;
    config.set_timeout(utility::seconds(5));
    web::http::client::http_client raw_client(
        utility::conversions::to_string_t(url), config);
    web::http::http_request request(web::http::methods::DEL);
    request.headers().add(U("Content-Type"), U("application/json"));
    request.headers().add(U("Api-Nonce"),
                          utility::conversions::to_string_t(nonce));
    request.headers().add(U("Api-Key"),
                          utility::conversions::to_string_t(user_info_.ot_key));
    request.headers().add(U("Api-Signature"),
                          utility::conversions::to_string_t(sign));

    auto response = raw_client.request(request).get();
    concurrency::streams::stringstreambuf body;
    response.body().read_to_end(body).wait();
    std::cout << body.collection() << std::endl;
  } catch (...) {
    HandleError(CONNECT_FAILED, REQ_CANCEL_ORDER, "cancel all order failed");
  }
}

void RestTrade::ParseResponse(ReqType type, const TradeBaseInfo &base_info,
                              web::http::http_response response) {
  try {
    concurrency::streams::stringstreambuf buf;
    response.body().read_to_end(buf).wait();
    auto body = buf.collection();

    rapidjson::Document doc;
    if (doc.Parse(body.c_str()).HasParseError()) {
      HandleError(RESP_MESSAGE_FORMAT_ERROR, type, "parse resp data failed.");
      return;
    }

    // error message
    if (doc.HasMember("code")) {
      HandleError(RESP_FAILED_MESSAGE, type, body);
      return;
    }

    TradeResponseMessage message;
    message.base_info = base_info;
    if (type == REQ_ACCOUNT_INFO) {
      AccountInfo account;
      account.balance = doc["balance"].GetDouble();
      account.cash = doc["cash"].GetDouble();
      account.market_value = doc["market_value"].GetDouble();
      auto detail = doc["market_value_detail"].GetObject();
      for (auto member = detail.MemberBegin(); member != detail.MemberEnd();
           ++member) {
        account.market_value_detail.emplace(member->name.GetString(),
                                            member->value.GetDouble());
      }
      auto positions = doc["position"].GetArray();
      for (auto const &position : positions) {
        AccountInfo::Position pos;
        pos.contract = position["contract"].GetString();
        pos.total_amount = position["total_amount"].GetDouble();
        pos.available = position["available"].GetDouble();
        pos.frozen = position["frozen"].GetDouble();
        pos.market_value = position["market_value"].GetDouble();
        pos.value_cny = position["value_cny"].GetDouble();
        pos.type = position["type"].GetString();
        account.positions.push_back(pos);
      }

      message.header.version = 1;
      message.header.req_type = type;
      message.header.seq = seq_++;
      message.header.resp_type = RESP_ACCOUNT;
      message.header.error_code = SUCCESS;
      user_interface_->OnGetAccountInfo(&message);
      return;
    } else if (type == REQ_GET_ORDERS) {
      auto order_list = doc.GetArray();
      for (auto &order : order_list) {
        ResponseOrderInfo order_info;
        if (order.HasMember("account")) {
          order_info.account = order["account"].GetString();
        }
        if (order.HasMember("contract")) {
          order_info.contract = order["contract"].GetString();
        }
        if (order.HasMember("client_oid")) {
          order_info.client_oid = order["client_oid"].GetString();
        }
        if (order.HasMember("exchange_oid")) {
          order_info.exchange_oid = order["exchange_oid"].GetString();
        }
        if (order.HasMember("entrust_amount")) {
          order_info.entrust_amount = order["entrust_amount"].GetDouble();
        }
        if (order.HasMember("entrust_price")) {
          order_info.entrust_price = order["entrust_price"].GetDouble();
        }
        if (order.HasMember("dealt_amount")) {
          order_info.dealt_amount = order["dealt_amount"].GetDouble();
        }
        if (order.HasMember("average_dealt_price")) {
          order_info.average_dealt_price =
              order["average_dealt_price"].GetDouble();
        }
        if (order.HasMember("last_dealt_amount")) {
          if (order["last_dealt_amount"].IsNull()) {
            order_info.last_dealt_amount = 0.0;
          } else {
            order_info.last_dealt_amount =
                order["last_dealt_amount"].GetDouble();
          }
        }
        if (order.HasMember("last_update")) {
          order_info.update_time = order["last_update"].GetString();
        }
        if (order.HasMember("canceled_time")) {
          if (order["canceled_time"].IsNull()) {
            order_info.canceled_time = "";
          } else {
            order_info.canceled_time = order["canceled_time"].GetString();
          }
        }
        if (order.HasMember("closed_time")) {
          if (order["closed_time"].IsNull()) {
            order_info.closed_time = "";
          } else {
            order_info.closed_time = order["closed_time"].GetString();
          }
        }
        if (order.HasMember("ots_closed_time")) {
          if (order["ots_closed_time"].IsNull()) {
            order_info.ots_closed_time = "";
          } else {
            order_info.ots_closed_time = order["ots_closed_time"].GetString();
          }
        }
        if (order.HasMember("status")) {
          order_info.status = order["status"].GetString();
        }
        message.order_info.push_back(order_info);
      }
      message.header.version = 1;
      message.header.req_type = type;
      message.header.seq = seq_++;
      message.header.resp_type = RESP_ORDER;
      message.header.error_code = SUCCESS;
      user_interface_->OnGetOrders(&message);
      return;
    } else if (type == REQ_PLACE_ORDER) {
      ResponseOrderInfo order_info;
      if (doc.HasMember("client_oid")) {
        order_info.client_oid = doc["client_oid"].GetString();
      }
      if (doc.HasMember("exchange_oid")) {
        order_info.exchange_oid = doc["exchange_oid"].GetString();
      }
      message.order_info.push_back(order_info);
      message.header.version = 1;
      message.header.req_type = type;
      message.header.seq = seq_++;
      message.header.resp_type = RESP_ORDER;
      message.header.error_code = SUCCESS;
      user_interface_->OnPlaceOrder(&message);
      return;
    } else if (type == REQ_CANCEL_ORDER) {
      auto order_list = doc.GetArray();
      for (auto &order : order_list) {
        ResponseOrderInfo tmp;
        if (order.HasMember("client_oid")) {
          tmp.client_oid = order["client_oid"].GetString();
        }
        if (order.HasMember("exchange_oid")) {
          tmp.exchange_oid = order["exchange_oid"].GetString();
        }
        message.order_info.push_back(tmp);
      }

      message.header.version = 1;
      message.header.req_type = type;
      message.header.seq = seq_++;
      message.header.resp_type = RESP_ORDER;
      message.header.error_code = SUCCESS;
      user_interface_->OnCancelOrder(&message);
      return;
    }
  } catch (...) {
    HandleError(RESP_MESSAGE_FORMAT_ERROR, type, "parse resp data failed.");
    return;
  }
}

void RestTrade::HandleError(ErrorCode error_code, ReqType type,
                            const std::string &info) {
  ControlMessage message;
  message.header.version = 1;
  message.header.req_type = type;
  message.header.resp_type = RESP_ERROR;
  message.header.error_code = error_code;
  message.info = info;
  user_interface_->OnErrorResponse(&message);
}

}  // namespace onetoken
