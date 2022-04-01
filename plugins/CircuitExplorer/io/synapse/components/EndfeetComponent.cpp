#include "EndfeetComponent.h"

#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/common/SizeHelper.h>
#include <brayns/engine/components/MaterialComponent.h>

size_t EndfeetComponent::getSizeInBytes() const noexcept
{
    size_t endfeetSize = 0;
    for(const auto &endfoot : _endFeet)
    {
        const auto &geometry = endfoot.geometry;
        endfeetSize += geometry.getSizeInBytes();
    }

    return sizeof(EndfeetComponent)
            + brayns::SizeHelper::vectorSize(_endFeet)
            + brayns::SizeHelper::vectorSize(_astrocyteIds);
}

brayns::Bounds EndfeetComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    brayns::Bounds result;
    for(const auto &endfoot : _endFeet)
    {
        const auto &geometry = endfoot.geometry;
        const auto endFootBounds = geometry.computeBounds(transform);
        result.expand(endFootBounds);
    }
    return result;
}

void EndfeetComponent::onStart()
{
    auto &model = getModel();
    model.addComponent<brayns::MaterialComponent>();
}

bool EndfeetComponent::commit()
{
    bool needsCommit = _colorsDirty;

    auto &material = brayns::ExtractModelObject::extractMaterial(getModel());
    if(material.commit())
    {
        for(auto &endfoot : _endFeet)
        {
            auto model = endfoot.model;
            brayns::GeometricModelHandler::setMaterial(model, material);
            needsCommit = true;
        }
    }

    if(needsCommit)
    {
        for(auto &endfoot : _endFeet)
        {
            auto model = endfoot.model;
            brayns::GeometricModelHandler::commitModel(model);
        }
    }

    return needsCommit;
}

void EndfeetComponent::onDestroyed()
{
    auto &group = getModel();
    for(auto &endfoot : _endFeet)
    {
        auto &model = endfoot.model;
        brayns::GeometricModelHandler::removeFromGeometryGroup(model, group);
        brayns::GeometricModelHandler::destory(model);
    }
}

const std::vector<uint64_t> &EndfeetComponent::getAstroctyeIds() const noexcept
{
    return _astrocyteIds;
}

void EndfeetComponent::addEndfeet(uint64_t endfootId, const std::vector<brayns::TriangleMesh> &endfeetGeometry)
{
    if(endfeetGeometry.empty())
    {
        return;
    }

    _astrocyteIds.push_back(endfootId);
    _endFeet.emplace_back();

    auto &endfoot = _endFeet.back();
    auto &geometry = endfoot.geometry;
    auto &model = endfoot.model;

    size_t startMergeIndex = 0;
    if(geometry.getNumGeometries() == 0)
    {
        geometry.add(endfeetGeometry.front());
        startMergeIndex = 1;
    }

    for(size_t i = startMergeIndex; i < endfeetGeometry.size(); ++i)
    {
        const auto &currentMesh = endfeetGeometry[i];
        geometry.manipulate(0, [&](brayns::TriangleMesh& mesh)
        {
            brayns::TriangleMeshMerger::merge(currentMesh, mesh);
        });
    }

    model = brayns::GeometricModelHandler::create();
    brayns::GeometricModelHandler::setGeometry(model, geometry);
}

void EndfeetComponent::setColor(const brayns::Vector4f &color) noexcept
{
    for(auto &endfoot : _endFeet)
    {
        auto model = endfoot.model;
        brayns::GeometricModelHandler::setColor(model, color);
    }
    _colorsDirty = true;
}

void EndfeetComponent::setColorById(const std::vector<brayns::Vector4f> &colors)
{
    for(size_t i = 0; i < _endFeet.size(); ++i)
    {
        const auto &color = colors[i];
        auto &endfoot = _endFeet[i];
        auto model = endfoot.model;
        brayns::GeometricModelHandler::setColor(model, color);
    }
    _colorsDirty = true;
}

void EndfeetComponent::setColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
{
    auto idIt = _astrocyteIds.begin();
    auto efIt = _endFeet.begin();
    auto colorsIt = colorMap.begin();

    while(colorsIt != colorMap.end())
    {
        const auto targetId = colorsIt->first;
        const auto &targetColor = colorsIt->second;

        while(idIt != _astrocyteIds.end())
        {
            if(*idIt != targetId)
            {
                ++idIt;
                ++efIt;
            }
        }

        if(idIt == _astrocyteIds.end())
        {
            break;
        }

        _colorsDirty = true;
        auto &endfoot = *efIt;
        auto model = endfoot.model;
        brayns::GeometricModelHandler::setColor(model, targetColor);

        ++colorsIt;
    }
}
