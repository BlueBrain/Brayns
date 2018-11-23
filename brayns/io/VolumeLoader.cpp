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

#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/utils/Utils.h>
#include <brayns/common/volume/SharedDataVolume.h>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;

namespace
{
using Property = brayns::PropertyMap::Property;
const Property PROP_DIMENSIONS = {"dimensions", "Dimensions",
                                  std::array<int32_t, 3>({{0, 0, 0}})};
const Property PROP_SPACING = {"spacing", "Spacing",
                               std::array<double, 3>({{0, 0, 0}})};
const Property PROP_TYPE = {"type", "Type",
                            brayns::enumToString(brayns::DataType::UINT8),
                            brayns::enumNames<brayns::DataType>()};
}

namespace brayns
{
namespace
{
template <size_t M, typename T>
std::string to_string(const vmml::vector<M, T>& vec)
{
    std::stringstream stream;
    stream << vec;
    return stream.str();
}

template <typename T>
vmml::vector<3, T> to_Vector3(const std::string& s)
{
    std::vector<T> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ' '))
        result.push_back(boost::lexical_cast<T>(item));
    if (result.size() != 3)
        throw std::runtime_error("Not exactly 3 values for mhd array");
    return vmml::vector<3, T>(result.data());
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
    Blob&& blob BRAYNS_UNUSED, const LoaderProgress&,
    const PropertyMap& properties BRAYNS_UNUSED,
    const size_t index BRAYNS_UNUSED,
    const size_t defaultMaterialId BRAYNS_UNUSED) const
{
    throw std::runtime_error("Volume loading from blob is not supported");
}

ModelDescriptorPtr RawVolumeLoader::importFromFile(
    const std::string& filename, const LoaderProgress& callback,
    const PropertyMap& propertiesTmp, const size_t index BRAYNS_UNUSED,
    const size_t defaultMaterialId BRAYNS_UNUSED) const
{
    // Fill property map since the actual property types are known now.
    PropertyMap properties = getProperties();
    properties.merge(propertiesTmp);

    callback.updateProgress("Parsing volume file ...", 0.f);

    const auto dimensions = arrayToVmmlVec(
        properties.getProperty(PROP_DIMENSIONS.name,
                               std::array<int32_t, 3>({{0, 0, 0}})));
    const auto spacing = arrayToVmmlVec(
        properties.getProperty(PROP_SPACING.name,
                               std::array<double, 3>({{0, 0, 0}})));
    const auto type = stringToEnum<DataType>(
        properties.getProperty<std::string>(PROP_TYPE.name));

    if (dimensions.product() == 0)
        throw std::runtime_error("Volume dimensions are empty");

    const auto dataRange = dataRangeFromType(type);
    auto model = _scene.createModel();
    auto volume = model->createSharedDataVolume(dimensions, spacing, type);
    volume->setDataRange(dataRange);

    callback.updateProgress("Loading voxels ...", 0.5f);
    volume->mapData(filename);

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
    const PropertyMap& properties BRAYNS_UNUSED,
    const size_t index BRAYNS_UNUSED,
    const size_t defaultMaterialId BRAYNS_UNUSED) const
{
    throw std::runtime_error("Volume loading from blob is not supported");
}

ModelDescriptorPtr MHDVolumeLoader::importFromFile(
    const std::string& filename, const LoaderProgress& callback,
    const PropertyMap&, const size_t index BRAYNS_UNUSED,
    const size_t defaultMaterialId BRAYNS_UNUSED) const
{
    std::string volumeFile = filename;
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(filename, pt);

    if (pt.get<std::string>("ObjectType") != "Image")
        throw std::runtime_error("Wrong object type for mhd file");

    const auto dimensions =
        vmmlVecToArray(to_Vector3<int32_t>(pt.get<std::string>("DimSize")));
    const auto spacing = vmmlVecToArray(
        to_Vector3<double>(pt.get<std::string>("ElementSpacing")));
    const auto type = dataTypeFromMET(pt.get<std::string>("ElementType"));

    boost::filesystem::path path = pt.get<std::string>("ElementDataFile");
    if (!path.is_absolute())
    {
        boost::filesystem::path basePath(filename);
        path = boost::filesystem::canonical(path, basePath.parent_path());
    }
    volumeFile = path.string();

    PropertyMap properties;
    properties.setProperty(
        {PROP_DIMENSIONS.name, PROP_DIMENSIONS.label, dimensions});
    properties.setProperty({PROP_SPACING.name, PROP_SPACING.label, spacing});
    properties.setProperty({PROP_TYPE.name, PROP_TYPE.label,
                            brayns::enumToString(type), PROP_TYPE.enums});

    return RawVolumeLoader(_scene).importFromFile(volumeFile, callback,
                                                  properties, index,
                                                  defaultMaterialId);
}

std::string MHDVolumeLoader::getName() const
{
    return "mhd-volume";
}

std::vector<std::string> MHDVolumeLoader::getSupportedExtensions() const
{
    return {"mhd"};
}

PropertyMap MHDVolumeLoader::getProperties() const
{
    return {};
}
}
