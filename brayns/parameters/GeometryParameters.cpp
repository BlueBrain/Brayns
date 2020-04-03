/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "GeometryParameters.h"
#include <brayns/common/log.h>
#include <brayns/common/types.h>

namespace
{
const std::string PARAM_COLOR_SCHEME = "color-scheme";
const std::string PARAM_GEOMETRY_QUALITY = "geometry-quality";
const std::string PARAM_RADIUS_MULTIPLIER = "radius-multiplier";
const std::string PARAM_MEMORY_MODE = "memory-mode";
const std::string PARAM_DEFAULT_BVH_FLAG = "default-bvh-flag";

const std::array<std::string, 5> COLOR_SCHEMES = {
    {"none", "by-id", "protein-atoms", "protein-chains", "protein-residues"}};

const std::string GEOMETRY_QUALITIES[3] = {"low", "medium", "high"};
const std::string GEOMETRY_MEMORY_MODES[2] = {"shared", "replicated"};
const std::map<std::string, brayns::BVHFlag> BVH_TYPES = {
    {"dynamic", brayns::BVHFlag::dynamic},
    {"compact", brayns::BVHFlag::compact},
    {"robust", brayns::BVHFlag::robust}};
} // namespace

namespace brayns
{
GeometryParameters::GeometryParameters()
    : AbstractParameters("Geometry")
{
    _parameters.add_options()
        //
        (PARAM_COLOR_SCHEME.c_str(), po::value<std::string>(),
         "Color scheme to be applied to the geometry "
         "[none|by-id|protein-atoms|protein-chains|protein-residues]")
        //
        (PARAM_GEOMETRY_QUALITY.c_str(), po::value<std::string>(),
         "Geometry rendering quality [low|medium|high]")
        //
        (PARAM_RADIUS_MULTIPLIER.c_str(), po::value<float>(),
         "Radius multiplier for spheres, cones and cylinders [float]")
        //
        (PARAM_MEMORY_MODE.c_str(), po::value<std::string>(),
         "Defines what memory mode should be used between Brayns and "
         "the underlying renderer [shared|replicated]")
        //
        (PARAM_DEFAULT_BVH_FLAG.c_str(),
         po::value<std::vector<std::string>>()->multitoken(),
         "Set a default flag to apply to BVH creation, one of "
         "[dynamic|compact|robust], may appear multiple times.");
}

void GeometryParameters::parse(const po::variables_map& vm)
{
    if (vm.count(PARAM_COLOR_SCHEME))
    {
        _colorScheme = ProteinColorScheme::none;
        const auto& colorScheme = vm[PARAM_COLOR_SCHEME].as<std::string>();
        if (!colorScheme.empty())
        {
            auto it = std::find(COLOR_SCHEMES.begin(), COLOR_SCHEMES.end(),
                                colorScheme);
            if (it == COLOR_SCHEMES.end())
                throw po::error("No match for color scheme '" + colorScheme);

            const auto index = std::distance(COLOR_SCHEMES.begin(), it);
            _colorScheme = static_cast<ProteinColorScheme>(index);
        }
    }
    if (vm.count(PARAM_GEOMETRY_QUALITY))
    {
        _geometryQuality = GeometryQuality::low;
        const auto& geometryQuality =
            vm[PARAM_GEOMETRY_QUALITY].as<std::string>();
        for (size_t i = 0;
             i < sizeof(GEOMETRY_QUALITIES) / sizeof(GEOMETRY_QUALITIES[0]);
             ++i)
            if (geometryQuality == GEOMETRY_QUALITIES[i])
                _geometryQuality = static_cast<GeometryQuality>(i);
    }
    if (vm.count(PARAM_RADIUS_MULTIPLIER))
        _radiusMultiplier = vm[PARAM_RADIUS_MULTIPLIER].as<float>();
    if (vm.count(PARAM_MEMORY_MODE))
    {
        const auto& memoryMode = vm[PARAM_MEMORY_MODE].as<std::string>();
        for (size_t i = 0; i < sizeof(GEOMETRY_MEMORY_MODES) /
                                   sizeof(GEOMETRY_MEMORY_MODES[0]);
             ++i)
            if (memoryMode == GEOMETRY_MEMORY_MODES[i])
                _memoryMode = static_cast<MemoryMode>(i);
    }
    if (vm.count(PARAM_DEFAULT_BVH_FLAG))
    {
        const auto& bvhs =
            vm[PARAM_DEFAULT_BVH_FLAG].as<std::vector<std::string>>();
        for (const auto& bvh : bvhs)
        {
            const auto kv = BVH_TYPES.find(bvh);
            if (kv != BVH_TYPES.end())
                _defaultBVHFlags.insert(kv->second);
            else
                throw std::runtime_error("Invalid bvh flag '" + bvh + "'.");
        }
    }

    markModified();
}

void GeometryParameters::print()
{
    AbstractParameters::print();
    BRAYNS_INFO << "Color scheme               : "
                << COLOR_SCHEMES[static_cast<size_t>(_colorScheme)]
                << std::endl;
    BRAYNS_INFO << "Geometry quality           : "
                << GEOMETRY_QUALITIES[static_cast<size_t>(_geometryQuality)]
                << std::endl;
    BRAYNS_INFO << "Radius multiplier          : " << _radiusMultiplier
                << std::endl;
    BRAYNS_INFO << "Memory mode                : "
                << (_memoryMode == MemoryMode::shared ? "Shared" : "Replicated")
                << std::endl;
}
} // namespace brayns
