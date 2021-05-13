#pragma once

#include <memory>
#include <type_traits>
#include <ostream>
#include <string>
#include <iomanip>
#include <vector>

// template <typename Base, typename T>
// inline bool instanceOf(const T *t)
// {
//     return dynamic_cast<const Base *>(t) != nullptr;
// }

template <typename Base, typename T>
inline bool instanceOf(const std::shared_ptr<T> t)
{
    return (bool)std::dynamic_pointer_cast<Base>(t);
}

struct HexString
{
    HexString(std::string string) : HexString((char *)string.c_str(), string.size()) {}

    HexString(char *string) : HexString(string, std::char_traits<char>::length(string)) {}

    HexString(char *string, unsigned int length)
    {
        this->string = (unsigned char *)string;
        this->length = length;
    }

    HexString(const char *string)
    {
        this->string = (unsigned char *)string;
        this->length = std::char_traits<unsigned char>::length(this->string);
    }

    HexString(unsigned char *string)
    {
        this->string = string;
        this->length = std::char_traits<unsigned char>::length(this->string);
    }

    HexString(unsigned char *string, unsigned int length)
    {
        this->string = string;
        this->length = length;
    }

    unsigned char *string;
    unsigned int length;
};

inline std::ostream &operator<<(std::ostream &out, const HexString &hexString)
{
    for (unsigned int j = 0; j < hexString.length; j++)
    {
        out << std::hex << std::setw(2) << std::setfill('0') << (unsigned int)(hexString.string[j]);
    }

    return out;
}

std::ostream &operator<<(std::ostream &, const std::byte &);
std::ostream &operator<<(std::ostream &, const std::vector<std::byte> &);

namespace std
{
    std::vector<std::byte> to_vector(const std::string &);
}
