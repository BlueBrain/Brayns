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

#include "Vasculature.h"

#include "detail/Common.h"

namespace
{
struct Attributes
{
    static inline const std::string startX = "start_x";
    static inline const std::string startY = "start_y";
    static inline const std::string startZ = "start_z";
    static inline const std::string endX = "end_x";
    static inline const std::string endY = "end_y";
    static inline const std::string endZ = "end_z";
    static inline const std::string startDiameter = "start_diameter";
    static inline const std::string endDiameter = "end_diameter";
    static inline const std::string startNode = "start_node";
    static inline const std::string endNode = "end_node";
    static inline const std::string type = "type";
    static inline const std::string sectionId = "section_id";
};

class DataReader
{
public:
    static std::vector<brayns::Vector3f> readPoints(
        const bbp::sonata::NodePopulation &nodes,
        const bbp::sonata::Selection &selection,
        const std::string &xName,
        const std::string &yName,
        const std::string &zName)
    {
        sonataloader::detail::AttributeValidator::validate(nodes, {xName, yName, zName});
        auto x = nodes.getAttribute<float>(xName, selection);
        auto y = nodes.getAttribute<float>(yName, selection);
        auto z = nodes.getAttribute<float>(zName, selection);
        auto count = x.size();

        auto result = std::vector<brayns::Vector3f>();
        result.reserve(count);

        for (size_t i = 0; i < count; ++i)
        {
            result.emplace_back(x[i], y[i], z[i]);
        }

        return result;
    }

    static std::vector<float> readRadii(
        const bbp::sonata::NodePopulation &nodes,
        const bbp::sonata::Selection &selection,
        const std::string &radiiName)
    {
        sonataloader::detail::AttributeValidator::validate(nodes, {radiiName});
        auto radii = nodes.getAttribute<float>(radiiName, selection);
        for (auto &radius : radii)
        {
            radius *= 0.5f;
        }
        return radii;
    }
};
} // namespace

namespace sonataloader
{
std::vector<brayns::Vector3f> Vasculature::getSegmentStartPoints(const Nodes &nodes, const Selection &selection)
{
    return DataReader::readPoints(nodes, selection, Attributes::startX, Attributes::startY, Attributes::startZ);
}

std::vector<brayns::Vector3f> Vasculature::getSegmentEndPoints(const Nodes &nodes, const Selection &selection)
{
    return DataReader::readPoints(nodes, selection, Attributes::endX, Attributes::endY, Attributes::endZ);
}

std::vector<float> Vasculature::getSegmentStartRadii(const Nodes &nodes, const Selection &selection)
{
    return DataReader::readRadii(nodes, selection, Attributes::startDiameter);
}

std::vector<float> Vasculature::getSegmentEndRadii(const Nodes &nodes, const Selection &selection)
{
    return DataReader::readRadii(nodes, selection, Attributes::endDiameter);
}

std::vector<VasculatureSection> Vasculature::getSegmentSectionTypes(const Nodes &nodes, const Selection &selection)
{
    detail::AttributeValidator::validate(nodes, {Attributes::type});

    auto rawTypes = nodes.getAttribute<uint8_t>(Attributes::type, selection);

    auto result = std::vector<VasculatureSection>();
    result.reserve(rawTypes.size());

    for (auto type : rawTypes)
    {
        result.push_back(static_cast<VasculatureSection>(type));
    }

    return result;
}
} // namespace sonataloader
