## OneToken C++ SDK
#### Overview
 - Based on OneToken APIs: 
 - Currently support get tick/zhubi information using either REST or Websocket API

#### Information
  - Current development env�� Visual Studio 2017, x64_release��c++11
  - .h files��
    - onetoken_market_api.h��Market api for OneToken.
	- onetoken_interface.h��Define a interface class, used for callback. User should create a class that inherits from this class and implements the methods.
	- structs.h��Define some structures.
	- error_code.h��Define error codes.

  - directories��
    - lib��3rd-party lib files
    - common��3rd-party source and header files
    - example��a simple example to tell how to use the SDK

#### TODO List:
 - x86/Linux Compile Env
 - Candle API
 - Trade API
