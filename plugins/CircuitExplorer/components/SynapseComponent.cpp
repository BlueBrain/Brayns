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

#include "SynapseComponent.h"

#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/MaterialComponent.h>

#include <api/coloring/ColorByIDAlgorithm.h>

SynapseComponent::SynapseComponent(std::map<uint64_t, std::vector<brayns::Sphere>> &synapses)
{
    _cellIds.reserve(synapses.size());
    _synapses.reserve(synapses.size());

    for (auto &[id, synapseGeometry] : synapses)
    {
        _cellIds.push_back(id);
        _synapses.emplace_back(std::move(synapseGeometry));
    }
}

brayns::Bounds SynapseComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    brayns::Bounds result;
    for (const auto &synapse : _synapses)
    {
        const auto synapseBounds = synapse.computeBounds(transform);
        result.expand(synapseBounds);
    }
    return result;
}

bool SynapseComponent::commit()
{
    bool needsCommit = _colorsDirty;
    _colorsDirty = false;

    auto &material = brayns::ExtractModelObject::extractMaterial(getModel());
    if (material.commit())
    {
        for (auto &synapse : _synapses)
        {
            synapse.setMaterial(material);
            needsCommit = true;
        }
    }

    if (needsCommit)
    {
        for (auto &synapse : _synapses)
        {
            synapse.commit();
        }
    }

    return needsCommit;
}

void SynapseComponent::onCreate()
{
    auto &model = getModel();
    model.addComponent<brayns::MaterialComponent>();

    auto &group = model.getGroup();
    for (auto &synapse : _synapses)
    {
        group.addGeometry(synapse);
    }
}

void SynapseComponent::onDestroy()
{
    auto &model = getModel();
    auto &group = model.getGroup();
    for (auto &synapse : _synapses)
    {
        group.removeGeometry(synapse);
    }
}

const std::vector<uint64_t> &SynapseComponent::getCellIds() const noexcept
{
    return _cellIds;
}

void SynapseComponent::setColor(const brayns::Vector4f &color) noexcept
{
    for (auto &synapse : _synapses)
    {
        synapse.setColor(color);
    }
    _colorsDirty = true;
}

void SynapseComponent::setColorById(const std::vector<brayns::Vector4f> &colors)
{
    for (size_t i = 0; i < _synapses.size(); ++i)
    {
        auto &synapse = _synapses[i];
        synapse.setColor(colors[i]);
    }
    _colorsDirty = true;
}

std::vector<uint64_t> SynapseComponent::setColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
{
    auto skipped = ColorByIDAlgorithm::execute(
        colorMap,
        _cellIds,
        [&](uint64_t id, size_t index, const brayns::Vector4f &color)
        {
            (void)id;
            auto &synapse = _synapses[index];
            synapse.setColor(color);
            _colorsDirty = true;
        });

    return skipped;
}

void SynapseComponent::setIndexedColor(const std::vector<brayns::Vector4f> &color, const std::vector<uint8_t> &mapping)
{
    if (color.size() > 256)
    {
        throw std::invalid_argument("Colormap has more than 256 values");
    }

    auto colorData = ospray::cpp::CopiedData(color);
    size_t mappingOffset = 0;
    for (auto &synapses : _synapses)
    {
        auto geometrySize = synapses.getNumPrimitives();

        if (mappingOffset + geometrySize < mapping.size())
        {
            throw std::invalid_argument("Not enough mapping data provided");
        }

        auto morphologyMapping = &mapping[mappingOffset];
        auto mappingData = ospray::cpp::CopiedData(morphologyMapping, geometrySize);
        synapses.setColorMap(colorData, mappingData);
    }
    _colorsDirty = true;
}
