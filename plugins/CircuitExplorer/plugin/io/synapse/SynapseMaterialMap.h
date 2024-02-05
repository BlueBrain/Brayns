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

/**
 * @brief The SynapseMaterialInfo struct holds information for a single edge of
 *        a single cell (edge id and material id)
 */
struct SynapseMaterialInfo
{
    uint64_t id;
    size_t material;

    SynapseMaterialInfo(const uint64_t id, const size_t matId)
        : id(id)
        , material(matId)
    {
    }
};

/**
 * @brief The SynapseMaterialMap class its an implementation of the
 * ElementMaterialMap class that allows CircuitColorHandlers to access the
 * materials of a cell edge geometries
 */
class SynapseMaterialMap : public ElementMaterialMap
{
public:
    using Ptr = std::unique_ptr<SynapseMaterialMap>;

    std::vector<SynapseMaterialInfo> materials;

    void setColor(brayns::ModelDescriptor*,
                  const brayns::Vector4f&) noexcept final
    {
    }
};
