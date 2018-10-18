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
#include <brayns/common/volume/SharedDataVolume.h>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;

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

VolumeLoader::VolumeLoader(Scene& scene, VolumeParameters& volumeParameters)
    : Loader(scene)
    , _volumeParameters(volumeParameters)
{
}

bool VolumeLoader::isSupported(const std::string& filename BRAYNS_UNUSED,
                               const std::string& extension) const
{
    const std::set<std::string> types = {"raw", "mhd"};
    return types.find(extension) != types.end();
}

ModelDescriptorPtr VolumeLoader::importFromBlob(
    Blob&& blob BRAYNS_UNUSED, const LoaderProgress&,
    const size_t index BRAYNS_UNUSED,
    const size_t defaultMaterialId BRAYNS_UNUSED) const
{
    throw std::runtime_error("Volume loading from blob is not supported");
}

ModelDescriptorPtr VolumeLoader::importFromFile(
    const std::string& filename, const LoaderProgress& callback,
    const size_t index BRAYNS_UNUSED,
    const size_t defaultMaterialId BRAYNS_UNUSED) const
{
    callback.updateProgress("Parsing volume file ...", 0.f);

    Vector3ui dimensions;
    Vector3f spacing;
    DataType type;
    std::string volumeFile = filename;
    const bool mhd = boost::filesystem::extension(filename) == ".mhd";
    if (mhd)
    {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(filename, pt);

        if (pt.get<std::string>("ObjectType") != "Image")
            throw std::runtime_error("Wrong object type for mhd file");

        dimensions = to_Vector3<unsigned>(pt.get<std::string>("DimSize"));
        spacing = to_Vector3<float>(pt.get<std::string>("ElementSpacing"));
        type = dataTypeFromMET(pt.get<std::string>("ElementType"));
        boost::filesystem::path path = pt.get<std::string>("ElementDataFile");
        if (!path.is_absolute())
        {
            boost::filesystem::path basePath(filename);
            path = boost::filesystem::canonical(path, basePath.parent_path());
        }
        volumeFile = path.string();

        _volumeParameters.setDimensions(dimensions);
        _volumeParameters.setElementSpacing(spacing);
    }
    else
    {
        dimensions = _volumeParameters.getDimensions();
        spacing = _volumeParameters.getElementSpacing();
        type = DataType::UINT8;
    }

    if (dimensions.product() == 0)
        throw std::runtime_error("Volume dimensions are empty");

    const auto dataRange = dataRangeFromType(type);
    auto model = _scene.createModel();
    auto volume = model->createSharedDataVolume(dimensions, spacing, type);
    volume->setDataRange(dataRange);

    callback.updateProgress("Loading voxels ...", 0.5f);
    volume->mapData(volumeFile);

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
}
