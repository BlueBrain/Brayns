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

#include "DTIComponent.h"

#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/components/MaterialComponent.h>

namespace
{
struct NormalColorGenerator
{
    static std::vector<brayns::Vector4f> generate(const std::vector<brayns::Primitive> &prims)
    {
        std::vector<brayns::Vector4f> colors(prims.size());

        for (size_t i = 0; i < prims.size(); ++i)
        {
            const auto &primitive = prims[i];
            const auto &p1 = primitive.p0;
            const auto &p2 = primitive.p1;
            const auto dir = glm::normalize(p2 - p1);
            const auto n = brayns::Vector3f(0.5f + dir.x * 0.5f, 0.5f + dir.y * 0.5f, 0.5f + dir.z * 0.5f);
            colors[i] = brayns::Vector4f(n, 1.f);
        }

        return colors;
    }
};
}

namespace dti
{
size_t DTIComponent::getSizeInBytes() const noexcept
{
    size_t geometrySize = 0;
    for (const auto &streamline : _streamlines)
    {
        const auto &geometry = streamline.geometry;
        geometrySize += sizeof(Streamline) + geometry.getSizeInBytes();
    }

    return sizeof(DTIComponent) + geometrySize;
}

brayns::Bounds DTIComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    brayns::Bounds base;
    for (const auto &streamline : _streamlines)
    {
        const auto &geometry = streamline.geometry;
        const auto bounds = geometry.computeBounds(transform);
        base.expand(bounds);
    }

    return base;
}

void DTIComponent::onStart()
{
    auto &group = getModel();
    group.addComponent<brayns::MaterialComponent>();
}

bool DTIComponent::commit()
{
    bool needsCommit = false;

    auto &group = getModel();

    auto &material = brayns::ExtractModelObject::extractMaterial(group);
    if (material.commit())
    {
        for (auto &streamline : _streamlines)
        {
            auto model = streamline.model;
            brayns::GeometricModelHandler::setMaterial(model, material);
        }
        needsCommit = true;
    }

    if (needsCommit)
    {
        for (auto &streamline : _streamlines)
        {
            auto model = streamline.model;
            brayns::GeometricModelHandler::commitModel(model);
        }
    }

    return needsCommit;
}

void DTIComponent::onDestroyed()
{
    auto &group = getModel();
    for (auto &streamline : _streamlines)
    {
        auto &model = streamline.model;
        brayns::GeometricModelHandler::removeFromGeometryGroup(model, group);
        brayns::GeometricModelHandler::destory(model);
    }
}

void DTIComponent::setStreamlines(std::vector<std::vector<brayns::Primitive>> &geometries)
{
    auto &group = getModel();

    _streamlines.reserve(geometries.size());

    for (auto &primitives : geometries)
    {
        auto &streamline = _streamlines.emplace_back();

        auto &geometry = streamline.geometry;
        geometry.set(std::move(primitives));
        geometry.commit();

        auto &model = streamline.model;
        model = brayns::GeometricModelHandler::create();
        brayns::GeometricModelHandler::addToGeometryGroup(model, group);
        brayns::GeometricModelHandler::setGeometry(model, geometry);
    }

    setDefaultColors();
}

size_t DTIComponent::getNumStreamlines() const noexcept
{
    return _streamlines.size();
}

void DTIComponent::setDefaultColors() noexcept
{
    for (auto &streamline : _streamlines)
    {
        auto &colors = streamline.colors;
        auto &geometry = streamline.geometry;
        const auto &primitives = geometry.getAll();
        colors = NormalColorGenerator::generate(primitives);
    }

    _commitColors();
}

void DTIComponent::updateSimulation(const std::vector<std::vector<float>> &data)
{
    if (_streamlines.size() != data.size())
    {
        throw std::invalid_argument("Not enough data provided for all streamlines");
    }

    for (size_t i = 0; i < _streamlines.size(); ++i)
    {
        auto &streamlineData = data[i];
        if (streamlineData.empty())
        {
            continue;
        }

        auto &streamline = _streamlines[i];
        auto &colors = streamline.colors;

        const auto streamlineLength = colors.size() - 1;

        for (const auto value : streamlineData)
        {
            auto index = static_cast<size_t>(value * streamlineLength);
            index = index > streamlineLength ? streamlineLength : index;
            colors[index] = brayns::Vector4f(1.f);
        }
    }

    _commitColors();
}

void DTIComponent::_commitColors() noexcept
{
    for (auto &streamline : _streamlines)
    {
        auto &colors = streamline.colors;
        auto colorBuffer = brayns::DataHandler::shareBuffer(colors, OSPDataType::OSP_VEC4F);

        auto model = streamline.model;
        brayns::GeometricModelHandler::setColors(model, colorBuffer);
    }
    _colorsDirty = true;
}
}
