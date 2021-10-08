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

#include <set>

namespace bbic
{
class File;
class Plugin;
struct VolumeModel;

class Loader : public brayns::Loader
{
public:
    Loader(brayns::Scene& scene, Plugin* plugin);

    std::vector<std::string> getSupportedExtensions() const final;
    std::string getName() const final;

    bool isSupported(const std::string& filename,
                     const std::string& extension) const final;

    std::vector<brayns::ModelDescriptorPtr> importFromFile(
        const std::string& fileName, const brayns::LoaderProgress& callback,
        const brayns::PropertyMap& properties) const final;

    std::vector<brayns::ModelDescriptorPtr> importFromBlob(
        brayns::Blob&& /*blob*/, const brayns::LoaderProgress& /*callback*/,
        const brayns::PropertyMap& /*properties*/) const final
    {
        throw std::runtime_error(
            "Loading BBIC volume from blob is not supported");
    }

private:
    bool _createVolume(const size_t lod, const File& file,
                       VolumeModel& volumeModel) const;

    Plugin* _plugin;
};
} // namespace bbic
