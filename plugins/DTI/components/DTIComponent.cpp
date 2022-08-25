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

#include <brayns/engine/common/ExtractComponent.h>
#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/MaterialComponent.h>
#include <brayns/engine/model/Model.h>

namespace
{
class NormalColorGenerator
{
public:
    static std::vector<brayns::Vector4f> generate(const std::vector<brayns::Capsule> &prims)
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
DTIComponent::DTIComponent(std::vector<std::vector<brayns::Capsule>> streamlineGeometries)
{
    _streamlines.reserve(streamlineGeometries.size());
    for (auto &primitives : streamlineGeometries)
    {
        _streamlines.emplace_back(std::move(primitives));
    }
    setDefaultColors();
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

void DTIComponent::onCreate()
{
    auto &model = getModel();
    model.addComponent<brayns::MaterialComponent>();

    auto &group = model.getGroup();
    std::vector<ospray::cpp::GeometricModel> geometricModels;
    geometricModels.reserve(_streamlines.size());
    for (auto &streamline : _streamlines)
    {
        geometricModels.push_back(streamline.view.getHandle());
    }
    group.setGeometry(geometricModels);
}

bool DTIComponent::commit()
{
    bool needsCommit = _colorsDirty;
    _colorsDirty = false;

    auto &material = brayns::ExtractComponent::material(getModel());
    if (material.commit())
    {
        for (auto &streamline : _streamlines)
        {
            streamline.view.setMaterial(material);
        }
        needsCommit = true;
    }

    if (needsCommit)
    {
        for (auto &streamline : _streamlines)
        {
            streamline.view.commit();
        }
    }

    return needsCommit;
}

size_t DTIComponent::getNumStreamlines() const noexcept
{
    return _streamlines.size();
}

void DTIComponent::setDefaultColors() noexcept
{
    for (auto &streamline : _streamlines)
    {
        auto &geometry = streamline.geometry;
        auto primitives = geometry.as<brayns::Capsule>();
        streamline.colors = NormalColorGenerator::generate(*primitives);
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
        auto &view = streamline.view;
        view.setColorPerPrimitive(ospray::cpp::SharedData(colors));
    }
    _colorsDirty = true;
}
}
