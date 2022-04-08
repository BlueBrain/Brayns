#pragma once

#include <brayns/utils/EnumUtils.h>

/**
 * @brief Methods availables to color a vasculature circuit by (if corresponding data is available as well)
 */
enum class VasculatureColorMethods
{
    BY_SECTION
};

namespace brayns
{
template<>
inline std::vector<std::pair<std::string, VasculatureColorMethods>> enumMap()
{
    return {{"vasculature section", VasculatureColorMethods::BY_SECTION}};
}
}
