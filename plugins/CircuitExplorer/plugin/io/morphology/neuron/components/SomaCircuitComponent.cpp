#include "SomaCircuitComponent.h"

#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/common/SizeHelper.h>

size_t SomaCircuitComponent::getSizeInBytes() const noexcept
{
    return sizeof (SomaCircuitComponent)
            + _geometry.getSizeInBytes()
            + brayns::SizeHelper::vectorSize(_ids)
            + brayns::SizeHelper::vectorSize(_colors);
}

brayns::Bounds SomaCircuitComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    return _geometry.computeBounds(transform);
}

void SomaCircuitComponent::onStart()
{
    _model = brayns::GeometricModelHandler::create();
    brayns::GeometricModelHandler::addToGeometryGroup(_model, getModel());
}

bool SomaCircuitComponent::commit()
{
    bool needsCommit = false;

    if(_geometry.commit())
    {
        brayns::GeometricModelHandler::setGeometry(_model, _geometry);
        needsCommit = true;
    }

    auto &material = brayns::ExtractModelObject::extractMaterial(getModel());
    if(material.commit())
    {
        brayns::GeometricModelHandler::setMaterial(_model, material);
        needsCommit = true;
    }

    needsCommit = needsCommit || _colorsDirty;
    _colorsDirty = false;

    if(needsCommit)
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

void SomaCircuitComponent::setNumCells(const size_t size) noexcept
{
    _ids.reserve(size);
    _colors.reserve(size);
}

void SomaCircuitComponent::addSoma(uint64_t id, brayns::Sphere geometry) noexcept
{
    _ids.push_back(id);
    _geometry.add(std::move(geometry));
    _colors.push_back(brayns::Vector4f(1.f));
}

const std::vector<uint64_t> &SomaCircuitComponent::getIDs() const noexcept
{
    return _ids;
}

void SomaCircuitComponent::setColor(const brayns::Vector4f &color) noexcept
{
    auto begin = _colors.begin();
    auto end = _colors.end();
    std::fill(begin, end, color);
    auto colorBuffer = brayns::DataHandler::shareBuffer(_colors, OSPDataType::OSP_VEC4F);
    brayns::GeometricModelHandler::setColors(_model, colorBuffer);
    _colorsDirty = true;
}

void SomaCircuitComponent::setColorById(std::vector<brayns::Vector4f> colors) noexcept
{
    _colors = std::move(colors);
    auto colorBuffer = brayns::DataHandler::shareBuffer(_colors, OSPDataType::OSP_VEC4F);
    brayns::GeometricModelHandler::setColors(_model, colorBuffer);
    _colorsDirty = true;
}

void SomaCircuitComponent::setColorById(const std::map<uint64_t, brayns::Vector4f> &colors) noexcept
{

    auto idIt = _ids.begin();
    auto bufferIt = _colors.begin();
    auto colorsIt = colors.begin();

    while(colorsIt != colors.end())
    {
        const auto targetId = colorsIt->first;
        const auto &targetColor = colorsIt->second;

        while(idIt != _ids.end())
        {
            if(*idIt != targetId)
            {
                ++idIt;
                ++bufferIt;
            }
        }

        if(idIt == _ids.end())
        {
            break;
        }

        _colorsDirty = true;
        *bufferIt = targetColor;

        ++colorsIt;
    }

    if(_colorsDirty)
    {
        auto colorBuffer = brayns::DataHandler::shareBuffer(_colors, OSPDataType::OSP_VEC4F);
        brayns::GeometricModelHandler::setColors(_model, colorBuffer);
    }
}

void SomaCircuitComponent::setSimulationColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &mapping) noexcept
{
    auto indexData = brayns::DataHandler::shareBuffer(mapping, OSPDataType::OSP_UCHAR);
    brayns::GeometricModelHandler::setColorMap(_model, color, indexData);
    _colorsDirty = true;
}
