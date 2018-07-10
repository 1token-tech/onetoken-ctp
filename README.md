## OneToken C++ SDK
#### Overview
 - Based on OneToken APIs: 
 - Currently support get tick/zhubi information using either REST or Websocket API

#### Information
  - Current development env£º Visual Studio 2017, x64_release£¬c++11
  - .h files£º
    - onetoken_market_api.h£ºMarket api for OneToken.
	- onetoken_interface.h£ºDefine a interface class, used for callback. User should create a class that inherits from this class and implements the methods.
	- structs.h£ºDefine some structures.
	- error_code.h£ºDefine error codes.

  - directories£º
    - lib£º3rd-party lib files
    - common£º3rd-party source and header files
    - example£ºa simple example to tell how to use the SDK

#### TODO List:
 - x86/Linux Compile Env
 - Candle API
 - Trade API
