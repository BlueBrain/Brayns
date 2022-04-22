#include "EndfeetComponent.h"

#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/common/SizeHelper.h>
#include <brayns/engine/components/MaterialComponent.h>

size_t EndfeetComponent::getSizeInBytes() const noexcept
{
    size_t endfeetSize = 0;
    for (const auto &endfoot : _endFeet)
    {
        const auto &geometry = endfoot.geometry;
        endfeetSize += geometry.getSizeInBytes();
    }

    return sizeof(EndfeetComponent) + brayns::SizeHelper::vectorSize(_endFeet)
        + brayns::SizeHelper::vectorSize(_astrocyteIds);
}

brayns::Bounds EndfeetComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    brayns::Bounds result;
    for (const auto &endfoot : _endFeet)
    {
        const auto &geometry = endfoot.geometry;
        const auto endFootBounds = geometry.computeBounds(transform);
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
            auto model = endfoot.model;
            brayns::GeometricModelHandler::setMaterial(model, material);
            needsCommit = true;
        }
    }

    if (needsCommit)
    {
        for (auto &endfoot : _endFeet)
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
    for (auto &endfoot : _endFeet)
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

void EndfeetComponent::addEndfeet(std::map<uint64_t, std::vector<brayns::TriangleMesh>> &endfeetGeometry)
{
    _astrocyteIds.reserve(endfeetGeometry.size());
    _endFeet.reserve(endfeetGeometry.size());

    auto &group = getModel();

    group.addComponent<brayns::MaterialComponent>();

    for (auto &[astrocyteId, endfeets] : endfeetGeometry)
    {
        _astrocyteIds.push_back(astrocyteId);
        auto &endfoot = _endFeet.emplace_back();
        auto &geometry = endfoot.geometry;
        auto &model = endfoot.model;

        brayns::TriangleMesh mergedMeshes;
        for (auto &mesh : endfeets)
        {
            brayns::TriangleMeshMerger::merge(mesh, mergedMeshes);
        }

        geometry.add(std::move(mergedMeshes));
        geometry.commit();

        model = brayns::GeometricModelHandler::create();
        brayns::GeometricModelHandler::addToGeometryGroup(model, group);
        brayns::GeometricModelHandler::setGeometry(model, geometry);
        brayns::GeometricModelHandler::setColor(model, brayns::Vector4f(1.f));
    }
}

void EndfeetComponent::setColor(const brayns::Vector4f &color) noexcept
{
    for (auto &endfoot : _endFeet)
    {
        auto model = endfoot.model;
        brayns::GeometricModelHandler::setColor(model, color);
    }
    _colorsDirty = true;
}

void EndfeetComponent::setColorById(const std::vector<brayns::Vector4f> &colors)
{
    for (size_t i = 0; i < _endFeet.size(); ++i)
    {
        const auto &color = colors[i];
        auto &endfoot = _endFeet[i];
        auto model = endfoot.model;
        brayns::GeometricModelHandler::setColor(model, color);
    }
    _colorsDirty = true;
}

std::vector<uint64_t> EndfeetComponent::setColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
{
    auto idIt = _astrocyteIds.begin();
    auto efIt = _endFeet.begin();
    auto colorsIt = colorMap.begin();

    std::vector<uint64_t> skippedIds;
    skippedIds.reserve(_astrocyteIds.size());

    while (colorsIt != colorMap.end())
    {
        const auto targetId = colorsIt->first;
        const auto &targetColor = colorsIt->second;

        while (idIt != _astrocyteIds.end())
        {
            const auto astrocyteId = *idIt;
            if (astrocyteId != targetId)
            {
                skippedIds.push_back(astrocyteId);
                ++idIt;
                ++efIt;
            }
        }

        if (idIt == _astrocyteIds.end())
        {
            break;
        }

        _colorsDirty = true;
        auto &endfoot = *efIt;
        auto model = endfoot.model;
        brayns::GeometricModelHandler::setColor(model, targetColor);

        ++colorsIt;
    }

    skippedIds.shrink_to_fit();
    return skippedIds;
}
