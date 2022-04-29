/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include <io/BBPLoaderParameters.h>

#include <brion/blueConfig.h>

/**
 * @brief Imports a circuit from a BBP's BlueConfig/CircuitConfig file
 */
class BBPLoader final : public brayns::Loader<BBPLoaderParameters>
{
public:
    std::vector<std::string> getSupportedExtensions() const final;

    bool isSupported(const std::string &filename, const std::string &extension) const final;

    std::string getName() const final;

    std::vector<std::unique_ptr<brayns::Model>> importFromBlob(
        brayns::Blob &&blob,
        const brayns::LoaderProgress &callback,
        const BBPLoaderParameters &params) const final;

    std::vector<std::unique_ptr<brayns::Model>> importFromFile(
        const std::string &path,
        const brayns::LoaderProgress &callback,
        const BBPLoaderParameters &params) const final;

    /**
     * @brief importFromBlueConfig imports a neuronal circuit from a BlueConfig
     * file. NOTE: Temporary utility function so SonataNGVLoader can call this
     * loader for each population.
     *        TODO: Remove once NGV Project has switched to SONATA format...
     */
    std::vector<std::unique_ptr<brayns::Model>> importFromBlueConfig(
        const brayns::LoaderProgress &callback,
        const BBPLoaderParameters &params,
        const brion::BlueConfig &config) const;
};
