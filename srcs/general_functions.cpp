#include <srcs/general_functions.h>


std::string Helpers::hexDecode(const std::string &data)
{
    const char *data_c = data.c_str();
    std::string payload;

    for (size_t x = 0; x < data.length(); x+=2) {
      char char1 = hexToChar(data_c[x]);
      char char2 = hexToChar(data_c[x + 1]);

      u_char temp = (static_cast<u_char>(char1) * 16) + static_cast<u_char>(char2);
      payload.push_back(*reinterpret_cast<char*>(&temp));
    }

    return payload;
}

char Helpers::hexToChar(char val)
{
    char dec;

    if(val >= 65){
      val = static_cast<char>(std::toupper(val));
      dec = val - 65 + 10;
    }else{
      dec = val - 48;
    }

    return dec;
}

std::string Helpers::hexEncode(const char *data, size_t num_bytes, std::string leading)
{
    std::stringstream stream;

    const unsigned char *d = reinterpret_cast<const unsigned char*>(data);

    stream << leading;

    for (size_t x = 0; x < num_bytes; x++) {
      stream << std::setw(2) << std::setfill('0') << std::uppercase <<std::hex << static_cast<uint>(d[x]);
    }

    return stream.str();
}
