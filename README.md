## OneToken C++ SDK
#### Overview
 - Based on OneToken APIs, please check http://1token.trade for detail
 - Currently support get tick/zhubi information using either REST or Websocket API
 - Cross-platform.

#### Information
  - Development env: Visual Studio 2017, x64_release, c++11
  - Dependences: zlib, openssl, websocketpp(with asio alone), cpprestsdk, rapidjson, gzip. Boost is needed on Linux
  - Some useful header files:
    - onetoken_market_api.h: Market api for OneToken.
	- onetoken_interface.h: Define a interface class, used for callback. User should create a class that inherits from this class and implements the methods.
	- structs.h: Define some structures.
	- error_code.h: Define error codes.

  - Directories: 
    - lib: 3rd-party libraries
    - example: a simple example to tell how to use the SDK

#### TODO List
 - x86/Linux Compile Env
 - Candle API
 - Trade API
