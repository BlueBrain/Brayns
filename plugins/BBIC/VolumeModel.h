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

#pragma once

#include <brayns/common/loader/Loader.h>
#include <brayns/common/types.h>

#include <memory>
#include <thread>

#include "BBICFile.h"

namespace bbic
{
class VolumeModel
{
public:
    VolumeModel(const std::string& fileName, brayns::ModelPtr model,
                const brayns::LoaderProgress& callback);
    VolumeModel(VolumeModel&&) = default;
    ~VolumeModel();

    void updateActiveVolume();
    std::function<void()> triggerRender;

    using Block = std::array<uint32_t, 3>;

    brayns::ModelDescriptorPtr getModel() const;

private:
    bool _createVolume(const size_t lod, brayns::Model& model);
    void _uploadOneBlock(brayns::BrickedVolumePtr volume, const uint32_t lod,
                         const Block& block);
    void _startUploadThread();
    void _stopUploadThread();

    std::unique_ptr<File> _file;
    brayns::ModelDescriptorPtr _modelDesc;
    std::vector<brayns::BrickedVolumePtr> _volumes;
    brayns::BrickedVolumePtr _activeVolume;
    uint32_t _lod{std::numeric_limits<uint32_t>::max()};

    std::vector<std::vector<Block>> _toUpload;

    std::thread _uploadThread;
    bool _keepUploading{true};
};
}
