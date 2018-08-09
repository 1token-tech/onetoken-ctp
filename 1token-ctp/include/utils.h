#pragma once
#include <string>

namespace onetoken {
namespace utils {
void CharToHex(unsigned char c, unsigned char &hex1, unsigned char &hex2);
std::string BinaryToHex(const std::string &input);
std::string UrlEncode(std::string s);
std::string HmacSha256Encode(const std::string &key, const std::string &input);
std::string GenerateRandomId(const std::string &base);
}  // namespace utils
}  // namespace onetoken
