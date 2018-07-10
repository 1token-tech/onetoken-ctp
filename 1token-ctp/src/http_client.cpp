#include <thread>
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
        printf("Error connecting to server, error code: %d\n", connect_status);
        client->exit_flag_ = 1;
        client->req_callback_("");
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
        printf("Server closed connection\n");
        client->exit_flag_ = 1;
      };
      break;
    default:
      break;
  }
}

void HttpClient::SendReq(const std::string &url, ReqCallback req_callback) {
  req_callback_ = req_callback;
  mg_mgr mgr;
  mg_mgr_init(&mgr, this);
  auto connection = mg_connect_http(&mgr, OnHttpEvent, url.c_str(), NULL, NULL);
  mg_set_protocol_http_websocket(connection);

  while (exit_flag_ == 0) {
    mg_mgr_poll(&mgr, 500);
  }

  mg_mgr_free(&mgr);
}

}  // namespace onetoken