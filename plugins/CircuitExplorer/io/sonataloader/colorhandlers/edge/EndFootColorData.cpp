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

#include "EndFootColorData.h"

#include <stdexcept>

namespace sonataloader
{
std::vector<std::string> EndFootColorData::getMethods() const noexcept
{
    return {};
}

std::vector<std::string> EndFootColorData::getMethodVariables(const std::string &method) const
{
    throw std::invalid_argument("Method " + method + " not available to color endfeet");
}

std::vector<std::string> EndFootColorData::getMethodValuesForIDs(
        const std::string &method, const std::vector<uint64_t>& ids) const
{
    (void) ids;
    throw std::invalid_argument("Method " + method + " not available to color endfeet");
}
} // namespace sonataloader
