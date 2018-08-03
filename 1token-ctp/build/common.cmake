cmake_minimum_required(VERSION 2.8)

# 32 bits or 64 bits
option(M64 "Generate 64 bits program" off)
if(M64)
  set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS}   -m64 -fPIC")
  set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} -m64 -fPIC")
else()
  set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS}   -m32 -march=i686")
  set(CMAKE_CXX_FLAGS"${CMAKE_CXX_FLAGS} -m32 -march=i686")
endif()     

# Set compiler flags
set(USER_FLAGS "-Wall -Wextra -Wformat=2 -Wno-unused-parameter -Wno-missing-field-initializers -Wmissing-include-dirs -Wfloat-equal -Wpointer-arith -Wwrite-strings -Wshadow -Woverloaded-virtual")
set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS}   -pipe ${USER_FLAGS}")
set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} -pipe ${USER_FLAGS}")

