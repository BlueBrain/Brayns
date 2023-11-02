/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "Cells.h"

#include "detail/Common.h"

#include <bbp/sonata/node_sets.h>

namespace
{
struct Attributes
{
    static inline const std::string modelType = "model_type";
    static inline const std::string x = "x";
    static inline const std::string y = "y";
    static inline const std::string z = "z";
    static inline const std::string quatX = "orientation_x";
    static inline const std::string quatY = "orientation_y";
    static inline const std::string quatZ = "orientation_z";
    static inline const std::string quatW = "orientation_w";
    static inline const std::string layer = "layer";
    static inline const std::string region = "region";
    static inline const std::string mtype = "mtype";
    static inline const std::string etype = "etype";
    static inline const std::string morphology = "morphology";
};

} // namespace

namespace sonataloader
{
std::string Cells::getPopulationType(const Nodes &nodes)
{
    detail::AttributeValidator::validate(nodes, {Attributes::modelType});
    auto selection = bbp::sonata::Selection::fromValues({0});
    return nodes.getAttribute<std::string>(Attributes::modelType, selection)[0];
}

std::vector<std::string> Cells::getMorphologies(const Nodes &nodes, const Selection &selection)
{
    detail::AttributeValidator::validate(nodes, {Attributes::morphology});
    return nodes.getAttribute<std::string>(Attributes::morphology, selection);
}

std::vector<brayns::Vector3f> Cells::getPositions(const Nodes &nodes, const Selection &selection)
{
    detail::AttributeValidator::validate(nodes, {Attributes::x, Attributes::y, Attributes::z});

    auto x = nodes.getAttribute<float>(Attributes::x, selection);
    auto y = nodes.getAttribute<float>(Attributes::y, selection);
    auto z = nodes.getAttribute<float>(Attributes::z, selection);
    auto count = x.size();

    auto result = std::vector<brayns::Vector3f>();
    result.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        result.emplace_back(x[i], y[i], z[i]);
    }
    return result;
}

std::vector<brayns::Quaternion> Cells::getRotations(const Nodes &nodes, const Selection &selection)
{
    detail::AttributeValidator::validate(
        nodes,
        {Attributes::quatX, Attributes::quatY, Attributes::quatZ, Attributes::quatW});

    auto x = nodes.getAttribute<float>(Attributes::quatX, selection);
    auto y = nodes.getAttribute<float>(Attributes::quatY, selection);
    auto z = nodes.getAttribute<float>(Attributes::quatZ, selection);
    auto w = nodes.getAttribute<float>(Attributes::quatW, selection);
    auto count = x.size();

    auto result = std::vector<brayns::Quaternion>();
    result.reserve(count);

    for (size_t i = 0; i < count; ++i)
    {
        result.push_back(brayns::math::normalize(brayns::Quaternion(w[i], x[i], y[i], z[i])));
    }

    return result;
}

std::vector<std::string> Cells::getLayers(const Nodes &nodes, const Selection &selection)
{
    detail::AttributeValidator::validate(nodes, {Attributes::layer});
    return nodes.getAttribute<std::string>(Attributes::layer, selection);
}

std::vector<std::string> Cells::getRegions(const Nodes &nodes, const Selection &selection)
{
    detail::AttributeValidator::validate(nodes, {Attributes::region});
    return nodes.getAttribute<std::string>(Attributes::region, selection);
}

std::vector<std::string> Cells::getMTypes(const Nodes &nodes, const Selection &selection)
{
    detail::AttributeValidator::validate(nodes, {Attributes::mtype});
    return nodes.getAttribute<std::string>(Attributes::mtype, selection);
}

std::vector<std::string> Cells::getETypes(const Nodes &nodes, const Selection &selection)
{
    detail::AttributeValidator::validate(nodes, {Attributes::etype});
    return nodes.getAttribute<std::string>(Attributes::etype, selection);
}
} // namespace sonataloader
