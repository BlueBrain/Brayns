#include "MorphologyCircuitComponent.h"

#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/common/SizeHelper.h>
#include <brayns/engine/components/MaterialComponent.h>

size_t MorphologyCircuitComponent::getSizeInBytes() const noexcept
{
    size_t morphologiesSize = 0;
    for (const auto &morphology : _morphologies)
    {
        const auto &geometry = morphology.geometry;
        const auto &sections = morphology.sections;
        morphologiesSize += sizeof(MorphologyGeometry);
        morphologiesSize += geometry.getSizeInBytes();
        morphologiesSize += brayns::SizeHelper::vectorSize(sections);
    }

    return sizeof(MorphologyCircuitComponent) + morphologiesSize + brayns::SizeHelper::vectorSize(_ids);
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

void MorphologyCircuitComponent::onStart()
{
    auto &model = getModel();
    model.addComponent<brayns::MaterialComponent>();
}

bool MorphologyCircuitComponent::commit()
{
    bool needsCommit = false;

    if (_colorsDirty)
    {
        needsCommit = true;
        _colorsDirty = false;
    }

    auto &material = brayns::ExtractModelObject::extractMaterial(getModel());
    if (material.commit())
    {
        for (auto &morphology : _morphologies)
        {
            brayns::GeometricModelHandler::setMaterial(morphology.model, material);
        }
        needsCommit = true;
    }

    if (_geometryDirty)
    {
        _geometryDirty = false;
        needsCommit = true;

        for (auto &morphology : _morphologies)
        {
            auto &geometry = morphology.geometry;
            geometry.commit();
        }
    }

    if (needsCommit)
    {
        for (auto &morphology : _morphologies)
        {
            brayns::GeometricModelHandler::commitModel(morphology.model);
        }
    }

    return needsCommit;
}

void MorphologyCircuitComponent::onDestroyed()
{
    auto &group = getModel();
    for (auto &morphology : _morphologies)
    {
        auto &model = morphology.model;
        brayns::GeometricModelHandler::removeFromGeometryGroup(model, group);
        brayns::GeometricModelHandler::destory(model);
    }
}

void MorphologyCircuitComponent::setMorphologies(
    std::vector<uint64_t> ids,
    std::vector<std::vector<brayns::Primitive>> primitives,
    std::vector<std::vector<NeuronSectionMapping>> map) noexcept
{
    auto &group = getModel();

    _ids = std::move(ids);
    _morphologies.resize(_ids.size());

    for (size_t i = 0; i < _ids.size(); ++i)
    {
        auto &morphGeometry = primitives[i];
        auto &morphSections = map[i];

        auto &morphology = _morphologies[i];
        auto &model = morphology.model;
        auto &geometry = morphology.geometry;
        auto &sections = morphology.sections;

        geometry.set(std::move(morphGeometry));
        geometry.commit();

        sections = std::move(morphSections);

        model = brayns::GeometricModelHandler::create();
        brayns::GeometricModelHandler::addToGeometryGroup(model, group);
        brayns::GeometricModelHandler::setGeometry(model, geometry);
        brayns::GeometricModelHandler::setColor(model, brayns::Vector4f(1.f));
    }
}

const std::vector<uint64_t> &MorphologyCircuitComponent::getIDs() const noexcept
{
    return _ids;
}

void MorphologyCircuitComponent::setColor(const brayns::Vector4f &color) noexcept
{
    for (auto &morphology : _morphologies)
    {
        auto model = morphology.model;
        brayns::GeometricModelHandler::setColor(model, color);
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
        auto model = morphology.model;
        const auto &color = colors[i];
        brayns::GeometricModelHandler::setColor(model, color);
    }
    _colorsDirty = true;
}

std::vector<uint64_t> MorphologyCircuitComponent::setColorById(
    const std::map<uint64_t, brayns::Vector4f> &colors) noexcept
{
    auto idIt = _ids.begin();
    auto morphIt = _morphologies.begin();
    auto colorsIt = colors.begin();

    std::vector<uint64_t> skipped;
    skipped.reserve(_ids.size());

    while (colorsIt != colors.end())
    {
        const auto targetId = colorsIt->first;
        const auto &targetColor = colorsIt->second;

        while (idIt != _ids.end())
        {
            const auto morphId = *idIt;
            if (morphId != targetId)
            {
                skipped.push_back(morphId);
                ++idIt;
                ++morphIt;
            }
        }

        if (idIt == _ids.end())
        {
            break;
        }

        _colorsDirty = true;
        auto &geometry = *morphIt;
        auto model = geometry.model;
        brayns::GeometricModelHandler::setColor(model, targetColor);

        ++colorsIt;
    }

    skipped.shrink_to_fit();
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

    auto colorData = brayns::DataHandler::copyBuffer(colors, OSPDataType::OSP_VEC4F);

    // Apply the appropiate color index to the appropiate section on each morphology
    for (auto &morphology : _morphologies)
    {
        const auto &geometry = morphology.geometry;
        std::vector<uint8_t> indices(geometry.getNumGeometries(), 0u);
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

        auto indexData = brayns::DataHandler::copyBuffer(indices, OSPDataType::OSP_UCHAR);

        auto model = morphology.model;
        brayns::GeometricModelHandler::setColorMap(model, colorData, indexData);
    }

    _colorsDirty = true;
}

void MorphologyCircuitComponent::setIndexedColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &map)
{
    if (color.size > 256)
    {
        throw std::invalid_argument("Colormap has more than 256 values");
    }

    size_t mappingOffset = 0;
    for (auto &morphology : _morphologies)
    {
        auto model = morphology.model;
        auto &geometry = morphology.geometry;
        auto geometrySize = geometry.getNumGeometries();

        if (mappingOffset + geometrySize > map.size())
        {
            throw std::invalid_argument("Not enough mapping data provided");
        }

        auto morphologyMapping = &map[mappingOffset];
        auto mappingData = brayns::DataHandler::copyBuffer(morphologyMapping, geometrySize, OSPDataType::OSP_UCHAR);
        brayns::GeometricModelHandler::setColorMap(model, color, mappingData);
        mappingOffset += geometrySize;
    }
    _colorsDirty = true;
}

void MorphologyCircuitComponent::changeThickness(const float multiplier) noexcept
{
    for (auto &morphology : _morphologies)
    {
        auto &geometry = morphology.geometry;
        geometry.mainpulateAll(
            [mult = multiplier](uint32_t i, brayns::Primitive &primitive)
            {
                (void)i;
                primitive.r0 *= mult;
                primitive.r1 *= mult;
            });
    }

    _geometryDirty = true;
}
