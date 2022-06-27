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

#include "EndfeetComponent.h"

#include <brayns/common/Log.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/MaterialComponent.h>

#include <api/coloring/ColorByIDAlgorithm.h>

brayns::Bounds EndfeetComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    brayns::Bounds result;
    for (const auto &endfoot : _endFeet)
    {
        const auto endFootBounds = endfoot.computeBounds(transform);
        result.expand(endFootBounds);
    }
    return result;
}

bool EndfeetComponent::commit()
{
    bool needsCommit = _colorsDirty;
    _colorsDirty = false;

    auto &material = brayns::ExtractModelObject::extractMaterial(getModel());
    if (material.commit())
    {
        for (auto &endfoot : _endFeet)
        {
            endfoot.setMaterial(material);
            needsCommit = true;
        }
    }

    if (needsCommit)
    {
        for (auto &endfoot : _endFeet)
        {
            endfoot.commit();
        }
    }

    return needsCommit;
}

void EndfeetComponent::onDestroy()
{
    auto &model = getModel();
    auto &group = model.getGroup();
    for (auto &endfoot : _endFeet)
    {
        group.addGeometry(endfoot);
    }
}

const std::vector<uint64_t> &EndfeetComponent::getAstroctyeIds() const noexcept
{
    return _astrocyteIds;
}

void EndfeetComponent::addEndfeet(std::map<uint64_t, std::vector<brayns::TriangleMesh>> &endfeetGeometry)
{
    _astrocyteIds.reserve(endfeetGeometry.size());
    _endFeet.reserve(endfeetGeometry.size());

    auto &model = getModel();
    model.addComponent<brayns::MaterialComponent>();

    auto &group = model.getGroup();

    for (auto &[astrocyteId, endfeets] : endfeetGeometry)
    {
        brayns::TriangleMesh mergedMeshes;
        for (auto &mesh : endfeets)
        {
            if (mesh.vertices.empty() || mesh.indices.empty())
            {
                brayns::Log::warn("[CE] Skipping empty endfoot mesh connected to astrocyte id {}", astrocyteId);
                continue;
            }
            brayns::TriangleMeshMerger::merge(mesh, mergedMeshes);
        }

        if (mergedMeshes.indices.empty() || mergedMeshes.vertices.empty())
        {
            brayns::Log::warn("[CE] Skipping ALL endfeet mesh connected to astrocyte id {}", astrocyteId);
            continue;
        }

        _astrocyteIds.push_back(astrocyteId);
        auto &endfoot = _endFeet.emplace_back(std::move(mergedMeshes));
        endfoot.setColor(brayns::Vector4f(1.f));
        group.addGeometry(endfoot);
    }
}

void EndfeetComponent::setColor(const brayns::Vector4f &color) noexcept
{
    for (auto &endfoot : _endFeet)
    {
        endfoot.setColor(color);
    }
    _colorsDirty = true;
}

void EndfeetComponent::setColorById(const std::vector<brayns::Vector4f> &colors)
{
    for (size_t i = 0; i < _endFeet.size(); ++i)
    {
        const auto &color = colors[i];
        auto &endFoot = _endFeet[i];
        endFoot.setColor(color);
    }
    _colorsDirty = true;
}

std::vector<uint64_t> EndfeetComponent::setColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
{
    auto skipped = ColorByIDAlgorithm::execute(
        colorMap,
        _astrocyteIds,
        [&](uint64_t id, size_t index, const brayns::Vector4f &color)
        {
            (void)id;
            auto &endFoot = _endFeet[index];
            endFoot.setColor(color);
            _colorsDirty = true;
        });

    return skipped;
}
