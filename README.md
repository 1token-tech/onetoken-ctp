# OneToken C++ SDK
## Overview
 - Based on OneToken APIs, please check http://1token.trade for detail
 - Currently support get tick/zhubi information using either REST or Websocket API
 - Cross-platform.

## Getting Started
  - #### Dependency Libraries: 
    - zlib
	- openssl
	- [websocketpp(with asio alone)](https://github.com/zaphoyd/websocketpp)
	- [cpprestsdk](https://github.com/Microsoft/cpprestsdk)
	- [rapidjson](https://github.com/Tencent/rapidjson)
	- [gzip](https://github.com/mapbox/gzip-hpp)
	
  - #### How to Build:
    This project requires C++11 support, be sure the compiler has supported C++11 standard.
    - Windows: Visual Studio 2017 with x64_Release configuration
	- Linux: Be sure the dependency libraries are installed, and then build the project with CMake.
      ```
	  mkdir build
	  cd build
	  cmake ../
	  make
      ```
	  
	By default, static library will be generated. If you want to build dynamic library, add -DBUILD_SHARED_LIBS=1 when execute the *cmake* command `cmake ../ -DBUILD_SHARED_LIBS=1`
	
	> ps: libcpprest with apt-get version on Ubuntu has some problems, so we provide a built library in repository.
	
  - #### How to Use:
  
    Check the header files in the *include* directory.
      - onetoken_market_api.h: Market api for OneToken.
	  - onetoken_interface.h: Define a interface class, used for callback. User should create a class that inherits from this class and implements the methods.
	  - structs.h: Define some structures.
	  - error_code.h: Define error codes.
	  
	Simple examples are provided in the *examples* directory to tell how to use the APIs.
	
  - #### Platform Support
	Has been tested on Windows10 with Visual Studio 2017 & Ubuntu on Windows.


## TODO List
 - Candle API
 - More Trade API
