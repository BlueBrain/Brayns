/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include <brayns/common/utils/stringUtils.h>
#include <brayns/common/utils/utils.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>
#include <brayns/engine/SharedDataVolume.h>

#include <boost/filesystem.hpp>

#include <fstream>
#include <map>
#include <sstream>
#include <string>

namespace
{
using Property = brayns::Property;
const Property PROP_DIMENSIONS = {"dimensions",
                                  std::array<int32_t, 3>({{0, 0, 0}}),
                                  {"Dimensions"}};
const Property PROP_SPACING = {"spacing",
                               std::array<double, 3>({{1, 1, 1}}),
                               {"Spacing"}};
const Property PROP_TYPE = {"type",
                            brayns::enumToString(brayns::DataType::UINT8),
                            brayns::enumNames<brayns::DataType>(),
                            {"Type"}};
}

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
std::array<T, 3> parseArray3(const std::string& str,
                             std::function<T(std::string)> conv)
{
    const auto v = brayns::string_utils::split(str, ' ');
    if (v.size() != 3)
        throw std::runtime_error("Not exactly 3 values for mhd array");
    return {conv(v[0]), conv(v[1]), conv(v[2])};
}

std::map<std::string, std::string> parseMHD(const std::string& filename)
{
    if (!boost::filesystem::exists(filename))
        throw std::runtime_error("File not found");

    // Sample MHD File:
    //
    // ObjectType = Image
    // DimSize = 1 2 3
    // ElementSpacing = 0.1 0.2 0.3
    // ElementType = MET_USHORT
    // ElementDataFile = BS39.raw

    std::map<std::string, std::string> result;
    std::ifstream infile(filename);
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

DataType dataTypeFromMET(const std::string& type)
{
    if (type == "MET_FLOAT")
        return DataType::FLOAT;
    else if (type == "MET_DOUBLE")
        return DataType::DOUBLE;
    else if (type == "MET_UCHAR")
        return DataType::UINT8;
    else if (type == "MET_USHORT")
        return DataType::UINT16;
    else if (type == "MET_UINT")
        return DataType::UINT32;
    else if (type == "MET_CHAR")
        return DataType::INT8;
    else if (type == "MET_SHORT")
        return DataType::INT16;
    else if (type == "MET_INT")
        return DataType::INT32;
    else
        throw std::runtime_error("Unknown data type " + type);
}

Vector2f dataRangeFromType(DataType type)
{
    switch (type)
    {
    case DataType::UINT8:
        return {std::numeric_limits<uint8_t>::min(),
                std::numeric_limits<uint8_t>::max()};
    case DataType::UINT16:
        return {std::numeric_limits<uint16_t>::min(),
                std::numeric_limits<uint16_t>::max()};
    case DataType::UINT32:
        return {std::numeric_limits<uint32_t>::min() / 100,
                std::numeric_limits<uint32_t>::max() / 100};
    case DataType::INT8:
        return {std::numeric_limits<int8_t>::min(),
                std::numeric_limits<int8_t>::max()};
    case DataType::INT16:
        return {std::numeric_limits<int16_t>::min(),
                std::numeric_limits<int16_t>::max()};
    case DataType::INT32:
        return {std::numeric_limits<int32_t>::min() / 100,
                std::numeric_limits<int32_t>::max() / 100};
    case DataType::FLOAT:
    case DataType::DOUBLE:
    default:
        return {0, 1};
    }
}
}

RawVolumeLoader::RawVolumeLoader(Scene& scene)
    : Loader(scene)
{
}

bool RawVolumeLoader::isSupported(const std::string& filename BRAYNS_UNUSED,
                                  const std::string& extension) const
{
    return extension == "raw";
}

ModelDescriptorPtr RawVolumeLoader::importFromBlob(
    Blob&& blob, const LoaderProgress& callback,
    const PropertyMap& properties) const
{
    return _loadVolume(blob.name, callback, properties, [&blob](auto volume) {
        volume->mapData(std::move(blob.data));
    });
}

ModelDescriptorPtr RawVolumeLoader::importFromFile(
    const std::string& filename, const LoaderProgress& callback,
    const PropertyMap& properties) const
{
    return _loadVolume(filename, callback, properties,
                       [filename](auto volume) { volume->mapData(filename); });
}

ModelDescriptorPtr RawVolumeLoader::_loadVolume(
    const std::string& filename, const LoaderProgress& callback,
    const PropertyMap& propertiesTmp,
    const std::function<void(SharedDataVolumePtr)>& mapData) const
{
    // Fill property map since the actual property types are known now.
    PropertyMap properties = getProperties();
    properties.merge(propertiesTmp);

    callback.updateProgress("Parsing volume file ...", 0.f);

    const auto dimensions = toGlmVec(
        properties.getProperty<std::array<int32_t, 3>>(PROP_DIMENSIONS.name));
    const auto spacing = toGlmVec(
        properties.getProperty<std::array<double, 3>>(PROP_SPACING.name));
    const auto type = stringToEnum<DataType>(
        properties.getProperty<std::string>(PROP_TYPE.name));

    if (glm::compMul(dimensions) == 0)
        throw std::runtime_error("Volume dimensions are empty");

    const auto dataRange = dataRangeFromType(type);
    auto model = _scene.createModel();
    auto volume = model->createSharedDataVolume(dimensions, spacing, type);
    volume->setDataRange(dataRange);

    callback.updateProgress("Loading voxels ...", 0.5f);
    mapData(volume);

    callback.updateProgress("Adding model ...", 1.f);
    model->addVolume(volume);

    Transformation transformation;
    transformation.setRotationCenter(model->getBounds().getCenter());
    auto modelDescriptor = std::make_shared<ModelDescriptor>(
        std::move(model), filename,
        ModelMetadata{{"dimensions", to_string(dimensions)},
                      {"element-spacing", to_string(spacing)}});
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

PropertyMap RawVolumeLoader::getProperties() const
{
    PropertyMap pm;
    pm.setProperty(PROP_DIMENSIONS);
    pm.setProperty(PROP_SPACING);
    pm.setProperty(PROP_TYPE);
    return pm;
}
////////////////////////////////////////////////////////////////////////////

MHDVolumeLoader::MHDVolumeLoader(Scene& scene)
    : Loader(scene)
{
}

bool MHDVolumeLoader::isSupported(const std::string& filename BRAYNS_UNUSED,
                                  const std::string& extension) const
{
    return extension == "mhd";
}

ModelDescriptorPtr MHDVolumeLoader::importFromBlob(
    Blob&& blob BRAYNS_UNUSED, const LoaderProgress&,
    const PropertyMap& properties BRAYNS_UNUSED) const
{
    throw std::runtime_error("Volume loading from blob is not supported");
}

ModelDescriptorPtr MHDVolumeLoader::importFromFile(
    const std::string& filename, const LoaderProgress& callback,
    const PropertyMap&) const
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
        parseArray3<int32_t>(mhd.at("DimSize"),
                             [](const auto& s) { return stoi(s); });
    const auto spacing =
        parseArray3<double>(mhd.at("ElementSpacing"),
                            [](const auto& s) { return stod(s); });
    const auto type = dataTypeFromMET(mhd.at("ElementType"));

    boost::filesystem::path path = mhd.at("ElementDataFile");
    if (!path.is_absolute())
    {
        boost::filesystem::path basePath(filename);
        path = boost::filesystem::canonical(path, basePath.parent_path());
    }
    volumeFile = path.string();

    PropertyMap properties;
    properties.setProperty(
        {PROP_DIMENSIONS.name, dimensions, PROP_DIMENSIONS.metaData});
    properties.setProperty({PROP_SPACING.name, spacing, PROP_SPACING.metaData});
    properties.setProperty({PROP_TYPE.name, brayns::enumToString(type),
                            PROP_TYPE.enums, PROP_TYPE.metaData});

    return RawVolumeLoader(_scene).importFromFile(volumeFile, callback,
                                                  properties);
}

std::string MHDVolumeLoader::getName() const
{
    return "mhd-volume";
}

std::vector<std::string> MHDVolumeLoader::getSupportedExtensions() const
{
    return {"mhd"};
}
}
