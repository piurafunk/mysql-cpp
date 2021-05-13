#include <MysqlCpp/helpers.hpp>

// template <typename T>
// std::ostream &operator<<(std::ostream &out, const std::vector<T> &vector)
// {
//     std::for_each(vector.begin(), vector.end(), [&out](T t) { out << t; });
//     return out;
// }

std::ostream &operator<<(std::ostream &out, const std::byte &b)
{
    out << (char)b;
    return out;
}

std::ostream &operator<<(std::ostream &out, const std::vector<std::byte> &vector)
{
    std::for_each(vector.begin(), vector.end(), [&out](std::byte b) { out << b; });
    return out;
}

namespace std
{
    std::vector<std::byte> to_vector(const std::string &string)
    {
        std::vector<std::byte> result(string.size());

        std::transform(string.begin(), string.end(), result.begin(), [](char c) { return std::byte(c); });

        return result;
    }
}
