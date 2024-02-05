/* Copyright 2018-2024 Blue Brain Project/EPFL
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

#include "DTILoaderParameters.h"
#include "DTITypes.h"

#include <brayns/io/Loader.h>

#include <boost/property_tree/ini_parser.hpp>

/**
 * Load circuit from BlueConfig or CircuitConfig file, including simulation.
 */
namespace dti
{
class DTILoader : public brayns::Loader<DTILoaderParameters>
{
public:
    std::string getName() const final;

    std::vector<std::string> getSupportedExtensions() const final;

    std::vector<brayns::ModelDescriptorPtr> importFromBlob(
        brayns::Blob&& blob, const brayns::LoaderProgress& callback,
        const DTILoaderParameters& properties,
        brayns::Scene& scene) const final;

    std::vector<brayns::ModelDescriptorPtr> importFromFile(
        const std::string& filename, const brayns::LoaderProgress& callback,
        const DTILoaderParameters& properties,
        brayns::Scene& scene) const final;

    static Colors getColorsFromPoints(const Points& points, const float opacity,
                                      const ColorScheme colorScheme);

private:
    DTIConfiguration _readConfiguration(
        const boost::property_tree::ptree& pt) const;

    brayns::PropertyMap _defaults;
};
} // namespace dti
