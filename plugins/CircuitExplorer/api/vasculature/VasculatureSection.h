/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/utils/EnumInfo.h>

/**
 * @brief The VasculatureSection enum list holds all the available vasculature section types
 */
enum class VasculatureSection
{
    Vein,
    Artery,
    Venule,
    Arteriole,
    VenousCapillary,
    ArterialCapillary,
    Transitional
};

namespace brayns
{
template<>
struct EnumReflector<VasculatureSection>
{
    static EnumMap<VasculatureSection> reflect()
    {
        return {
            {"vein", VasculatureSection::Vein},
            {"artery", VasculatureSection::Artery},
            {"venule", VasculatureSection::Venule},
            {"arteriole", VasculatureSection::Arteriole},
            {"venous capillary", VasculatureSection::VenousCapillary},
            {"arterial capillary", VasculatureSection::ArterialCapillary},
            {"transitional", VasculatureSection::Transitional}};
    }
};
} // namespace brayns
