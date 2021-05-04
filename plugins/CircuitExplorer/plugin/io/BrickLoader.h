/* Copyright (c) 2018-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#include <common/types.h>
#include <plugin/api/CircuitExplorerParams.h>

#include <brayns/common/loader/Loader.h>
#include <brayns/common/types.h>

#include <set>
#include <vector>

/**
 * Load circuit from BlueConfig or CircuitConfig file, including simulation.
 */
class BrickLoader : public brayns::Loader
{
public:
    BrickLoader(brayns::Scene& scene, brayns::PropertyMap&& loaderParams = {});

    std::string getName() const final;

    std::vector<std::string> getSupportedExtensions() const final;

    bool isSupported(const std::string& filename,
                     const std::string& extension) const final;

    static brayns::PropertyMap getCLIProperties();

    brayns::PropertyMap getProperties() const final;

    std::vector<brayns::ModelDescriptorPtr> importFromBlob(
        brayns::Blob&& blob, const brayns::LoaderProgress& callback,
        const brayns::PropertyMap& properties) const final;

    std::vector<brayns::ModelDescriptorPtr> importFromFile(
        const std::string& filename, const brayns::LoaderProgress& callback,
        const brayns::PropertyMap& properties) const final;

    void exportToFile(const brayns::ModelDescriptorPtr modelDescriptor,
                      const std::string& filename);

private:
    std::string _readString(std::ifstream& f) const;
    brayns::PropertyMap _defaults;
};
