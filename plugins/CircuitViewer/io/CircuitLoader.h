/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Juan Hernando <juan.hernando@epfl.ch>
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
#include <brayns/common/types.h>

#include <vector>

namespace servus
{
class URI;
}

namespace brayns
{
/**
 * Load circuit from BlueConfig or CircuitConfig file, including simulation.
 */
class CircuitLoader : public Loader
{
public:
    CircuitLoader(Scene& scene, PropertyMap defaultParams);
    ~CircuitLoader();

    std::vector<std::string> getSupportedExtensions() const final;
    std::string getName() const final;
    static PropertyMap getCLIProperties();
    PropertyMap getProperties() const final;

    bool isSupported(const std::string& filename,
                     const std::string& extension) const final;

    ModelDescriptorPtr importFromBlob(
        Blob&& blob, const LoaderProgress& callback,
        const PropertyMap& properties) const final;

    ModelDescriptorPtr importFromFile(
        const std::string& filename, const LoaderProgress& callback,
        const PropertyMap& properties) const final;

private:
    PropertyMap _defaults; // command line defaults
};
}
