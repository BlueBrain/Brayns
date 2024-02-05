/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <cstdint>

/**
 * @brief The VasculatureSection enum list holds all the available vasculature
 * morphology sections, given also support to work with them as bitmask elements
 */
enum class VasculatureSection : uint8_t
{
    NONE = 0,
    VEIN = 1,
    ARTERY = 2,
    VENULE = 4,
    ARTERIOLE = 8,
    VENOUS_CAPILLARY = 16,
    ARTERIAL_CAPILLARY = 32,
    TRANSITIONAL = 64,
    ALL = VEIN | ARTERY | VENULE | ARTERIOLE | VENOUS_CAPILLARY | TRANSITIONAL
};

inline VasculatureSection operator&(const VasculatureSection a,
                                    const VasculatureSection b) noexcept
{
    return static_cast<VasculatureSection>(static_cast<uint8_t>(a) &
                                           static_cast<uint8_t>(b));
}

inline VasculatureSection operator&=(VasculatureSection& a,
                                     const VasculatureSection& b) noexcept
{
    a = a & b;
    return a;
}

inline VasculatureSection operator|(const VasculatureSection a,
                                    const VasculatureSection b) noexcept
{
    return static_cast<VasculatureSection>(static_cast<uint8_t>(a) |
                                           static_cast<uint8_t>(b));
}

inline VasculatureSection operator|=(VasculatureSection& a,
                                     const VasculatureSection& b) noexcept
{
    a = a | b;
    return a;
}
