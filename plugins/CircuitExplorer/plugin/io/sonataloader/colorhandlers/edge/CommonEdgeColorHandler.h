/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#pragma once

#include <plugin/api/CircuitColorHandler.h>

#include <bbp/sonata/config.h>

namespace sonataloader
{
/**
 * @brief The NeuronColorHandler class provides functionality
 *        to set edge population circuit color for all edge population types
 * except 'endfoot'
 */
class CommonEdgeColorHandler : public CircuitColorHandler
{
public:
    CommonEdgeColorHandler(const std::string& configPath,
                           const std::string& edgePopulation,
                           const bool afferent);

    void _setElementsImpl(
        const std::vector<uint64_t>& ids,
        std::vector<ElementMaterialMap::Ptr>&& elements) final;

    std::vector<std::string> _getMethodsImpl() const final;

    std::vector<std::string> _getMethodVariablesImpl(
        const std::string& method) const final;

    void _updateColorByIdImpl(
        const std::map<uint64_t, brayns::Vector4f>& colorMap) final;

    void _updateSingleColorImpl(const brayns::Vector4f& color) final;

    void _updateColorImpl(
        const std::string& method,
        const std::vector<ColoringInformation>& variables) final;

protected:
    const bbp::sonata::CircuitConfig _config;
    const std::string _edgePopulation;
    const std::string _nodePopulation;
    const bool _afferent;
    std::vector<uint64_t> _nodeIds;
    std::vector<std::vector<size_t>> _elements;
};
} // namespace sonataloader
