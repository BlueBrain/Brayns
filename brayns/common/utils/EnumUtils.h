#pragma once

#include <string>
#include <tuple>
#include <vector>

namespace brayns
{
template <typename EnumT>
std::vector<std::pair<std::string, EnumT>> enumMap();

template <typename EnumT>
inline std::vector<std::string> enumNames()
{
    std::vector<std::string> v;
    for (const auto& p : enumMap<EnumT>())
        v.push_back(p.first);
    return v;
}

template <typename EnumT>
inline EnumT stringToEnum(const std::string& v)
{
    for (const auto& p : enumMap<EnumT>())
        if (p.first == v)
            return p.second;

    throw std::runtime_error("Could not match enum '" + v + "'");
    return static_cast<EnumT>(0);
}

template <typename EnumT>
inline std::string enumToString(const EnumT v)
{
    for (const auto& p : enumMap<EnumT>())
        if (p.second == v)
            return p.first;

    throw std::runtime_error("Could not match enum");
    return "Invalid";
}
}
