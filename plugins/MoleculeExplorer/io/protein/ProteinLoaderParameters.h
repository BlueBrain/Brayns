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

enum class ProteinLoaderColorScheme
{
    None,
    ById,
    ProteinAtoms,
    ProteinChains,
    ProteinResidues
};

struct ProteinLoaderParameters
{
    ProteinLoaderColorScheme color_scheme = ProteinLoaderColorScheme::None;
    double radius_multiplier = 0;
};

namespace brayns
{
template<>
struct EnumReflector<ProteinLoaderColorScheme>
{
    static EnumMap<ProteinLoaderColorScheme> reflect()
    {
        return {
            {"none", ProteinLoaderColorScheme::None},
            {"by_id", ProteinLoaderColorScheme::ById},
            {"protein_atoms", ProteinLoaderColorScheme::ProteinAtoms},
            {"protein_chains", ProteinLoaderColorScheme::ProteinChains},
            {"protein_residues", ProteinLoaderColorScheme::ProteinResidues}};
    }
};

template<>
struct JsonAdapter<ProteinLoaderColorScheme> : EnumAdapter<ProteinLoaderColorScheme>
{
};

template<>
struct JsonAdapter<ProteinLoaderParameters> : ObjectAdapter<ProteinLoaderParameters>
{
    static JsonObjectInfo reflect()
    {
        auto builder = Builder("ProteinLoaderParameters");
        builder
            .getset(
                "color_scheme",
                [](auto &object) { return object.color_scheme; },
                [](auto &object, auto value) { object.color_scheme = value; })
            .description("Proteins coloring scheme")
            .defaultValue(ProteinLoaderColorScheme::None);
        builder
            .getset(
                "radius_multiplier",
                [](auto &object) { return object.radius_multiplier; },
                [](auto &object, auto value) { object.radius_multiplier = value; })
            .description("Protein sample radius multiplier")
            .defaultValue(1);
        return builder.build();
    }
};
} // namespace brayns
