/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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
#include <brayns/engine/systems/VolumeDataSystem.h>
#include <brayns/engine/volume/types/RegularVolume.h>

#include <brayns/utils/FileReader.h>
#include <brayns/utils/parsing/FileStream.h>
#include <brayns/utils/parsing/Parser.h>
#include <brayns/utils/string/StringExtractor.h>
#include <brayns/utils/string/StringSplitter.h>
#include <brayns/utils/string/StringTrimmer.h>

#include <filesystem>
#include <unordered_map>

namespace
{
class MhdRequiredKeys
{
public:
    static inline const std::vector<std::string> requiredKeys =
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

class MhdParser
{
public:
    static std::unordered_map<std::string, std::string> parse(const std::string &filename)
    {
        auto content = brayns::FileReader::read(filename);
        auto stream = brayns::FileStream(content);

        auto result = std::unordered_map<std::string, std::string>();

        while (stream.nextLine())
        {
            auto line = stream.getLine();
            if (line.empty())
            {
                break;
            }

            auto keyAndValue = brayns::StringSplitter::split(line, "=");
            if (keyAndValue.size() != 2)
            {
                throw std::runtime_error("Could not parse line " + std::to_string(stream.getLineNumber()));
            }

            auto key = brayns::StringTrimmer::trim(keyAndValue[0]);
            auto value = brayns::StringTrimmer::trim(keyAndValue[1]);
            result.emplace(key, value);
        }

        MhdRequiredKeys::check(result);

        return result;
    }
};

class MhdDataTypeConverter
{
public:
    static inline const std::unordered_map<std::string, brayns::VolumeDataType> converter = {
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
    static std::string resolve(const std::string &filename, const std::string &mhdFilePath)
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
    static brayns::RegularVolume build(std::string_view data, const brayns::RawVolumeLoaderParameters &params)
    {
        _checkDataSize(data.size(), params);

        auto volume = brayns::RegularVolume();
        volume.dataType = params.data_type;
        volume.size = params.dimensions;
        volume.spacing = params.spacing;
        volume.voxels = std::vector<uint8_t>(data.begin(), data.end());
        return volume;
    }

private:
    static void _checkDataSize(size_t dataSize, const brayns::RawVolumeLoaderParameters &params)
    {
        auto linealSize = brayns::math::reduce_mul(params.dimensions);
        if (linealSize == 0)
        {
            throw std::invalid_argument("Volume dimensions are empty");
        }

        auto expectedSize = linealSize * _getTypeByteSize(params.data_type);
        if (expectedSize != dataSize)
        {
            throw std::invalid_argument("Data size and expected size mismatch");
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
    explicit ModelBuilder(brayns::Model &model):
        _components(model.getComponents()),
        _systems(model.getSystems())
    {
    }

    void addVolume(brayns::RegularVolume volume)
    {
        _components.add<brayns::Volumes>(std::move(volume));
    }

    void addSystems()
    {
        _systems.setBoundsSystem<brayns::GenericBoundsSystem<brayns::Volumes>>();
        _systems.setDataSystem<brayns::VolumeDataSystem>();
    }

private:
    brayns::Components &_components;
    brayns::Systems &_systems;
};

} // namespace

namespace brayns
{
std::string RawVolumeLoader::getName() const
{
    return "raw-volume";
}

std::vector<std::string> RawVolumeLoader::getExtensions() const
{
    return {"raw"};
}

bool RawVolumeLoader::canLoadBinary() const
{
    return true;
}

std::vector<std::shared_ptr<Model>> RawVolumeLoader::loadBinary(const BinaryRequest &request)
{
    auto &progress = request.progress;

    progress("Parsing voxels", 0.f);
    auto volume = VoxelBuilder::build(request.data, request.params);

    progress("Building model", 0.5f);
    auto model = std::make_shared<Model>("Volume");
    auto builder = ModelBuilder(*model);
    builder.addVolume(std::move(volume));
    builder.addSystems();

    progress("Done", 1.f);
    auto result = std::vector<std::shared_ptr<Model>>();
    result.push_back(std::move(model));
    return result;
}

std::string MHDVolumeLoader::getName() const
{
    return "mhd-volume";
}

std::vector<std::string> MHDVolumeLoader::getExtensions() const
{
    return {"mhd"};
}

std::vector<std::shared_ptr<Model>> MHDVolumeLoader::loadFile(const FileRequest &request)
{
    auto path = std::string(request.path);

    auto mhd = MhdParser::parse(path);

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
    auto volumeFilePath = MhdRawFilePathResolver::resolve(mhd["ElementDataFile"], path);

    auto rawRequest = RawVolumeLoader::FileRequest();

    rawRequest.path = volumeFilePath;
    rawRequest.progress = request.progress;

    auto &params = rawRequest.params;
    params.dimensions = dimensions;
    params.spacing = spacing;
    params.data_type = dataType;

    return RawVolumeLoader().loadFile(rawRequest);
}
} // namespace brayns
