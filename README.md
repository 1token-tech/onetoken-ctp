## OneToken C++SDK
#### Overview
 - Based on OneToken APIs: https://1token.trade
 - Currently support get tick/zhubi information using either REST or Websocket API

#### Instruction
  - Current development env�� Visual Studio 2017, x64_release��c++11��cross-platform 3rd-party libs.
  - .h interface files��
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
