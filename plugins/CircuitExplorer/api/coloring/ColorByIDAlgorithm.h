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

#include <brayns/common/MathTypes.h>

#include <functional>
#include <map>
#include <vector>

/**
 * @brief Utility to iterate over the elements specified ona color by id request
 */
class ColorByIDAlgorithm
{
public:
    static std::vector<uint64_t> execute(
        const std::map<uint64_t, brayns::Vector4f> &colorMap,
        const std::vector<uint64_t> &ids,
        const std::function<void(uint64_t, size_t, const brayns::Vector4f &)> &elementCallback);

private:
    static std::vector<uint64_t> _withInput(
        const std::map<uint64_t, brayns::Vector4f> &colorMap,
        const std::vector<uint64_t> &ids,
        const std::function<void(uint64_t, size_t, const brayns::Vector4f &)> &elementCallback);

    static std::vector<uint64_t> _randomly(
        const std::vector<uint64_t> &ids,
        const std::function<void(uint64_t, size_t, const brayns::Vector4f &)> &elementCallback);
};
