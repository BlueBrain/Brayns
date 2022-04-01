#include "VasculatureComponent.h"

#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/common/SizeHelper.h>
#include <brayns/engine/components/MaterialComponent.h>

size_t VasculatureComponent::getSizeInBytes() const noexcept
{
    return sizeof (VasculatureComponent)
            + _geometry.getSizeInBytes()
            + brayns::SizeHelper::vectorSize(_ids)
            + brayns::SizeHelper::vectorSize(_colors);
}

brayns::Bounds VasculatureComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    return _geometry.computeBounds(transform);
}

void VasculatureComponent::onStart()
{
    auto &model = getModel();

    _model = brayns::GeometricModelHandler::create();
    brayns::GeometricModelHandler::addToGeometryGroup(_model, model);

    model.addComponent<brayns::MaterialComponent>();
}

bool VasculatureComponent::commit()
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

void VasculatureComponent::onDestroyed()
{
    brayns::GeometricModelHandler::removeFromGeometryGroup(_model, getModel());
    brayns::GeometricModelHandler::destory(_model);
}

void VasculatureComponent::setNumVessels(const size_t size) noexcept
{
    _ids.reserve(size);
    _sections.reserve(size);
    _colors.reserve(size);
}

void VasculatureComponent::addVessel(uint64_t id, brayns::Primitive geometry, VasculatureSection section) noexcept
{
    _ids.push_back(id);
    _geometry.add(std::move(geometry));
    _sections.push_back(section);
    _colors.push_back(brayns::Vector4f(1.f));
}

const std::vector<uint64_t> &VasculatureComponent::getIDs() const noexcept
{
    return _ids;
}

void VasculatureComponent::setColor(const brayns::Vector4f &color) noexcept
{
    auto begin = _colors.begin();
    auto end = _colors.end();
    std::fill(begin, end, color);
    auto colorBuffer = brayns::DataHandler::shareBuffer(_colors, OSPDataType::OSP_VEC4F);
    brayns::GeometricModelHandler::setColors(_model, colorBuffer);
    _colorsDirty = true;
}

void VasculatureComponent::setColorBySection(
        const std::vector<std::pair<VasculatureSection, brayns::Vector4f>> &colormap) noexcept
{
    for(size_t i = 0; i < _colors.size(); ++i)
    {
        const auto section = _sections[i];
        for(const auto &entry : colormap)
        {
            const auto checkSection = entry.first;
            const auto &color = entry.second;
            if(checkSection == section)
            {
                _colors[i] = color;
                _colorsDirty = true;
            }
        }
    }

    if(_colorsDirty)
    {
        auto colorData = brayns::DataHandler::shareBuffer(_colors, OSPDataType::OSP_VEC4F);
        brayns::GeometricModelHandler::setColors(_model, colorData);
    }
}

void VasculatureComponent::setColorById(std::vector<brayns::Vector4f> colors) noexcept
{
    _colors = std::move(colors);
    auto colorBuffer = brayns::DataHandler::shareBuffer(_colors, OSPDataType::OSP_VEC4F);
    brayns::GeometricModelHandler::setColors(_model, colorBuffer);
    _colorsDirty = true;
}

void VasculatureComponent::setColorById(const std::map<uint64_t, brayns::Vector4f> &colors) noexcept
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

void VasculatureComponent::setSimulationColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &mapping) noexcept
{
    auto indexData = brayns::DataHandler::shareBuffer(mapping, OSPDataType::OSP_UCHAR);
    brayns::GeometricModelHandler::setColorMap(_model, color, indexData);
    _colorsDirty = true;
}
