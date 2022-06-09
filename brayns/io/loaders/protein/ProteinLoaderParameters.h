/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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
    None = 0,
    ById = 1,
    ProteinAtoms = 2,
    ProteinChains = 3,
    ProteinResidues = 4
};

BRAYNS_JSON_ADAPTER_ENUM(
    ProteinLoaderColorScheme,
    {"none", ProteinLoaderColorScheme::None},
    {"by_id", ProteinLoaderColorScheme::ById},
    {"protein_atoms", ProteinLoaderColorScheme::ProteinAtoms},
    {"protein_chains", ProteinLoaderColorScheme::ProteinChains},
    {"protein_residues", ProteinLoaderColorScheme::ProteinResidues})

BRAYNS_JSON_OBJECT_BEGIN(ProteinLoaderParameters)
BRAYNS_JSON_OBJECT_ENTRY(
    ProteinLoaderColorScheme,
    color_scheme,
    "Defines how to color the loaded proteins",
    Default("none"))
BRAYNS_JSON_OBJECT_ENTRY(double, radius_multiplier, "A multiplier to apply to the protein sample radii", Default(1.0))
BRAYNS_JSON_OBJECT_END()
} // namespace brayns
