/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "VolumeLoader.h"

#include <brayns/engine/volume/types/RegularVolume.h>
#include <brayns/utils/FileReader.h>
#include <brayns/utils/string/StringExtractor.h>
#include <brayns/utils/string/StringSplitter.h>
#include <brayns/utils/string/StringTrimmer.h>

#include <unordered_map>

namespace
{
class MhdParser
{
public:
    static std::unordered_map<std::string, std::string> parse(const std::string &filename)
    {
        // Sample MHD File:
        //
        // ObjectType = Image
        // DimSize = 1 2 3
        // ElementSpacing = 0.1 0.2 0.3
        // ElementType = MET_USHORT
        // ElementDataFile = BS39.raw

        auto content = brayns::FileReader::read(filename);
        auto view = std::string_view(content);

        auto result = std::unordered_map<std::string, std::string>();

        auto ctr = 1ul;
        while (!view.empty())
        {
            auto line = brayns::StringExtractor::extractLine(view);
            brayns::StringExtractor::extract(view, 1);

            auto keyAndValue = brayns::StringSplitter::split(line, "=");
            if (keyAndValue.size() != 2)
            {
                throw std::runtime_error("Could not parse line " + std::to_string(ctr));
            }

            keyAndValue[0] = std::string(brayns::StringTrimmer::trim(keyAndValue[0]));
            keyAndValue[1] = std::string(brayns::StringTrimmer::trim(keyAndValue[1]));
            result[keyAndValue[0]] = keyAndValue[1];
        }

        return result;
    }
};

class VolumeDataTypeConverter
{
public:
    inline static const std::unordered_map<std::string_view, brayns::VolumeDataType> converter = {
        {"MET_FLOAT", brayns::VolumeDataType::Float},
        {"MET_DOUBLE", brayns::VolumeDataType::Double},
        {"MET_UCHAR", brayns::VolumeDataType::UnsignedChar},
        {"MET_USHORT", brayns::VolumeDataType::UnsignedShort},
        {"MET_UINT", brayns::VolumeDataType::Double},
        {"MET_CHAR", brayns::VolumeDataType::Short},
        {"MET_SHORT", brayns::VolumeDataType::Short},
        {"MET_INT", brayns::VolumeDataType::Double}};

    static brayns::VolumeDataType fromMET(const std::string &type)
    {
        auto it = converter.find(type);

        if (it == converter.end())
        {
            throw std::runtime_error("Unknown data type " + type);
        }

        return it->second;
    }
};

class VoxelReader
{
public:
    static brayns::RegularVolume fromFile(const std::string &filename, const brayns::RawVolumeLoaderParameters &params)
    {
        auto fileData = brayns::FileReader::read(filename);
        _checkDataSize(fileData.size(), params);
    }

    static brayns::RegularVolume fromBlob(const brayns::Blob &blob, const brayns::RawVolumeLoaderParameters &params)
    {
        _checkDataSize(blob.data.size(), params);

        auto volume = brayns::RegularVolume();
        volume.dataType = params.type;
        volume.size = params.dimensions;
        volume.spacing = params.spacing;
        volume.voxels = blob.data;
        return volume;
    }

private:
    static void _checkDataSize(size_t dataSize, const brayns::RawVolumeLoaderParameters &params)
    {
        auto expectedSize = glm::compMul(params.dimensions) * _getTypeByteSize(params.type);
        if (expectedSize != dataSize)
        {
            throw std::invalid_argument("Data size and exptected size mismatch");
        }
    }

    static size_t _getTypeByteSize(brayns::VolumeDataType type)
    {
        switch (type)
        {
        case brayns::VolumeDataType::UnsignedChar:
            return 1;
        case brayns::VolumeDataType::HalfFloat:
        case brayns::VolumeDataType::Short:
        case brayns::VolumeDataType::UnsignedShort:
            return 2;
        case brayns::VolumeDataType::Float:
            return 4;
        default:
            return 8;
        }
    }
};

class ModelBuilder
{
public:
    ModelBuilder(brayns::Model &model)
        : _components(model.getComponents())
        , _systems(model.getSystems())
    {
    }

private:
    brayns::Components &_components;
    brayns::Systems &_systems;
};

class RawVolumeReader
{
public:
    static std::shared_ptr<brayns::Model> read(
        const std::string &filename,
        const brayns::LoaderProgress &callback,
        const brayns::RawVolumeLoaderParameters &params)
    {
        _checkParameters(params);

        callback.updateProgress("Parsing volume file ...", 0.f);

        auto model = std::make_shared<brayns::Model>();
        auto volume = model->createSharedDataVolume(params.dimensions, params.spacing, params.type);
        volume->setDataRange(dataRange);

        callback.updateProgress("Loading voxels ...", 0.5f);
        mapData(volume);

        callback.updateProgress("Adding model ...", 1.f);
        model->addVolume(volume);

        Transformation transformation;
        transformation.setRotationCenter(model->getBounds().getCenter());
        auto modelDescriptor = std::make_shared<ModelDescriptor>(
            std::move(model),
            filename,
            ModelMetadata{
                {"dimensions", to_string(params.dimensions)},
                {"element-spacing", to_string(params.spacing)}});
        modelDescriptor->setTransformation(transformation);
        return modelDescriptor;
    }

private:
    static void _checkParameters(const brayns::RawVolumeLoaderParameters &params)
    {
        if (glm::compMul(params.dimensions) == 0)
        {
            throw std::runtime_error("Volume dimensions are empty");
        }
    }
};

} // namespace

namespace brayns
{
std::vector<std::shared_ptr<Model>> RawVolumeLoader::importFromBlob(
    const Blob &blob,
    const LoaderProgress &callback,
    const RawVolumeLoaderParameters &properties) const
{
    return {_loadVolume(
        blob.name,
        callback,
        properties,
        [&blob](auto volume) { volume->mapData(blob.data); },
        scene)};
}

std::vector<std::shared_ptr<Model>> RawVolumeLoader::importFromFile(
    const std::string &filename,
    const LoaderProgress &callback,
    const RawVolumeLoaderParameters &properties) const
{
    return {_loadVolume(
        filename,
        callback,
        properties,
        [filename](auto volume) { volume->mapData(filename); },
        scene)};
}

std::string RawVolumeLoader::getName() const
{
    return "raw-volume";
}

std::vector<std::string> RawVolumeLoader::getSupportedExtensions() const
{
    return {"raw"};
}

std::vector<ModelDescriptorPtr> MHDVolumeLoader::importFromBlob(const Blob &, const LoaderProgress &, Scene &) const
{
    throw std::runtime_error("Volume loading from blob is not supported");
}

std::vector<ModelDescriptorPtr>
    MHDVolumeLoader::importFromFile(const std::string &filename, const LoaderProgress &callback, Scene &scene) const
{
    std::string volumeFile = filename;
    const auto mhd = parseMHD(filename);

    // Check all keys present
    for (const auto key : {"ObjectType", "DimSize", "ElementSpacing", "ElementType", "ElementDataFile"})
        if (mhd.find(key) == mhd.end())
            throw std::runtime_error("Missing key " + std::string(key));

    if (mhd.at("ObjectType") != "Image")
        throw std::runtime_error("Wrong object type for mhd file");

    const auto dimensions = parseVec3<int32_t>(mhd.at("DimSize"), [](const auto &s) { return stoi(s); });
    const auto spacing = parseVec3<double>(mhd.at("ElementSpacing"), [](const auto &s) { return stod(s); });
    const auto type = dataTypeFromMET(mhd.at("ElementType"));

    std::filesystem::path path = mhd.at("ElementDataFile");
    if (!path.is_absolute())
    {
        auto basePath = std::filesystem::path(filename).parent_path();
        path = std::filesystem::canonical(basePath / path);
    }
    volumeFile = path.string();

    RawVolumeLoaderParameters params;
    params.dimensions = dimensions;
    params.spacing = spacing;
    params.type = type;

    return RawVolumeLoader().importFromFile(volumeFile, callback, params, scene);
}

std::string MHDVolumeLoader::getName() const
{
    return "mhd-volume";
}

std::vector<std::string> MHDVolumeLoader::getSupportedExtensions() const
{
    return {"mhd"};
}
} // namespace brayns
