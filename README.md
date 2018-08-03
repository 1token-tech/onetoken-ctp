## OneToken C++ SDK
#### Overview
 - Based on OneToken APIs, please check http://1token.trade for detail
 - Currently support get tick/zhubi information using either REST or Websocket API
 - Cross-platform.

#### Getting Started
  - Dependency Libraries: 
    - zlib
	- openssl
	- [websocketpp(with asio alone)](https://github.com/zaphoyd/websocketpp)
	- [cpprestsdk](https://github.com/Microsoft/cpprestsdk)
	- [rapidjson](https://github.com/Tencent/rapidjson)
	- [gzip](https://github.com/mapbox/gzip-hpp)
	
  - How to Build:
    - Windows: Visual Studio 2017 with x64_Release configuration
	- Linux: Be sure the dependency libraries are installed, and then build the project with CMake.
	  > mkdir build
	  > cd build
	  > cmake ../
	  > make
	Defaultly, dynamic library will be generated. If you want to use static library, add -DBUILD_SHARED_LIBS=0 when execute the cmake command `cmake ../ -DBUILD_SHARED_LIBS=0`
	(ps: libcpprest with apt-get version on Ubuntu has some problems, so we provide a built library in repository.
     
  - Interfaces:
    - onetoken_market_api.h: Market api for OneToken.
	- onetoken_interface.h: Define a interface class, used for callback. User should create a class that inherits from this class and implements the methods.
	- structs.h: Define some structures.
	- error_code.h: Define error codes.
	
  - How to Use:
    - check the header files in the *include* directory.
      > onetoken_market_api.h: Market api for OneToken.
	  > onetoken_interface.h: Define a interface class, used for callback. User should create a class that inherits from this class and implements the methods.
	  > structs.h: Define some structures.
	  > error_code.h: Define error codes.
	- Simple examples are provided in the *examples* directory to tell how to use the APIs.

#### TODO List
 - Candle API
 - Trade API
