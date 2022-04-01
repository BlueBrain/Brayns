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

#include <plugin/api/CircuitColorData.h>

namespace sonataloader
{
/**
 * @brief The EndFootColorHandler class provides functionality to set endfett population color
 */
class EndFootColorData final : public CircuitColorData
{
public:
    /**
     * @brief getMethods
     * @return
     */
    std::vector<std::string> getMethods() const noexcept override;

    /**
     * @brief getMethodVariables
     * @return
     */
    std::vector<std::string> getMethodVariables(const std::string &) const override;

    /**
     * @brief Return the values corresponding to the given ids for the given method of coloring
     * @param method
     * @param ids
     * @return std::vector<std::string>
     */
    std::vector<std::string> getMethodValuesForIDs(const std::string &method,
                                                   const std::vector<uint64_t>& ids) const override;

};
} // namespace sonataloader
