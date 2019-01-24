/* Copyright (c) 2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Juan.Hernando@epfl.ch
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

#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>

#include <brain/neuron/types.h>
#include <brain/types.h>

#include <random>
#include <regex>

namespace brayns
{
namespace
{
inline void _createBySegmentMaterials(Model& model)
{
    std::array<MaterialPtr, 5> materials{{
        model.createMaterial(0, "unknown_section"),
        model.createMaterial(1, "soma"),
        model.createMaterial(2, "axon"),
        model.createMaterial(3, "dendrite"),
        model.createMaterial(4, "apical dendrite")}};
    materials[0]->setDiffuseColor(Vector3f(0.9f, 0.9f, 0.9f));
    materials[1]->setDiffuseColor(Vector3f(0.9f, 0.9f, 0.9f));
    materials[2]->setDiffuseColor(Vector3f(0.2f, 0.4f, 0.8f));
    materials[3]->setDiffuseColor(Vector3f(0.8f, 0.2f, 0.2f));
    materials[4]->setDiffuseColor(Vector3f(0.8f, 0.2f, 0.8f));
    for (auto& material : materials)
    {
        material->setSpecularColor(Vector3f(0.f));
        material->setOpacity(1.f);
        material->setReflectionIndex(0.f);
        material->setEmission(0.f);
    }
}

inline size_t _getMaterialId(const brain::neuron::SectionType type)
{
    switch (type)
    {
    case brain::neuron::SectionType::soma:
        return 1;
    case brain::neuron::SectionType::axon:
        return 2;
    case brain::neuron::SectionType::dendrite:
        return 3;
    case brain::neuron::SectionType::apicalDendrite:
        return 4;
    default:
        return 0;
    }
}

inline brion::GIDSet _gidsFromRange(const uint32_t first, const uint32_t last,
                                    const double fraction, const int32_t seed)
{
    if (last < first)
        throw std::runtime_error("Invalid GID range");

    if (fraction == 1.)
    {
        brion::GIDSet result;
        for (uint32_t i = first; i <= last; ++i)
            result.insert(result.end(), i);
        return result;
    }

    // We need to create a gids list, shuffle and trim. This is the easiest
    // way to ensure that the size of the resulting container has always
    // the same size.

    std::vector<uint32_t> gids;
    gids.reserve(last - first + 1);
    for (uint32_t i = first; i <= last; ++i)
        gids.push_back(i);

    std::random_device device;
    std::mt19937_64 engine(device());
    engine.seed(seed);
    std::shuffle(gids.begin(), gids.end(), engine);
    gids.resize(size_t(std::ceil(gids.size() * fraction)));

    return brion::GIDSet(gids.begin(), gids.end());
}

inline brion::GIDSet _keyToGIDorRange(const std::string& key,
                                      const double fraction,
                                      const int32_t seed)
{
    std::regex regex(
        "([0-9]+)|"               // single GID
        "(([0-9]+)\\-([0-9]+))"); // GID range
    std::smatch sm;
    std::regex_match(key, sm, regex);

    const std::string gid = sm[1];
    if (!gid.empty())
        // Single gid matched
        return brion::GIDSet{static_cast<uint32_t>(std::stol(gid))};

    if (!static_cast<std::string>(sm[2]).empty())
    {
        // Range matched
        const uint32_t first =
            static_cast<uint32_t>(std::stol(static_cast<std::string>(sm[3])));
        const uint32_t last =
            static_cast<uint32_t>(std::stol(static_cast<std::string>(sm[4])));
        return _gidsFromRange(first, last, fraction, seed);
    }

    return {};
}
}
}
