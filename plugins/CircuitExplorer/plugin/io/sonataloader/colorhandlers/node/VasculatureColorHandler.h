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
#include <plugin/io/morphology/vasculature/VasculatureSection.h>

#include <unordered_map>

namespace sonataloader
{
/**
 * @brief The NeuronColorHandler class provides functionality
 *        to set a vasculature node population circuit color
 */
class VasculatureColorHandler : public CircuitColorHandler
{
public:
    void _setElementsImpl(
        const std::vector<uint64_t>& ids,
        std::vector<ElementMaterialMap::Ptr>&& elements) final;

    std::vector<std::string> _getMethodsImpl() const final;

    std::vector<std::string> _getMethodVariablesImpl(
        const std::string&) const final;

    void _updateColorByIdImpl(
        const std::map<uint64_t, brayns::Vector4f>& colorMap) final;

    void _updateSingleColorImpl(const brayns::Vector4f& color) final;

    void _updateColorImpl(
        const std::string&,
        const std::vector<ColoringInformation>& variables) final;

private:
    std::vector<uint64_t> _ids;
    std::vector<ElementMaterialMap::Ptr> _elements;
    std::unordered_map<VasculatureSection, std::vector<size_t>>
        _sectionMaterials;
};
} // namespace sonataloader
