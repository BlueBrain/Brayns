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

class MhdDataConverter
{
public:
    inline static const std::string_view metFloat = "MET_FLOAT";
    inline static const std::string_view metDouble = "MET_DOUBLE";
    inline static const std::string_view metUChar = "MET_UCHAR";
    inline static const std::string_view metUShort = "MET_USHORT";
    inline static const std::string_view metUInt = "MET_UINT";
    inline static const std::string_view metChar = "MET_CHAR";
    inline static const std::string_view metShort = "MET_SHORT";
    inline static const std::string_view metInt = "MET_INT";

public:
    explicit MhdDataConverter(const std::string &dataType, const std::string &filename)
    {
        _convertData(dataType, filename);
    }

    std::vector<uint8_t> data;
    brayns::VolumeDataType dataType;

private:
    void _convertData(const std::string &type, const std::string &filename)
    {
        if (type == metFloat)
        {
            _copyBytes(brayns::VolumeDataType::Float, filename);
            return;
        }
        if (type == metDouble)
        {
            _copyBytes(brayns::VolumeDataType::Double, filename);
            return;
        }
        if (type == metUChar)
        {
            _copyBytes(brayns::VolumeDataType::UnsignedChar, filename);
            return;
        }
        if (type == metUShort)
        {
            _copyBytes(brayns::VolumeDataType::UnsignedShort, filename);
            return;
        }
        if (type == metUInt)
        {
            _convertAndCopyBytes<unsigned int, float>(brayns::VolumeDataType::Float, filename);
            return;
        }
        if (type == metChar)
        {
            _convertAndCopyBytes<char, short>(brayns::VolumeDataType::Short, filename);
            return;
        }
        if (type == metShort)
        {
            _copyBytes(brayns::VolumeDataType::Short, filename);
            return;
        }
        if (type == metInt)
        {
            _convertAndCopyBytes<int, float>(brayns::VolumeDataType::Float, filename);
            return;
        }

        throw std::invalid_argument("Unkown/Unsupported MHD datatype: " + type);
    }

    void _copyBytes(brayns::VolumeDataType type, const std::string &filename)
    {
        dataType = type;
        auto input = brayns::FileReader::read(filename);
        auto src = reinterpret_cast<const uint8_t *>(input.data());
        auto size = input.size();
        data = std::vector<uint8_t>(src, src + size);
    }

    template<typename From, typename To>
    void _convertAndCopyBytes(brayns::VolumeDataType type, const std::string &filename)
    {
        dataType = type;
        auto input = brayns::FileReader::read(filename);
        auto src = reinterpret_cast<const From *>(input.data());
        auto size = input.size() / sizeof(From);

        data = std::vector<uint8_t>(input.size());

        for (size_t i = 0; i < size; ++i)
        {
            auto index = i * sizeof(To);
            auto converted = static_cast<To>(src[i]);
            std::memcpy(&data[index], &converted, sizeof(To));
        }
    }
};

class VoxelBuilder
{
public:
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
        auto linealSize = glm::compMul(params.dimensions);
        if (linealSize == 0)
        {
            throw std::invalid_argument("Volume dimensions are empty");
        }

        auto expectedSize = linealSize * _getTypeByteSize(params.type);
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

    auto &type = mhd["ElementType"];
    auto volumeFilename = std::filesystem::path(mhd["ElementDataFile"]);
    if (!volumeFilename.is_absolute())
    {
        auto basePath = std::filesystem::path(filename).parent_path();
        volumeFilename = std::filesystem::canonical(basePath / volumeFilename);
    }
    auto converter = MhdDataConverter(type, volumeFilename.string());

    auto params = RawVolumeLoaderParameters();
    params.dimensions = dimensions;
    params.spacing = spacing;
    params.type = converter.dataType;

    auto blob = Blob();
    blob.name = filename;
    blob.type = "raw";
    blob.data = std::move(converter.data);

    return RawVolumeLoader().importFromBlob(blob, callback, params);
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
