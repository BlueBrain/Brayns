/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

#include <brayns/utils/EnumUtils.h>

/**
 * @brief The VasculatureSection enum list holds all the available vasculature section types
 */
enum class VasculatureSection
{
    VEIN,
    ARTERY,
    VENULE,
    ARTERIOLE,
    VENOUS_CAPILLARY,
    ARTERIAL_CAPILLARY,
    TRANSITIONAL
};

namespace brayns
{
template<>
inline std::vector<std::pair<std::string, VasculatureSection>> enumMap()
{
    return {
        {"vein", VasculatureSection::VEIN},
        {"artery", VasculatureSection::ARTERY},
        {"venule", VasculatureSection::VENULE},
        {"arteriole", VasculatureSection::ARTERIOLE},
        {"venous capillary", VasculatureSection::VENOUS_CAPILLARY},
        {"arterial capillary", VasculatureSection::ARTERIAL_CAPILLARY},
        {"transitional", VasculatureSection::TRANSITIONAL}};
}
} // namespace brayns
