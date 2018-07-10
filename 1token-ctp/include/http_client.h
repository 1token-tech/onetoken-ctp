#pragma once
#include <functional>
#include <string>
#include "mongoose.h"

namespace onetoken {
using ReqCallback = std::function<void(const std::string&)>;

class HttpClient {
 public:
  HttpClient() : exit_flag_(0) {}
  ~HttpClient() {}

  void SendReq(const std::string &url, ReqCallback req_callback);
  static void OnHttpEvent(mg_connection *connection, int event_type,
                          void *event_data);
  int exit_flag_;
  ReqCallback req_callback_;
};
}