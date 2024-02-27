/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <io/NeuronMorphologyLoaderParameters.h>

struct CellPlacementLoaderParameters
{
    std::string morphology_folder;
    std::optional<std::string> extension;
    float percentage = 0;
    std::vector<uint64_t> ids;
    NeuronMorphologyLoaderParameters morphology_parameters;
};

namespace brayns
{
template<>
struct JsonAdapter<CellPlacementLoaderParameters> : ObjectAdapter<CellPlacementLoaderParameters>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("CellPlacementLoaderParameters");
        builder
            .getset(
                "morphology_folder",
                [](auto &object) -> auto & { return object.morphology_folder; },
                [](auto &object, auto value) { object.morphology_folder = std::move(value); })
            .description("Path to morphology folder")
            .required(false);
        builder
            .getset(
                "extension",
                [](auto &object) -> auto & { return object.extension; },
                [](auto &object, auto value) { object.extension = std::move(value); })
            .description("Morphology file extension")
            .required(false);
        builder
            .getset(
                "percentage",
                [](auto &object) { return object.percentage; },
                [](auto &object, auto value) { object.percentage = value; })
            .description("Percentage of cells to load")
            .minimum(0)
            .maximum(1)
            .defaultValue(1);
        builder
            .getset(
                "ids",
                [](auto &object) -> auto & { return object.ids; },
                [](auto &object, auto value) { object.ids = std::move(value); })
            .description("IDs of the nodes to load (overrides percentage)")
            .required(false);
        builder
            .getset(
                "morphology_parameters",
                [](auto &object) -> auto & { return object.morphology_parameters; },
                [](auto &object, auto value) { object.morphology_parameters = std::move(value); })
            .description("Settings for morphology geometry loading")
            .defaultValue(brayns::JsonFactory::object());
        return builder.build();
    }
};
} // namespace brayns
