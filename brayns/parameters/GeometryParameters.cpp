/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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
const std::string PARAM_MEMORY_MODE = "memory-mode";
const std::string PARAM_DEFAULT_BVH_FLAG = "default-bvh-flag";

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
    BRAYNS_INFO << "Memory mode                : "
                << (_memoryMode == MemoryMode::shared ? "Shared" : "Replicated")
                << std::endl;
}
} // namespace brayns
