#pragma once
#include <string>

#ifdef _WIN32
#ifdef _CLASSINDLL
#define CLASSINDLL_CLASS_DECL __declspec(dllexport)
#else
#define CLASSINDLL_CLASS_DECL __declspec(dllimport)
#endif
#elif
#define CLASSINDLL_CLASS_DECL
#endif

namespace onetoken {
namespace utils {
void CharToHex(unsigned char c, unsigned char &hex1, unsigned char &hex2);
std::string BinaryToHex(const std::string &input);
std::string UrlEncode(std::string s);
std::string HmacSha256Encode(const std::string &key, const std::string &input);
}  // namespace utils
}  // namespace onetoken