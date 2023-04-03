/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

#include <brayns/json/Json.h>

struct CellPlacementLoaderParameters
{
    std::string morphology_folder;
    float percentage = 0;
    std::optional<std::string> extension;
};

namespace brayns
{
template<>
struct JsonAdapter<CellPlacementLoaderParameters> : ObjectAdapter<CellPlacementLoaderParameters>
{
    static void reflect()
    {
        title("CellPlacementLoaderParameters");
        getset(
            "morphology_folder",
            [](auto &object) -> auto & { return object.morphology_folder; },
            [](auto &object, auto value) { object.morphology_folder = std::move(value); })
            .description("Path to morphology folder");
        getset(
            "percentage",
            [](auto &object) { return object.percentage; },
            [](auto &object, auto value) { object.percentage = value; })
            .description("Percentage of cells to load")
            .minimum(0)
            .maximum(1)
            .defaultValue(1);
        getset(
            "extension",
            [](auto &object) -> auto & { return object.extension; },
            [](auto &object, auto value) { object.extension = std::move(value); })
            .description("Morphology file extension")
            .required(false);
    }
};
} // namespace brayns
