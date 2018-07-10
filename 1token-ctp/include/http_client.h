#pragma once
#include <functional>
#include <string>
#include "error_code.h"
#include "mongoose.h"

namespace onetoken {
using ReqCallback = std::function<void(const std::string &)>;
using ErrCallback = std::function<void(ErrorCode, const std::string &)>;

class HttpClient {
 public:
  HttpClient() : exit_flag_(0) {}
  ~HttpClient() {}

  void SetErrorCallback(ErrCallback callback) { err_callback_ = callback; }

  void SendReq(const std::string &url, ReqCallback req_callback);
  static void OnHttpEvent(mg_connection *connection, int event_type,
                          void *event_data);
  int exit_flag_;
  std::string url_;
  ReqCallback req_callback_;
  ErrCallback err_callback_;
};
}  // namespace onetoken