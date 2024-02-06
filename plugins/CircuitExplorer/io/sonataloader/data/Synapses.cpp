/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "Synapses.h"

#include "detail/Common.h"

#include <algorithm>
#include <map>
#include <random>

namespace
{
struct Attributes
{
    static inline const std::string efferentSurfaceX = "efferent_surface_x";
    static inline const std::string efferentSurfaceY = "efferent_surface_y";
    static inline const std::string efferentSurfaceZ = "efferent_surface_z";
    static inline const std::string afferentSurfaceX = "afferent_surface_x";
    static inline const std::string afferentSurfaceY = "afferent_surface_y";
    static inline const std::string afferentSurfaceZ = "afferent_surface_z";
    static inline const std::string astrocyteCenterX = "astrocyte_center_x";
    static inline const std::string astrocyteCenterY = "astrocyte_center_y";
    static inline const std::string astrocyteCenterZ = "astrocyte_center_z";
    static inline const std::string endFeetId = "endfoot_id";
};

class PositionReader
{
public:
    static std::vector<brayns::Vector3f> read(
        const bbp::sonata::EdgePopulation &pops,
        const bbp::sonata::Selection &selection,
        const std::string &attribX,
        const std::string &attribY,
        const std::string &attribZ)
    {
        sonataloader::detail::AttributeValidator::validate(pops, {attribX, attribY, attribZ});
        auto x = pops.getAttribute<float>(attribX, selection);
        auto y = pops.getAttribute<float>(attribY, selection);
        auto z = pops.getAttribute<float>(attribZ, selection);
        auto count = x.size();

        auto result = std::vector<brayns::Vector3f>();
        result.reserve(count);

        for (size_t i = 0; i < count; ++i)
        {
            result.emplace_back(x[i], y[i], z[i]);
        }

        return result;
    }
};
} // namespace

namespace sonataloader
{
std::vector<uint64_t> Synapses::getSourceNodes(const Edges &edges, const Selection &selection)
{
    return edges.sourceNodeIDs(selection);
}

std::vector<uint64_t> Synapses::getTargetNodes(const Edges &edges, const Selection &selection)
{
    return edges.targetNodeIDs(selection);
}

std::vector<brayns::Vector3f> Synapses::getAfferentSurfacePos(const Edges &edges, const Selection &selection)
{
    return PositionReader::read(
        edges,
        selection,
        Attributes::afferentSurfaceX,
        Attributes::afferentSurfaceY,
        Attributes::afferentSurfaceZ);
}

std::vector<brayns::Vector3f> Synapses::getEfferentSurfacePos(const Edges &edges, const Selection &selection)
{
    return PositionReader::read(
        edges,
        selection,
        Attributes::efferentSurfaceX,
        Attributes::efferentSurfaceY,
        Attributes::efferentSurfaceZ);
}

std::vector<brayns::Vector3f> Synapses::getEfferentAstrocyteCenterPos(const Edges &edges, const Selection &selection)
{
    return PositionReader::read(
        edges,
        selection,
        Attributes::astrocyteCenterX,
        Attributes::astrocyteCenterY,
        Attributes::astrocyteCenterZ);
}

std::vector<uint64_t> Synapses::getEndFeetIds(const Edges &edges, const Selection &selection)
{
    detail::AttributeValidator::validate(edges, {Attributes::endFeetId});
    return edges.getAttribute<uint64_t>(Attributes::endFeetId, selection);
}
} // namespace sonataloader
