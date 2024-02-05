/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/json/JsonAdapterMacro.h>
#include <brayns/json/JsonObjectMacro.h>

namespace brayns
{
enum class ProteinLoaderColorScheme
{
    none = 0,
    by_id = 1,
    protein_atoms = 2,
    protein_chains = 3,
    protein_residues = 4
};

BRAYNS_JSON_ADAPTER_ENUM(
    ProteinLoaderColorScheme, {"none", ProteinLoaderColorScheme::none},
    {"by_id", ProteinLoaderColorScheme::by_id},
    {"protein_atoms", ProteinLoaderColorScheme::protein_atoms},
    {"protein_chains", ProteinLoaderColorScheme::protein_chains},
    {"protein_residues", ProteinLoaderColorScheme::protein_residues});

BRAYNS_JSON_OBJECT_BEGIN(ProteinLoaderParameters)
BRAYNS_JSON_OBJECT_ENTRY(ProteinLoaderColorScheme, color_scheme,
                         "Defines how to color the loaded proteins",
                         Default("none"))
BRAYNS_JSON_OBJECT_ENTRY(double, radius_multiplier,
                         "A multiplier to apply to the protein sample radii",
                         Default(1.0))
BRAYNS_JSON_OBJECT_END()
} // namespace brayns
