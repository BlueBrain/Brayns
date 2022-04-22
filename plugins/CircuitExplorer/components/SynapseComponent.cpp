#include "SynapseComponent.h"

#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/common/SizeHelper.h>
#include <brayns/engine/components/MaterialComponent.h>

#include <api/coloring/ColorByIDAlgorithm.h>

size_t SynapseComponent::getSizeInBytes() const noexcept
{
    size_t synapsesSize = brayns::SizeHelper::vectorSize(_synapses);
    for (const auto &synapse : _synapses)
    {
        auto &geometry = synapse.geometry;
        synapsesSize += geometry.getSizeInBytes();
    }

    return sizeof(SynapseComponent) + synapsesSize + brayns::SizeHelper::vectorSize(_cellIds);
}

brayns::Bounds SynapseComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    brayns::Bounds result;
    for (const auto &synapse : _synapses)
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
    _colorsDirty = false;

    auto &material = brayns::ExtractModelObject::extractMaterial(getModel());
    if (material.commit())
    {
        for (auto &synapse : _synapses)
        {
            auto model = synapse.model;
            brayns::GeometricModelHandler::setMaterial(model, material);
            needsCommit = true;
        }
    }

    if (needsCommit)
    {
        for (auto &synapse : _synapses)
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
    for (auto &synapse : _synapses)
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

void SynapseComponent::addSynapses(std::map<uint64_t, std::vector<brayns::Sphere>> &synapses)
{
    _cellIds.reserve(synapses.size());
    _synapses.reserve(synapses.size());

    auto &group = getModel();
    group.addComponent<brayns::MaterialComponent>();

    for (auto &[id, synapseGeometry] : synapses)
    {
        _cellIds.push_back(id);
        _synapses.emplace_back();
        auto &synapse = _synapses.back();
        auto &geometry = synapse.geometry;
        auto &model = synapse.model;

        geometry.set(std::move(synapseGeometry));
        geometry.commit();

        model = brayns::GeometricModelHandler::create();
        brayns::GeometricModelHandler::addToGeometryGroup(model, group);
        brayns::GeometricModelHandler::setGeometry(model, geometry);
        brayns::GeometricModelHandler::setColor(model, brayns::Vector4f(1.f));
    }
}

void SynapseComponent::setColor(const brayns::Vector4f &color) noexcept
{
    for (auto &synapse : _synapses)
    {
        auto model = synapse.model;
        brayns::GeometricModelHandler::setColor(model, color);
    }
    _colorsDirty = true;
}

void SynapseComponent::setColorById(const std::vector<brayns::Vector4f> &colors)
{
    for (size_t i = 0; i < _synapses.size(); ++i)
    {
        const auto &color = colors[i];
        auto &synapse = _synapses[i];
        auto model = synapse.model;
        brayns::GeometricModelHandler::setColor(model, color);
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
            auto model = synapse.model;
            brayns::GeometricModelHandler::setColor(model, color);
            _colorsDirty = true;
        });

    return skipped;
}

void SynapseComponent::setIndexedColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &mapping)
{
    if (color.size > 256)
    {
        throw std::invalid_argument("Colormap has more than 256 values");
    }

    size_t mappingOffset = 0;
    for (auto &synapses : _synapses)
    {
        auto model = synapses.model;
        auto &geometry = synapses.geometry;
        auto geometrySize = geometry.getNumGeometries();

        if (mappingOffset + geometrySize < mapping.size())
        {
            throw std::invalid_argument("Not enough mapping data provided");
        }

        auto morphologyMapping = &mapping[mappingOffset];
        auto mappingData = brayns::DataHandler::copyBuffer(morphologyMapping, geometrySize, OSPDataType::OSP_UCHAR);
        brayns::GeometricModelHandler::setColorMap(model, color, mappingData);
    }
    _colorsDirty = true;
}
