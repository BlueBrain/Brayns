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

#include <brayns/engine/components/Volumes.h>
#include <brayns/engine/systems/GenericBoundsSystem.h>
#include <brayns/engine/systems/VolumeCommitSystem.h>
#include <brayns/engine/systems/VolumeInitSystem.h>
#include <brayns/engine/volume/types/RegularVolume.h>

#include <brayns/utils/FileReader.h>
#include <brayns/utils/parsing/Parser.h>
#include <brayns/utils/string/StringExtractor.h>
#include <brayns/utils/string/StringSplitter.h>
#include <brayns/utils/string/StringTrimmer.h>

#include <filesystem>
#include <unordered_map>

namespace
{
class MhdParser
{
public:
    static std::unordered_map<std::string, std::string> parse(const std::string &filename)
    {
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

class MhdRequiredKeys
{
public:
    inline static const std::vector<std::string> requiredKeys =
        {"ObjectType", "DimSize", "ElementSpacing", "ElementType", "ElementDataFile"};

    static void check(const std::unordered_map<std::string, std::string> &mhd)
    {
        for (auto &key : requiredKeys)
        {
            if (mhd.find(key) == mhd.end())
            {
                throw std::runtime_error("Incomplete MHD file. Missing " + key);
            }
        }
    }
};

class MhdDataTypeConverter
{
public:
    inline static const std::unordered_map<std::string, brayns::VolumeDataType> converter = {
        {"MET_FLOAT", brayns::VolumeDataType::Float},
        {"MET_DOUBLE", brayns::VolumeDataType::Double},
        {"MET_UCHAR", brayns::VolumeDataType::UnsignedChar},
        {"MET_USHORT", brayns::VolumeDataType::UnsignedShort},
        {"MET_SHORT", brayns::VolumeDataType::Short}};

public:
    static brayns::VolumeDataType convert(const std::string &dataType)
    {
        auto it = converter.find(dataType);
        if (it == converter.end())
        {
            throw std::invalid_argument("Unsupported data type: " + dataType);
        }

        return it->second;
    }
};

class MhdRawFilePathResolver
{
public:
    static std::string resolve(const std::string filename, const std::string mhdFilePath)
    {
        auto filePath = std::filesystem::path(filename);
        if (!filePath.is_absolute())
        {
            auto basePath = std::filesystem::path(mhdFilePath).parent_path();
            filePath = std::filesystem::canonical(basePath / filePath);
        }
        return filePath.string();
    }
};

class VoxelBuilder
{
public:
    static brayns::RegularVolume fromBlob(const brayns::Blob &blob, const brayns::RawVolumeLoaderParameters &params)
    {
        _checkDataSize(blob.data.size(), params);

        auto volume = brayns::RegularVolume();
        volume.dataType = params.data_type;
        volume.size = params.dimensions;
        volume.spacing = params.spacing;
        volume.voxels = blob.data;
        return volume;
    }

private:
    static void _checkDataSize(size_t dataSize, const brayns::RawVolumeLoaderParameters &params)
    {
        auto linealSize = glm::compMul(params.dimensions);
        if (linealSize == 0)
        {
            throw std::invalid_argument("Volume dimensions are empty");
        }

        auto expectedSize = linealSize * _getTypeByteSize(params.data_type);
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
    explicit ModelBuilder(brayns::Model &model)
        : _components(model.getComponents())
        , _systems(model.getSystems())
    {
    }

    void addVolume(brayns::RegularVolume volume)
    {
        _components.add<brayns::Volumes>(std::move(volume));
    }

    void addSystems()
    {
        _systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Volumes>>();
        _systems.setCommitSystem<brayns::VolumeCommitSystem>();
        _systems.setInitSystem<brayns::VolumeInitSystem>();
    }

private:
    brayns::Components &_components;
    brayns::Systems &_systems;
};

} // namespace

namespace brayns
{
std::vector<std::shared_ptr<Model>> RawVolumeLoader::importFromBlob(
    const Blob &blob,
    const LoaderProgress &callback,
    const RawVolumeLoaderParameters &parameters) const
{
    callback.updateProgress("Parsing voxels", 0.f);
    auto volume = VoxelBuilder::fromBlob(blob, parameters);

    callback.updateProgress("Building model", 0.5f);
    auto model = std::make_shared<Model>("Volume");
    auto builder = ModelBuilder(*model);
    builder.addVolume(std::move(volume));
    builder.addSystems();

    callback.updateProgress("Done", 1.f);
    auto result = std::vector<std::shared_ptr<Model>>();
    result.push_back(std::move(model));
    return result;
}

std::vector<std::shared_ptr<Model>> RawVolumeLoader::importFromFile(
    const std::string &filename,
    const LoaderProgress &callback,
    const RawVolumeLoaderParameters &parameters) const
{
    auto fileData = brayns::FileReader::read(filename);
    auto src = reinterpret_cast<const uint8_t *>(fileData.data());

    auto blob = Blob();
    blob.name = filename;
    blob.type = "raw";
    blob.data = std::vector<uint8_t>(src, src + fileData.size());
    return importFromBlob(blob, callback, parameters);
}

std::string RawVolumeLoader::getName() const
{
    return "raw-volume";
}

std::vector<std::string> RawVolumeLoader::getSupportedExtensions() const
{
    return {"raw"};
}

std::vector<std::shared_ptr<Model>> MHDVolumeLoader::importFromBlob(const Blob &blob, const LoaderProgress &callback)
    const
{
    (void)blob;
    (void)callback;
    throw std::runtime_error("MHD Volume loading from blob is not supported");
}

std::vector<std::shared_ptr<Model>> MHDVolumeLoader::importFromFile(
    const std::string &filename,
    const LoaderProgress &callback) const
{
    auto mhd = MhdParser::parse(filename);
    MhdRequiredKeys::check(mhd);

    auto &objectType = mhd["ObjectType"];
    if (objectType != "Image")
    {
        throw std::runtime_error("Wrong object type for mhd file");
    }

    auto rawDimensions = std::string_view(mhd["DimSize"]);
    auto rawSpacing = std::string_view(mhd["ElementSpacing"]);
    auto dimensions = Parser::extractToken<Vector3ui>(rawDimensions);
    auto spacing = Parser::extractToken<Vector3f>(rawSpacing);
    auto dataType = MhdDataTypeConverter::convert(mhd["ElementType"]);
    auto volumeFilePath = MhdRawFilePathResolver::resolve(mhd["ElementDataFile"], filename);

    auto params = RawVolumeLoaderParameters();
    params.dimensions = dimensions;
    params.spacing = spacing;
    params.data_type = dataType;

    return RawVolumeLoader().importFromFile(volumeFilePath, callback, params);
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
