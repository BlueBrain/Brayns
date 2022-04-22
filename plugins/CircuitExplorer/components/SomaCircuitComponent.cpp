#include "SomaCircuitComponent.h"

#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/common/SizeHelper.h>
#include <brayns/engine/components/MaterialComponent.h>

size_t SomaCircuitComponent::getSizeInBytes() const noexcept
{
    return sizeof(SomaCircuitComponent) + _geometry.getSizeInBytes() + brayns::SizeHelper::vectorSize(_ids)
        + brayns::SizeHelper::vectorSize(_colors);
}

brayns::Bounds SomaCircuitComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    return _geometry.computeBounds(transform);
}

bool SomaCircuitComponent::commit()
{
    bool needsCommit = false;

    auto &material = brayns::ExtractModelObject::extractMaterial(getModel());
    if (material.commit())
    {
        brayns::GeometricModelHandler::setMaterial(_model, material);
        needsCommit = true;
    }

    needsCommit = needsCommit || _colorsDirty;
    _colorsDirty = false;

    if (needsCommit)
    {
        brayns::GeometricModelHandler::commitModel(_model);
    }

    return needsCommit;
}

void SomaCircuitComponent::onDestroyed()
{
    brayns::GeometricModelHandler::removeFromGeometryGroup(_model, getModel());
    brayns::GeometricModelHandler::destory(_model);
}

void SomaCircuitComponent::setSomas(std::vector<uint64_t> ids, std::vector<brayns::Sphere> geometry) noexcept
{
    _ids = std::move(ids);

    _model = brayns::GeometricModelHandler::create();

    auto &group = getModel();
    brayns::GeometricModelHandler::addToGeometryGroup(_model, group);

    auto &materialComponent = group.addComponent<brayns::MaterialComponent>();
    auto &material = materialComponent.getMaterial();
    material.commit();
    brayns::GeometricModelHandler::setMaterial(_model, material);

    _geometry.set(std::move(geometry));
    _geometry.commit();
    brayns::GeometricModelHandler::setGeometry(_model, _geometry);

    _colors.resize(_ids.size());
    setColor(brayns::Vector4f(1.f));
}

const std::vector<uint64_t> &SomaCircuitComponent::getIDs() const noexcept
{
    return _ids;
}

void SomaCircuitComponent::setColor(const brayns::Vector4f &color) noexcept
{
    std::fill(_colors.begin(), _colors.end(), color);
    auto colorBuffer = brayns::DataHandler::shareBuffer(_colors, OSPDataType::OSP_VEC4F);
    brayns::GeometricModelHandler::setColors(_model, colorBuffer);
    _colorsDirty = true;
}

void SomaCircuitComponent::setColorById(const std::vector<brayns::Vector4f> &colors)
{
    if (colors.size() < _geometry.getNumGeometries())
    {
        throw std::invalid_argument("Not enough colors for all geometry");
    }

    _colors = colors;

    auto colorBuffer = brayns::DataHandler::shareBuffer(_colors, OSPDataType::OSP_VEC4F);
    brayns::GeometricModelHandler::setColors(_model, colorBuffer);
    _colorsDirty = true;
}

std::vector<uint64_t> SomaCircuitComponent::setColorById(const std::map<uint64_t, brayns::Vector4f> &colors) noexcept
{
    auto idIt = _ids.begin();
    auto bufferIt = _colors.begin();
    auto colorsIt = colors.begin();

    std::vector<uint64_t> skipped;
    skipped.reserve(_ids.size());

    while (colorsIt != colors.end())
    {
        const auto targetId = colorsIt->first;
        const auto &targetColor = colorsIt->second;

        while (idIt != _ids.end())
        {
            const auto somaId = *idIt;
            if (somaId != targetId)
            {
                skipped.push_back(somaId);
                ++idIt;
                ++bufferIt;
            }
        }

        if (idIt == _ids.end())
        {
            break;
        }

        _colorsDirty = true;
        *bufferIt = targetColor;

        ++colorsIt;
    }

    if (_colorsDirty)
    {
        auto colorBuffer = brayns::DataHandler::shareBuffer(_colors, OSPDataType::OSP_VEC4F);
        brayns::GeometricModelHandler::setColors(_model, colorBuffer);
    }

    skipped.shrink_to_fit();
    return skipped;
}

void SomaCircuitComponent::setIndexedColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &mapping)
{
    if (color.size > 256)
    {
        throw std::invalid_argument("Color map has more than 256 values");
    }
    auto indexData = brayns::DataHandler::shareBuffer(mapping, OSPDataType::OSP_UCHAR);
    brayns::GeometricModelHandler::setColorMap(_model, color, indexData);
    _colorsDirty = true;
}
