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
#include <brayns/engine/common/ExtractComponent.h>
#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/MaterialComponent.h>

#include <api/coloring/ColorByIDAlgorithm.h>

brayns::Bounds EndfeetComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    brayns::Bounds result;
    for (const auto &endfoot : _geometries)
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

    auto &material = brayns::ExtractComponent::material(getModel());
    if (material.commit())
    {
        for (auto &endfoot : _views)
        {
            endfoot.setMaterial(material);
            needsCommit = true;
        }
    }

    if (needsCommit)
    {
        for (auto &endfoot : _views)
        {
            endfoot.commit();
        }
    }

    return needsCommit;
}

void EndfeetComponent::onCreate()
{
    auto &model = getModel();
    auto &group = model.getGroup();
    group.setGeometry(_views);
}

const std::vector<uint64_t> &EndfeetComponent::getAstroctyeIds() const noexcept
{
    return _astrocyteIds;
}

void EndfeetComponent::addEndfeet(std::map<uint64_t, std::vector<brayns::TriangleMesh>> &endfeetGeometry)
{
    _astrocyteIds.reserve(endfeetGeometry.size());
    _geometries.reserve(endfeetGeometry.size());
    _views.reserve(endfeetGeometry.size());

    auto &model = getModel();
    model.addComponent<brayns::MaterialComponent>();

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
            brayns::TriangleMeshUtils::merge(mesh, mergedMeshes);
        }

        if (mergedMeshes.indices.empty() || mergedMeshes.vertices.empty())
        {
            brayns::Log::warn("[CE] Skipping ALL endfeet mesh connected to astrocyte id {}", astrocyteId);
            continue;
        }

        _astrocyteIds.push_back(astrocyteId);
        auto &geometry = _geometries.emplace_back(std::move(mergedMeshes));
        geometry.commit();
        auto &view = _views.emplace_back(geometry);
        view.setColor(brayns::Vector4f(1.f));
    }
}

void EndfeetComponent::setColor(const brayns::Vector4f &color) noexcept
{
    for (auto &endfoot : _views)
    {
        endfoot.setColor(color);
    }
    _colorsDirty = true;
}

void EndfeetComponent::setColorById(const std::vector<brayns::Vector4f> &colors)
{
    assert(colors.size() == _views.size());
    for (size_t i = 0; i < _views.size(); ++i)
    {
        const auto &color = colors[i];
        auto &endFoot = _views[i];
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
            auto &endFoot = _views[index];
            endFoot.setColor(color);
            _colorsDirty = true;
        });

    return skipped;
}
