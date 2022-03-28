#include "SynapseComponent.h"

#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/common/SizeHelper.h>

size_t SynapseComponent::getSizeInBytes() const noexcept
{
    size_t synapsesSize = brayns::SizeHelper::vectorSize(_synapses);
    for(const auto &synapse : _synapses)
    {
        auto &geometry = synapse.geometry;
        synapsesSize += geometry.getSizeInBytes();
    }

    return sizeof(SynapseComponent)
            + synapsesSize
            + brayns::SizeHelper::vectorSize(_cellIds);
}

brayns::Bounds SynapseComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    brayns::Bounds result;
    for(const auto &synapse : _synapses)
    {
        const auto &geometry = synapse.geometry;
        const auto synapseBounds = geometry.computeBounds(transform);
        result.expand(synapseBounds);
    }
    return result;
}

bool SynapseComponent::commit()
{
    bool needsCommit = _colorsDirty;

    auto &material = brayns::ExtractModelObject::extractMaterial(getModel());
    if(material.commit())
    {
        for(auto &synapse : _synapses)
        {
            auto model = synapse.model;
            brayns::GeometricModelHandler::setMaterial(model, material);
            needsCommit = true;
        }
    }

    if(needsCommit)
    {
        for(auto &synapse : _synapses)
        {
            auto model = synapse.model;
            brayns::GeometricModelHandler::commitModel(model);
        }
    }

    return needsCommit;
}

void SynapseComponent::onDestroyed()
{
    auto &group = getModel();
    for(auto &synapse : _synapses)
    {
        auto &model = synapse.model;
        brayns::GeometricModelHandler::removeFromGeometryGroup(model, group);
        brayns::GeometricModelHandler::destory(model);
    }
}

const std::vector<uint64_t> &SynapseComponent::getCellIds() const noexcept
{
    return _cellIds;
}

void SynapseComponent::setNumCells(const size_t size) noexcept
{
    _cellIds.reserve(size);
    _synapses.reserve(size);
}

void SynapseComponent::addSynapses(uint64_t cellId, std::vector<brayns::Sphere> synapseGeometry)
{
    _cellIds.push_back(cellId);
    _synapses.emplace_back();
    auto &synapse = _synapses.back();
    auto &geometry = synapse.geometry;
    auto &model = synapse.model;

    geometry.set(std::move(synapseGeometry));

    model = brayns::GeometricModelHandler::create();
    brayns::GeometricModelHandler::setGeometry(model, geometry);
}

void SynapseComponent::setColor(const brayns::Vector4f &color) noexcept
{
    for(auto &synapse : _synapses)
    {
        auto model = synapse.model;
        brayns::GeometricModelHandler::setColor(model, color);
    }
    _colorsDirty = true;
}

void SynapseComponent::setColorById(const std::vector<brayns::Vector4f> &colors)
{
    for(size_t i = 0; i < _synapses.size(); ++i)
    {
        const auto &color = colors[i];
        auto &synapse = _synapses[i];
        auto model = synapse.model;
        brayns::GeometricModelHandler::setColor(model, color);
    }
    _colorsDirty = true;
}

void SynapseComponent::setColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
{
    auto idIt = _cellIds.begin();
    auto synIt = _synapses.begin();
    auto colorsIt = colorMap.begin();

    while(colorsIt != colorMap.end())
    {
        const auto targetId = colorsIt->first;
        const auto &targetColor = colorsIt->second;

        while(idIt != _cellIds.end())
        {
            if(*idIt != targetId)
            {
                ++idIt;
                ++synIt;
            }
        }

        if(idIt == _cellIds.end())
        {
            break;
        }

        _colorsDirty = true;
        auto &synapse = *synIt;
        auto model = synapse.model;
        brayns::GeometricModelHandler::setColor(model, targetColor);

        ++colorsIt;
    }
}
