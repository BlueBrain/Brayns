/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>
#include <brayns/engine/SharedDataVolume.h>
#include <brayns/utils/Filesystem.h>
#include <brayns/utils/StringUtils.h>

#include <fstream>
#include <map>
#include <sstream>
#include <string>

namespace brayns
{
namespace
{
template <size_t M, typename T>
std::string to_string(const glm::vec<M, T>& vec)
{
    std::stringstream stream;
    stream << vec;
    return stream.str();
}

template <typename T>
glm::vec<3, T> parseVec3(const std::string& str,
                         std::function<T(std::string)> conv)
{
    const auto v = brayns::string_utils::split(str, ' ');
    if (v.size() != 3)
        throw std::runtime_error("Not exactly 3 values for mhd array");
    return {conv(v[0]), conv(v[1]), conv(v[2])};
}

std::map<std::string, std::string> parseMHD(const std::string& filename)
{
    std::ifstream infile(filename);
    if (!infile.good())
        throw std::runtime_error("Could not open file " + filename);

    // Sample MHD File:
    //
    // ObjectType = Image
    // DimSize = 1 2 3
    // ElementSpacing = 0.1 0.2 0.3
    // ElementType = MET_USHORT
    // ElementDataFile = BS39.raw

    std::map<std::string, std::string> result;
    std::string line;
    size_t ctr = 1;
    while (std::getline(infile, line))
    {
        const auto v = string_utils::split(line, '=');
        if (v.size() != 2)
            throw std::runtime_error("Could not parse line " +
                                     std::to_string(ctr));
        auto key = v[0];
        auto value = v[1];
        string_utils::trim(key);
        string_utils::trim(value);

        result[key] = value;
        ++ctr;
    }

    return result;
}

VolumeDataType dataTypeFromMET(const std::string& type)
{
    if (type == "MET_FLOAT")
        return VolumeDataType::FLOAT;
    else if (type == "MET_DOUBLE")
        return VolumeDataType::DOUBLE;
    else if (type == "MET_UCHAR")
        return VolumeDataType::UINT8;
    else if (type == "MET_USHORT")
        return VolumeDataType::UINT16;
    else if (type == "MET_UINT")
        return VolumeDataType::UINT32;
    else if (type == "MET_CHAR")
        return VolumeDataType::INT8;
    else if (type == "MET_SHORT")
        return VolumeDataType::INT16;
    else if (type == "MET_INT")
        return VolumeDataType::INT32;
    else
        throw std::runtime_error("Unknown data type " + type);
}

Vector2f dataRangeFromType(VolumeDataType type)
{
    switch (type)
    {
    case VolumeDataType::UINT8:
        return {std::numeric_limits<uint8_t>::min(),
                std::numeric_limits<uint8_t>::max()};
    case VolumeDataType::UINT16:
        return {std::numeric_limits<uint16_t>::min(),
                std::numeric_limits<uint16_t>::max()};
    case VolumeDataType::UINT32:
        return {std::numeric_limits<uint32_t>::min() / 100,
                std::numeric_limits<uint32_t>::max() / 100};
    case VolumeDataType::INT8:
        return {std::numeric_limits<int8_t>::min(),
                std::numeric_limits<int8_t>::max()};
    case VolumeDataType::INT16:
        return {std::numeric_limits<int16_t>::min(),
                std::numeric_limits<int16_t>::max()};
    case VolumeDataType::INT32:
        return {std::numeric_limits<int32_t>::min() / 100,
                std::numeric_limits<int32_t>::max() / 100};
    case VolumeDataType::FLOAT:
    case VolumeDataType::DOUBLE:
    default:
        return {0, 1};
    }
}
} // namespace

bool RawVolumeLoader::isSupported(const std::string&,
                                  const std::string& extension) const
{
    return extension == "raw";
}

std::vector<ModelDescriptorPtr> RawVolumeLoader::importFromBlob(
    Blob&& blob, const LoaderProgress& callback,
    const RawVolumeLoaderParameters& properties, Scene& scene) const
{
    return {_loadVolume(blob.name, callback, properties,
                        [&blob](auto volume) {
                            volume->mapData(std::move(blob.data));
                        },
                        scene)};
}

std::vector<ModelDescriptorPtr> RawVolumeLoader::importFromFile(
    const std::string& filename, const LoaderProgress& callback,
    const RawVolumeLoaderParameters& properties, Scene& scene) const
{
    return {_loadVolume(filename, callback, properties,
                        [filename](auto volume) { volume->mapData(filename); },
                        scene)};
}

ModelDescriptorPtr RawVolumeLoader::_loadVolume(
    const std::string& filename, const LoaderProgress& callback,
    const RawVolumeLoaderParameters& params,
    const std::function<void(SharedDataVolumePtr)>& mapData, Scene& scene) const
{
    callback.updateProgress("Parsing volume file ...", 0.f);

    if (glm::compMul(params.dimensions) == 0)
        throw std::runtime_error("Volume dimensions are empty");

    const auto dataRange = dataRangeFromType(params.type);
    auto model = scene.createModel();
    auto volume = model->createSharedDataVolume(params.dimensions,
                                                params.spacing, params.type);
    volume->setDataRange(dataRange);

    callback.updateProgress("Loading voxels ...", 0.5f);
    mapData(volume);

    callback.updateProgress("Adding model ...", 1.f);
    model->addVolume(volume);

    Transformation transformation;
    transformation.setRotationCenter(model->getBounds().getCenter());
    auto modelDescriptor = std::make_shared<ModelDescriptor>(
        std::move(model), filename,
        ModelMetadata{{"dimensions", to_string(params.dimensions)},
                      {"element-spacing", to_string(params.spacing)}});
    modelDescriptor->setTransformation(transformation);
    return modelDescriptor;
}

std::string RawVolumeLoader::getName() const
{
    return "raw-volume";
}

std::vector<std::string> RawVolumeLoader::getSupportedExtensions() const
{
    return {"raw"};
}

////////////////////////////////////////////////////////////////////////////

bool MHDVolumeLoader::isSupported(const std::string&,
                                  const std::string& extension) const
{
    return extension == "mhd";
}

std::vector<ModelDescriptorPtr> MHDVolumeLoader::importFromBlob(
    Blob&&, const LoaderProgress&, Scene&) const
{
    throw std::runtime_error("Volume loading from blob is not supported");
}

std::vector<ModelDescriptorPtr> MHDVolumeLoader::importFromFile(
    const std::string& filename, const LoaderProgress& callback,
    Scene& scene) const
{
    std::string volumeFile = filename;
    const auto mhd = parseMHD(filename);

    // Check all keys present
    for (const auto key : {"ObjectType", "DimSize", "ElementSpacing",
                           "ElementType", "ElementDataFile"})
        if (mhd.find(key) == mhd.end())
            throw std::runtime_error("Missing key " + std::string(key));

    if (mhd.at("ObjectType") != "Image")
        throw std::runtime_error("Wrong object type for mhd file");

    const auto dimensions =
        parseVec3<int32_t>(mhd.at("DimSize"),
                           [](const auto& s) { return stoi(s); });
    const auto spacing =
        parseVec3<double>(mhd.at("ElementSpacing"),
                          [](const auto& s) { return stod(s); });
    const auto type = dataTypeFromMET(mhd.at("ElementType"));

    fs::path path = mhd.at("ElementDataFile");
    if (!path.is_absolute())
    {
        auto basePath = fs::path(filename).parent_path();
        path = fs::canonical(basePath / path);
    }
    volumeFile = path.string();

    RawVolumeLoaderParameters params;
    params.dimensions = dimensions;
    params.spacing = spacing;
    params.type = type;

    return RawVolumeLoader().importFromFile(volumeFile, callback, params,
                                            scene);
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
