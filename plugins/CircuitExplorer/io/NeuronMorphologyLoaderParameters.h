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

#include <api/neuron/NeuronGeometryType.h>

struct NeuronMorphologyLoaderParameters
{
    float radius_multiplier = 0;
    bool load_soma = false;
    bool load_axon = false;
    bool load_dendrites = false;
    NeuronGeometryType geometry_type = NeuronGeometryType::Original;
    float resampling = 0;
    uint32_t subsampling = 1;
    float growth = 1.0f;
};

namespace brayns
{

template<>
struct EnumReflector<NeuronGeometryType>
{
    static EnumMap<NeuronGeometryType> reflect()
    {
        return {
            {"original", NeuronGeometryType::Original},
            {"smooth", NeuronGeometryType::Smooth},
            {"section_smooth", NeuronGeometryType::SectionSmooth},
            {"constant_radii", NeuronGeometryType::ConstantRadii},
            {"spheres", NeuronGeometryType::Spheres}};
    }
};

template<>
struct JsonAdapter<NeuronGeometryType> : EnumAdapter<NeuronGeometryType>
{
};

template<>
struct JsonAdapter<NeuronMorphologyLoaderParameters> : ObjectAdapter<NeuronMorphologyLoaderParameters>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("NeuronMorphologyLoaderParameters");
        builder
            .getset(
                "radius_multiplier",
                [](auto &object) { return object.radius_multiplier; },
                [](auto &object, auto value) { object.radius_multiplier = value; })
            .description("Parameter to multiply all morphology sample radii by")
            .defaultValue(1)
            .minimum(0.1);
        builder
            .getset(
                "load_soma",
                [](auto &object) { return object.load_soma; },
                [](auto &object, auto value) { object.load_soma = value; })
            .description("Load the soma section of the neuron")
            .defaultValue(false);
        builder
            .getset(
                "load_axon",
                [](auto &object) { return object.load_axon; },
                [](auto &object, auto value) { object.load_axon = value; })
            .description("Load the axon section of the neuron")
            .defaultValue(false);
        builder
            .getset(
                "load_dendrites",
                [](auto &object) { return object.load_dendrites; },
                [](auto &object, auto value) { object.load_dendrites = value; })
            .description("Load the dendrites section of the neuron")
            .defaultValue(false);
        builder
            .getset(
                "geometry_type",
                [](auto &object) { return object.geometry_type; },
                [](auto &object, auto value) { object.geometry_type = value; })
            .description("Geometry generation configuration")
            .defaultValue(NeuronGeometryType::Smooth);
        builder
            .getset(
                "resampling",
                [](auto &object) { return object.resampling; },
                [](auto &object, auto value) { object.resampling = value; })
            .description("Minimum angle cosine between 2 segments to merge them (disabled if > 1)")
            .defaultValue(2.0);
        builder
            .getset(
                "subsampling",
                [](auto &object) { return object.subsampling; },
                [](auto &object, auto value) { object.subsampling = value; })
            .description("Skip factor when converting samples into geometry (disabled if <= 1)")
            .defaultValue(1);
        builder
            .getset(
                "growth",
                [](auto &object) { return object.growth; },
                [](auto &object, auto value) { object.growth = value; })
            .description("Neuron growth [0-1], includes all segments at 1 and none at 0")
            .defaultValue(1.0f);
        return builder.build();
    }
};
} // namespace brayns
