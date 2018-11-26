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

#pragma once

#include <brayns/common/loader/Loader.h>
#include <brayns/parameters/VolumeParameters.h>

namespace brayns
{
/** A volume loader for mhd volumes.
 */
class MHDVolumeLoader : public Loader
{
public:
    MHDVolumeLoader(Scene& scene);

    std::vector<std::string> getSupportedExtensions() const final;
    std::string getName() const final;

    bool isSupported(const std::string& filename,
                     const std::string& extension) const final;
    ModelDescriptorPtr importFromBlob(
        Blob&& blob, const LoaderProgress& callback,
        const PropertyMap& properties, const size_t index = 0,
        const size_t defaultMaterialId = NO_MATERIAL) const final;

    ModelDescriptorPtr importFromFile(
        const std::string& filename, const LoaderProgress& callback,
        const PropertyMap& properties, const size_t index = 0,
        const size_t defaultMaterialId = NO_MATERIAL) const final;
};

/** A volume loader for raw volumes with params for dimensions.
 */
class RawVolumeLoader : public Loader
{
public:
    RawVolumeLoader(Scene& scene);

    std::vector<std::string> getSupportedExtensions() const final;
    std::string getName() const final;
    PropertyMap getProperties() const final;

    bool isSupported(const std::string& filename,
                     const std::string& extension) const final;
    ModelDescriptorPtr importFromBlob(
        Blob&& blob, const LoaderProgress& callback,
        const PropertyMap& properties, const size_t index = 0,
        const size_t defaultMaterialId = NO_MATERIAL) const final;

    ModelDescriptorPtr importFromFile(
        const std::string& filename, const LoaderProgress& callback,
        const PropertyMap& properties, const size_t index = 0,
        const size_t defaultMaterialId = NO_MATERIAL) const final;
};
}
