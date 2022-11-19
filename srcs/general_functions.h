#ifndef GENERAL_FUNCTIONS_H
#define GENERAL_FUNCTIONS_H

#include <string>
#include <sstream>
#include <iomanip>

namespace Helpers{
std::string hexDecode(const std::string &data);
char hexToChar(char val);
std::string hexEncode(const char *data, size_t num_bytes, std::string leading);
}

#endif // GENERAL_FUNCTIONS_H
