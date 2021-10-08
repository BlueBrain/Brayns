#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

namespace
{
/* Returns a mapping from a name to an enum type. */
template <typename EnumT>
std::vector<std::pair<std::string, EnumT>> enumerateMap();

/* Convert a string to an enum. */
template <typename EnumT>
inline EnumT stringToEnum(const std::string& v)
{
    for (const auto& p : enumerateMap<EnumT>())
        if (p.first == v)
            return p.second;

    throw std::runtime_error("Could not match enum '" + v + "'");
    return static_cast<EnumT>(0);
}

/* Returns all names for given enum type 'EnumT' */
template <typename EnumT>
inline std::vector<std::string> enumerateNames()
{
    std::vector<std::string> v;
    for (const auto& p : enumerateMap<EnumT>())
        v.push_back(p.first);
    return v;
}

/* Convert an enum to a string. */
template <typename EnumT>
inline std::string enumToString(const EnumT v)
{
    for (const auto& p : enumerateMap<EnumT>())
        if (p.second == v)
            return p.first;

    throw std::runtime_error("Could not match enum");
    return "Invalid";
}
} // namespace

#endif // UTILS_H
