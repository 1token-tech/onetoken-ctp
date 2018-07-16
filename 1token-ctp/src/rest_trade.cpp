#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rest_trade.h"
#include "utils.h"
#include "openssl_lock.h"

namespace onetoken {
void RestTrade::Init(const std::string &ot_key, const std::string &ot_secret) {
  user_info_.ot_key = ot_key;
  user_info_.ot_secret = ot_secret;
  SSLLock::ThreadSafetySetup();
}

void RestTrade::GetAccountInfo(const std::string &exchange,
                               const std::string &account_name) {
  srand(time(NULL));
  std::string verb = "GET";
  std::string nonce = std::to_string(rand());
  std::string path = "/";
  path += utils::UrlEncode(exchange);
  path += "/";
  path += utils::UrlEncode(account_name);
  path += "/info";

  std::string base = verb;
  base += path;
  base += nonce;

  std::string sign =
      utils::BinaryToHex(utils::HmacSha256Encode(user_info_.ot_secret, base));

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
    auto response = raw_client.request(request).get();
    concurrency::streams::stringstreambuf buf;
    response.body().read_to_end(buf).wait();
    std::cout << buf.collection() << std::endl;
  } catch (...) {
    HandleError(CONNECT_FAILED, REQ_ACCOUNT_INFO, "get account info failed");
  }
}

void RestTrade::GetOrders(const std::string &exchange,
                          const std::string &account_name,
                          const std::string &contract,
                          const std::string &state,
                          const std::vector<RequestOrderInfo> *order_info) {
  srand(time(NULL));
  std::string verb = "GET";
  std::string nonce = std::to_string(rand());
  std::string path = "/";
  path += utils::UrlEncode(exchange);
  path += "/";
  path += utils::UrlEncode(account_name);
  path += "/orders";

  std::string base = verb;
  base += path;
  base += nonce;
  std::string sign =
      utils::BinaryToHex(utils::HmacSha256Encode(user_info_.ot_secret, base));

  auto oids = [&]() -> std::pair<std::string, std::string> {
    std::string client_oids;
    std::string exchange_oids;
    bool first = true;
    int client_oid_count = 0;
    int exchange_oid_count = 0;
    if (order_info != NULL) {
      for (auto const &order : *order_info) {
        if (first) {
          first = false;
        } else {
          client_oids += ",";
          exchange_oids += ",";
        }
        if (!order.client_oid.empty() && client_oid_count < 9) {
          client_oids += order.client_oid;
          client_oid_count++;
        }
        if (!order.exchange_oid.empty() && exchange_oid_count < 9) {
          exchange_oids += order.exchange_oid;
          exchange_oid_count++;
        }
      }
    }
    return std::make_pair(client_oids, exchange_oids);
  }();

  std::string params = "?";
  if (!oids.first.empty()) {
    params += "client_oid=";
    params += oids.first;
    params += "&";
  }
  if (!oids.second.empty()) {
    params += "exchange_oid=";
    params += oids.second;
    params += "&";
  }
  if (!contract.empty()) {
    params += "contract=";
    params += contract;
    params += "&";
  }
  if (!state.empty()) {
    params += "state=";
    params += state;
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
    std::wcout << request.to_string() << std::endl;
    auto response = raw_client.request(request).get();
    concurrency::streams::stringstreambuf body;
    response.body().read_to_end(body).wait();
    std::cout << body.collection() << std::endl;
  } catch (...) {
    HandleError(CONNECT_FAILED, REQ_ACCOUNT_INFO, "get order info failed");
  }
}

void RestTrade::InsertOrder(const std::string &exchange,
                            const std::string &account_name,
                            const RequestOrderInfo *order_info) {
  std::cout << "aaaa" << std::endl;
  srand(time(NULL));
  std::string verb = "POST";
  std::string nonce = std::to_string(rand());
  std::string path = "/";
  path += utils::UrlEncode(exchange);
  path += "/";
  path += utils::UrlEncode(account_name);
  path += "/orders";

  std::string base = verb;
  base += path;
  base += nonce;

  std::cout << "bbbb" << std::endl;
  rapidjson::Document document;
  auto &allocator = document.GetAllocator();
  rapidjson::Value root(rapidjson::kObjectType);

  root.AddMember("contract", order_info->contract, allocator);
  root.AddMember("bs", order_info->bs, allocator);
  root.AddMember("price", order_info->price, allocator);
  root.AddMember("amount", order_info->amount, allocator);
  auto client_oid = order_info->client_oid;
  if (order_info->client_oid.empty()) {
    client_oid = utils::GenerateRandomId(order_info->contract);
  }
  root.AddMember("client_oid", client_oid, allocator);
  rapidjson::StringBuffer buf;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
  root.Accept(writer);
  auto body = buf.GetString();
  base += body;

  std::cout << "cccc" << std::endl;
  std::string sign =
      utils::BinaryToHex(utils::HmacSha256Encode(user_info_.ot_secret, base));

  std::cout << "dddd" << std::endl;
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
    std::wcout << request.to_string() << std::endl;
    auto response = raw_client.request(request).get();
    concurrency::streams::stringstreambuf body;
    response.body().read_to_end(body).wait();
    std::cout << body.collection() << std::endl;
  } catch (...) {
    HandleError(CONNECT_FAILED, REQ_ACCOUNT_INFO, "insert order failed");
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