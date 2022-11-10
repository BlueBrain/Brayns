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

#include <api/coloring/IBrainColorData.h>

namespace bbploader
{
class BBPColorData final : public IBrainColorData
{
public:
    BBPColorData(std::string circuitPath, std::string circuitPop);

    std::vector<BrainColorMethod> getMethods() const override;

    std::vector<std::string> getValues(BrainColorMethod method, const std::vector<uint64_t> &ids) const override;

private:
    std::string _circuitPath;
    std::string _circuitPop;
};
} // namespace bbploader
