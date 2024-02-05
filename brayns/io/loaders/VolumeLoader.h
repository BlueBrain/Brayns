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

#pragma once

#include <brayns/io/Loader.h>
#include <brayns/io/loaders/RawVolumeLoaderParameters.h>

namespace brayns
{
/** A volume loader for mhd volumes.
 */
class MHDVolumeLoader : public NoInputLoader
{
public:
    std::vector<std::string> getSupportedExtensions() const final;

    std::string getName() const final;

    bool isSupported(const std::string& filename,
                     const std::string& extension) const final;
    std::vector<ModelDescriptorPtr> importFromBlob(
        Blob&& blob, const LoaderProgress& callback, Scene& scene) const final;

    std::vector<ModelDescriptorPtr> importFromFile(
        const std::string& filename, const LoaderProgress& callback,
        Scene& scene) const final;
};

/** A volume loader for raw volumes with params for dimensions.
 */
class RawVolumeLoader : public Loader<RawVolumeLoaderParameters>
{
public:
    std::vector<std::string> getSupportedExtensions() const final;

    std::string getName() const final;

    bool isSupported(const std::string& filename,
                     const std::string& extension) const final;
    std::vector<ModelDescriptorPtr> importFromBlob(
        Blob&& blob, const LoaderProgress& callback,
        const RawVolumeLoaderParameters& properties, Scene& scene) const final;

    std::vector<ModelDescriptorPtr> importFromFile(
        const std::string& filename, const LoaderProgress& callback,
        const RawVolumeLoaderParameters& properties, Scene& scene) const final;

private:
    ModelDescriptorPtr _loadVolume(
        const std::string& filename, const LoaderProgress& callback,
        const RawVolumeLoaderParameters& properties,
        const std::function<void(SharedDataVolumePtr)>& mapData,
        Scene& scene) const;
};
} // namespace brayns
