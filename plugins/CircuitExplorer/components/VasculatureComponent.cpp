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

#include "VasculatureComponent.h"

#include <brayns/engine/common/ExtractComponent.h>
#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/MaterialComponent.h>

#include <api/coloring/ColorByIDAlgorithm.h>

VasculatureComponent::VasculatureComponent(
    std::vector<uint64_t> ids,
    std::vector<brayns::Capsule> geometry,
    std::vector<VasculatureSection> sections)
    : _ids(std::move(ids))
    , _geometry(std::move(geometry))
    , _view(_geometry)
    , _sections(std::move(sections))
    , _colors(_geometry.numPrimitives(), brayns::Vector4f(1.f))
{
    _geometry.commit();
}

brayns::Bounds VasculatureComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    return _geometry.computeBounds(transform);
}

void VasculatureComponent::onCreate()
{
    auto &model = getModel();
    model.addComponent<brayns::MaterialComponent>();

    auto &group = model.getGroup();
    group.setGeometry(_view);
}

bool VasculatureComponent::commit()
{
    auto &material = brayns::ExtractComponent::material(getModel());
    if (material.commit())
    {
        _view.setMaterial(material);
    }

    return _view.commit();
}

const std::vector<uint64_t> &VasculatureComponent::getIDs() const noexcept
{
    return _ids;
}

void VasculatureComponent::setColor(const brayns::Vector4f &color) noexcept
{
    std::fill(_colors.begin(), _colors.end(), color);
    _view.setColorPerPrimitive(ospray::cpp::SharedData(_colors));
}

void VasculatureComponent::setColorBySection(
    const std::vector<std::pair<VasculatureSection, brayns::Vector4f>> &colormap) noexcept
{
    bool somethingColored = false;
    for (size_t i = 0; i < _colors.size(); ++i)
    {
        const auto section = _sections[i];
        for (const auto &entry : colormap)
        {
            const auto checkSection = entry.first;
            const auto &color = entry.second;
            if (checkSection == section)
            {
                _colors[i] = color;
                somethingColored = true;
            }
        }
    }

    if (somethingColored)
    {
        _view.setColorPerPrimitive(ospray::cpp::SharedData(_colors));
    }
}

void VasculatureComponent::setColorById(std::vector<brayns::Vector4f> colors) noexcept
{
    _colors = std::move(colors);
    _view.setColorPerPrimitive(ospray::cpp::SharedData(_colors));
}

std::vector<uint64_t> VasculatureComponent::setColorById(const std::map<uint64_t, brayns::Vector4f> &colors) noexcept
{
    auto skipped = ColorByIDAlgorithm::execute(
        colors,
        _ids,
        [&](uint64_t id, size_t index, const brayns::Vector4f &color)
        {
            (void)id;
            _colors[index] = color;
        });

    if (skipped.size() < _ids.size())
    {
        _view.setColorPerPrimitive(ospray::cpp::SharedData(_colors));
    }

    return skipped;
}

void VasculatureComponent::setSimulationColor(
    const std::vector<brayns::Vector4f> &color,
    const std::vector<uint8_t> &mapping) noexcept
{
    _view.setColorMap(ospray::cpp::CopiedData(mapping), ospray::cpp::CopiedData(color));
}

brayns::Geometry &VasculatureComponent::getGeometry() noexcept
{
    return _geometry;
}
