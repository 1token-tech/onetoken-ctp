#include <sstream>
#include "http_client.h"

namespace onetoken {

// 客户端的网络请求响应
void HttpClient::OnHttpEvent(mg_connection *connection, int event_type,
                             void *event_data) {
  http_message *hm = (struct http_message *)event_data;
  HttpClient *client = (HttpClient *)connection->mgr->user_data;
  int connect_status;

  switch (event_type) {
    case MG_EV_CONNECT:
      connect_status = *(int *)event_data;
      if (connect_status != 0) {
        client->exit_flag_ = 1;

        std::ostringstream ss;
        ss << "connect to " << client->url_
           << " failed, status: " << connect_status;
        client->err_callback_(CONNECT_FAILED, ss.str());
      }
      break;
    case MG_EV_HTTP_REPLY: {
      std::string rsp = std::string(hm->body.p, hm->body.len);
      connection->flags |= MG_F_SEND_AND_CLOSE;
      client->req_callback_(rsp);
      client->exit_flag_ = 1;
    } break;
    case MG_EV_CLOSE:
      if (client->exit_flag_ == 0) {
        client->exit_flag_ = 1;
        std::ostringstream ss;
        ss << "connection for " << client->url_ << " closed by server";
        client->err_callback_(SERVER_CLOSED, ss.str());
      };
      break;
    default:
      break;
  }
}

void HttpClient::SendReq(const std::string &url, ReqCallback req_callback) {
  req_callback_ = req_callback;
  url_ = url;

  mg_mgr mgr;
  mg_mgr_init(&mgr, this);
  url_ = url;
  auto connection = mg_connect_http(&mgr, OnHttpEvent, url.c_str(), NULL, NULL);
  mg_set_protocol_http_websocket(connection);

  while (exit_flag_ == 0) {
    mg_mgr_poll(&mgr, 500);
  }

  mg_mgr_free(&mgr);
}

}  // namespace onetoken