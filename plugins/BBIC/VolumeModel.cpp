/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
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

#include "VolumeModel.h"

#include <brayns/engineapi/BrickedVolume.h>
#include <brayns/engineapi/Model.h>

namespace
{
std::string to_string(const brayns::Vector3d& vec)
{
    std::stringstream ss;
    ss << vec;
    return ss.str();
}

brayns::PropertyMap createPropertyMap(const int32_t lods)
{
    brayns::PropertyMap props;
    props.setProperty(
        {"lod", lods, 0, lods, {"Level of detail", "Level of detail"}});
    return props;
}
}

namespace bbic
{
VolumeModel::VolumeModel(const std::string& fileName, brayns::ModelPtr model,
                         const brayns::LoaderProgress& callback)
    : _file(std::make_unique<File>(fileName))
{
    const auto levels = _file->getLevels();
    for (size_t i = 0; i < levels; ++i)
    {
        if (!_createVolume(i, *model))
            break;
        callback.updateProgress("Loading volume...", float(i) / levels);
    }
    callback.updateProgress("Loading volume...", 1.f);

    const auto bbox = _file->getBoundingBox();
    brayns::Transformation transformation;
    transformation.setRotationCenter(bbox.getCenter());

    _modelDesc = std::make_shared<brayns::ModelDescriptor>(
        std::move(model), fileName,
        brayns::ModelMetadata{{"Levels of detail",
                               std::to_string(_volumes.size())},
                              {"Volume size", to_string(bbox.getMax())}});
    _modelDesc->setTransformation(transformation);
    _modelDesc->setProperties(createPropertyMap(_volumes.size() - 1));
    _activeVolume = _volumes.back();
    _modelDesc->getModel().addVolume(_volumes.back());
}

VolumeModel::~VolumeModel()
{
    _stopUploadThread();
}

void VolumeModel::updateActiveVolume()
{
    const size_t newLod =
        _modelDesc->getProperties().getProperty<int32_t>("lod");
    if (newLod >= _volumes.size())
        return;

    if (_lod == newLod)
        return;

    _stopUploadThread();
    _lod = newLod;

    if (_activeVolume)
        _modelDesc->getModel().removeVolume(_activeVolume);

    _activeVolume = _volumes[_lod];
    _modelDesc->getModel().addVolume(_activeVolume);

    _startUploadThread();
}

void VolumeModel::_uploadOneBlock(brayns::BrickedVolumePtr volume,
                                  const uint32_t lod, const Block& block)
{
    const auto& blockSize = _file->getBlockSize();
    const brayns::Vector3ui region_lo(block[0] * blockSize,
                                      block[1] * blockSize,
                                      block[2] * blockSize);

    const auto& data = _file->getData(lod, block);
    const brayns::Vector3ui voxelBox(blockSize);
    volume->setBrick(reinterpret_cast<const void*>(data.data()), region_lo,
                     voxelBox);
}

brayns::ModelDescriptorPtr VolumeModel::getModel() const
{
    return _modelDesc;
}

bool VolumeModel::_createVolume(const size_t lod, brayns::Model& model)
{
    const auto& blockCount = _file->getBlockCount(lod);
    const auto blockSize = _file->getBlockSize();

    const brayns::Vector3ui dim(blockCount[0] * blockSize,
                                blockCount[1] * blockSize,
                                blockCount[2] * blockSize);
    const brayns::Vector3ui spacing(1 << lod, 1 << lod, 1 << lod);

    auto volume =
        model.createBrickedVolume(dim, spacing, brayns::DataType::UINT8);
    volume->setDataRange({0, 255});

    _toUpload.push_back({});
    for (uint32_t x = 0; x < blockCount[0]; ++x)
        for (uint32_t y = 0; y < blockCount[1]; ++y)
            for (uint32_t z = 0; z < blockCount[2]; ++z)
                _toUpload[lod].emplace_back(Block{{x, y, z}});

    // upload one block already to build the acceleration structure
    const auto block = _toUpload[lod].back();
    _toUpload[lod].pop_back();
    _uploadOneBlock(volume, lod, block);

    _volumes.push_back(volume);
    volume->commit();

    return glm::compMin(dim) != glm::compMin(glm::vec<3, size_t>(blockSize));
}

void VolumeModel::_startUploadThread()
{
    auto& toUpload_ = _toUpload[_lod];
    if (toUpload_.empty())
        return;

    _uploadThread =
        std::thread([ this, volume = _activeVolume, lod = _lod, &toUpload_ ] {
            while (_keepUploading && !toUpload_.empty())
            {
                const auto block = toUpload_.back();
                toUpload_.pop_back();
                _uploadOneBlock(volume, lod, block);
                triggerRender();
            }
        });
}

void VolumeModel::_stopUploadThread()
{
    if (_uploadThread.joinable())
    {
        _keepUploading = false;
        _uploadThread.join();
    }
    _keepUploading = true;
}
}
