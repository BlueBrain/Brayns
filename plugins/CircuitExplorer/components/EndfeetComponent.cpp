#include "EndfeetComponent.h"

#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/common/SizeHelper.h>
#include <brayns/engine/components/MaterialComponent.h>

#include <api/coloring/ColorByIDAlgorithm.h>

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
    auto skipped = ColorByIDAlgorithm::execute(
        colorMap,
        _astrocyteIds,
        [&](uint64_t id, size_t index, const brayns::Vector4f &color)
        {
            (void)id;
            auto &endFoot = _endFeet[index];
            auto model = endFoot.model;
            brayns::GeometricModelHandler::setColor(model, color);
            _colorsDirty = true;
        });

    return skipped;
}
