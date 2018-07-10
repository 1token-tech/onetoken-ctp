## OneToken C++SDK
#### 功能概述
 - 支持行情订阅，tick/zhubi

#### API概述
  - 开发环境： Visual Studio 2017, x64，支持c++11
  - 接口文件：
    - onetoken_market_api.h：行情系统的相关API
	- onetoken_interface.h：回调函数，用户需实现一个接口类，该类需要继承UserInterface类，并实现相关虚函数接口。
	- structs.h：相关结构体定义
	- error_code.h：错误码的定义
  - lib文件：
    - onetoken_cpp_sdk.lib：所需依赖的静态库

  - 其他：
   - common目录：依赖的第三方库文件
   - example目录：示例代码

#### TODO List:
 - x86及Linux编译环境配置
 - Candle接口
 - 交易接口