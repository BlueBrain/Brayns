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

#include "MorphologyCircuitComponent.h"

#include <brayns/engine/common/ExtractComponent.h>
#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/MaterialComponent.h>

#include <api/coloring/ColorByIDAlgorithm.h>

namespace
{
class MorphologyGeometryInitializer
{
public:
    static auto init(
        std::vector<std::vector<brayns::Capsule>> primitives,
        std::vector<std::vector<NeuronSectionMapping>> map)
    {
        std::vector<MorphologyGeometry> morphologies;
        morphologies.reserve(primitives.size());

        for (size_t i = 0; i < primitives.size(); ++i)
        {
            auto &morphGeometry = primitives[i];
            auto &morphSections = map[i];
            morphologies.emplace_back(std::move(morphGeometry), std::move(morphSections));
        }

        return morphologies;
    }
};
}

MorphologyCircuitComponent::MorphologyCircuitComponent(
    std::vector<uint64_t> ids,
    std::vector<std::vector<brayns::Capsule>> primitives,
    std::vector<std::vector<NeuronSectionMapping>> map)
    : _ids(std::move(ids))
    , _morphologies(MorphologyGeometryInitializer::init(std::move(primitives), std::move(map)))
{
}

brayns::Bounds MorphologyCircuitComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    brayns::Bounds result;
    for (const auto &morphology : _morphologies)
    {
        const auto &geometry = morphology.geometry;
        const auto morphologyBounds = geometry.computeBounds(transform);
        result.expand(morphologyBounds);
    }

    return result;
}

void MorphologyCircuitComponent::onCreate()
{
    auto &model = getModel();
    model.addComponent<brayns::MaterialComponent>();

    std::vector<ospray::cpp::GeometricModel> handles;
    handles.reserve(_morphologies.size());
    auto &group = model.getGroup();
    for (auto &morphology : _morphologies)
    {
        handles.push_back(morphology.view.getHandle());
    }
    group.setGeometry(handles);
}

bool MorphologyCircuitComponent::commit()
{
    bool needsCommit = _colorsDirty || _geometryDirty;
    _colorsDirty = _geometryDirty = false;

    auto &material = brayns::ExtractComponent::material(getModel());
    if (material.commit())
    {
        for (auto &morphology : _morphologies)
        {
            morphology.view.setMaterial(material);
        }
        needsCommit = true;
    }

    if (needsCommit)
    {
        for (auto &morphology : _morphologies)
        {
            morphology.geometry.commit();
            morphology.view.commit();
        }
    }

    return needsCommit;
}

void MorphologyCircuitComponent::onInspect(const brayns::InspectContext &context, brayns::JsonObject &writeResult)
    const noexcept
{
    auto &model = context.model;
    auto modelHandle = model.handle();
    auto morphBegin = _morphologies.begin();
    auto morphEnd = _morphologies.end();
    auto it = std::find_if(
        morphBegin,
        morphEnd,
        [=](const MorphologyGeometry &morphology) { return morphology.view.getHandle().handle() == modelHandle; });

    if (it == morphEnd)
    {
        return;
    }

    auto index = std::distance(morphBegin, it);
    auto cellId = _ids[index];
    writeResult.set("neuron_id", cellId);
}

const std::vector<uint64_t> &MorphologyCircuitComponent::getIDs() const noexcept
{
    return _ids;
}

void MorphologyCircuitComponent::setColor(const brayns::Vector4f &color) noexcept
{
    for (auto &morphology : _morphologies)
    {
        morphology.view.setColor(color);
    }
    _colorsDirty = true;
}

void MorphologyCircuitComponent::setColorById(const std::vector<brayns::Vector4f> &colors)
{
    if (colors.size() < _morphologies.size())
    {
        throw std::invalid_argument("Not enough colors provided");
    }

    for (size_t i = 0; i < _morphologies.size(); ++i)
    {
        auto &morphology = _morphologies[i];
        morphology.view.setColor(colors[i]);
    }
    _colorsDirty = true;
}

std::vector<uint64_t> MorphologyCircuitComponent::setColorById(
    const std::map<uint64_t, brayns::Vector4f> &colors) noexcept
{
    auto skipped = ColorByIDAlgorithm::execute(
        colors,
        _ids,
        [&](uint64_t id, size_t index, const brayns::Vector4f &color)
        {
            (void)id;
            auto &morphology = _morphologies[index];
            morphology.view.setColor(color);
            _colorsDirty = true;
        });

    return skipped;
}

void MorphologyCircuitComponent::setColorBySection(
    const std::vector<std::pair<NeuronSection, brayns::Vector4f>> &sectionColorList) noexcept
{
    std::vector<std::pair<NeuronSection, uint8_t>> sectionIndices;
    sectionIndices.reserve(sectionColorList.size());

    std::vector<brayns::Vector4f> colors;
    colors.reserve(sectionColorList.size());

    // Store the colors in the buffer and assign an index to each
    for (const auto &entry : sectionColorList)
    {
        const auto sectionType = entry.first;
        const auto &sectionColor = entry.second;

        const auto index = static_cast<uint8_t>(colors.size());
        colors.push_back(sectionColor);
        sectionIndices.push_back(std::make_pair(sectionType, index));
    }

    auto colorData = ospray::cpp::CopiedData(colors);

    // Apply the appropiate color index to the appropiate section on each morphology
    for (auto &morphology : _morphologies)
    {
        auto &geometry = morphology.geometry;
        std::vector<uint8_t> indices(geometry.numPrimitives(), 0u);
        auto &sections = morphology.sections;

        for (const auto &entry : sectionIndices)
        {
            const auto sectionType = entry.first;
            const auto index = entry.second;

            for (const auto &section : sections)
            {
                if (section.type == sectionType)
                {
                    auto start = indices.begin() + section.begin;
                    auto end = indices.begin() + section.end;
                    std::fill(start, end, index);
                }
            }
        }
        auto indexData = ospray::cpp::CopiedData(indices);

        morphology.view.setColorMap(colorData, indexData);
    }

    _colorsDirty = true;
}

void MorphologyCircuitComponent::setIndexedColor(
    const std::vector<brayns::Vector4f> &colors,
    const std::vector<uint8_t> &map)
{
    assert(colors.size() <= 256);

    auto colorData = ospray::cpp::CopiedData(colors);

    size_t mappingOffset = 0;
    for (auto &morphology : _morphologies)
    {
        auto &geometry = morphology.geometry;
        auto geometrySize = geometry.numPrimitives();

        if (mappingOffset + geometrySize > map.size())
        {
            throw std::invalid_argument("Not enough mapping data provided");
        }

        auto morphologyMapping = &map[mappingOffset];
        auto mappingData = ospray::cpp::CopiedData(morphologyMapping, geometrySize);

        morphology.view.setColorMap(colorData, mappingData);
        mappingOffset += geometrySize;
    }
    _colorsDirty = true;
}

void MorphologyCircuitComponent::changeThickness(float multiplier) noexcept
{
    for (auto &morphology : _morphologies)
    {
        auto &geometry = morphology.geometry;
        geometry.forEach(
            [=](brayns::Capsule &primitive)
            {
                primitive.r0 *= multiplier;
                primitive.r1 *= multiplier;
            });
    }

    _geometryDirty = true;
}
