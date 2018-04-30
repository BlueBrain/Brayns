/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jafet Villafranca Diaz <jafet.villafrancadiaz@epfl.ch>
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <H5Cpp.h>
#include <brayns/common/log.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/io/MeshLoader.h>

#include <brain/brain.h>
#include <brion/brion.h>

#include "ConnectivityLoader.h"
#include <boost/progress.hpp>

namespace
{
const float CONNECTION_RADIUS_MULTIPLIER = 0.1f;
const float INCOMING_CONNECTIONS_RADIUS_MULTIPLIER = 0.01f;
const std::string H5_MATRIX_PREFIX = "/matrix_";
}

namespace brayns
{
ConnectivityLoader::ConnectivityLoader(
    const GeometryParameters& geometryParameters)
    : _geometryParameters(geometryParameters)
{
}

bool ConnectivityLoader::_importMatrix()
{
    try
    {
        const std::string& matrixFile =
            _geometryParameters.getConnectivityFile();
        BRAYNS_INFO << "Loading connectivity from " << matrixFile << std::endl;

        H5::H5File matrices(matrixFile.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
        H5::DataSet dataset;
        const std::string matrix =
            H5_MATRIX_PREFIX +
            std::to_string(_geometryParameters.getConnectivityMatrixId());
        try
        {
            dataset = matrices.openDataSet(matrix);
        }
        catch (const H5::FileIException& e)
        {
            BRAYNS_ERROR << e.getDetailMsg() << std::endl;
            return false;
        }

        hsize_t dims[2];
        H5::DataSpace dspace = dataset.getSpace();
        dspace.getSimpleExtentDims(dims);

        uint8_ts values;
        values.resize(dims[0] * dims[1]);
        dataset.read(values.data(), H5::PredType::NATIVE_INT8);

        std::stringstream message;
        message << "Reading " << dims[0] << "x" << dims[1] << " matrix...";
        for (size_t x = 0; x < dims[0]; ++x)
        {
            for (size_t y = 0; y < dims[1]; ++y)
            {
                uint64_t index = x + y * dims[0];
                if (values[index] != 0)
                {
                    _emitors[x].push_back(y);
                    if (_receptors.find(y) == _receptors.end())
                        _receptors[y] = 1;
                    else
                        _receptors[y]++;
                }
            }
            updateProgress(message.str(), x + 1, dims[0]);
        }
        dataset.close();
    }
    catch (const H5::Exception& e)
    {
        BRAYNS_ERROR << e.getCDetailMsg() << std::endl;
        return false;
    }

    return true;
}

bool ConnectivityLoader::_importMesh(const uint64_t gid,
                                     const Matrix4f& transformation,
                                     const size_t materialId, Scene& scene,
                                     MeshLoader& meshLoader)
{
    const auto meshedMorphologiesFolder =
        _geometryParameters.getCircuitMeshFolder();
    if (meshedMorphologiesFolder.empty())
        return true;

    // Load mesh from file
    if (!meshLoader.importMeshFromFile(meshLoader.getMeshFilenameFromGID(gid),
                                       scene, transformation, materialId))
        return false;

    return true;
}

bool ConnectivityLoader::importFromFile(Scene& scene, MeshLoader& meshLoader)
{
    try
    {
        // Load matrix
        if (!_importMatrix())
            return false;

        // Load circuit information
        const auto& circuitConfig =
            _geometryParameters.getCircuitConfiguration();
        BRAYNS_INFO << "Loading circuit from " << circuitConfig << std::endl;
        const auto& targets = _geometryParameters.getCircuitTargetsAsStrings();
        if (targets.size() == 0)
        {
            BRAYNS_ERROR << "No target specified. Matrix cannot be loaded"
                         << std::endl;
            return false;
        }
        const auto& target = targets[0];
        const brion::BlueConfig bc(circuitConfig);
        const brain::Circuit circuit(bc);
        const auto& gids =
            (target.empty() ? circuit.getGIDs() : circuit.getGIDs(target));
        if (_geometryParameters.getCircuitDensity() != 100)
            BRAYNS_WARN << "Circuit density will be ignored since it does not "
                           "apply to connectivity matrix visualization"
                        << std::endl;

        if (gids.empty())
        {
            BRAYNS_ERROR << "Circuit does not contain any cells" << std::endl;
            return false;
        }

        // Build scene
        const auto& scale = _geometryParameters.getConnectivityScale();
        const auto& dimensionRange =
            _geometryParameters.getConnectivityDimensionRange();
        const auto& transforms = circuit.getTransforms(gids);

        // Place inactive cells
        for (uint64_t i = 0; i < gids.size(); ++i)
            if (_emitors.find(i) == _emitors.end() &&
                _receptors.find(i) == _receptors.end())
            {
                const auto centerSource =
                    transforms[i].getTranslation() * scale;
                const auto radiusSource =
                    _geometryParameters.getRadiusMultiplier();
                scene.addSphere(NB_SYSTEM_MATERIALS,
                                {centerSource, radiusSource});
            }

        // Place active cells and connections
        std::stringstream message;
        message << "Building scene with " << _emitors.size() << " cells ...";
        boost::progress_display progress(_emitors.size(), std::cout,
                                         "[INFO ] " + message.str() +
                                             "\n[INFO ] ",
                                         "[INFO ] ", "[INFO ] ");
        for (const auto& emitor : _emitors)
        {
            // Cells
            const auto centerSource =
                transforms[emitor.first].getTranslation() * scale;
            // The color of the cell depends on the number of outgoing
            // connections
            const size_t materialId =
                NB_SYSTEM_MATERIALS + emitor.second.size();

            // The size of the cell depends on the number of incoming
            // connections
            const float ratio =
                1.f + (_receptors.find(emitor.first) == _receptors.end()
                           ? 0.f
                           : INCOMING_CONNECTIONS_RADIUS_MULTIPLIER *
                                 (_receptors[emitor.first]));
            const auto radiusSource =
                ratio * _geometryParameters.getRadiusMultiplier();

            bool createSphere = true;
            if (!_geometryParameters.getCircuitMeshFolder().empty())
            {
                const uint64_t gid = *std::next(gids.begin(), emitor.first);
                const auto transformation =
                    _geometryParameters.getCircuitMeshTransformation()
                        ? transforms[emitor.first]
                        : Matrix4f();
                createSphere = !_importMesh(gid, transformation, materialId,
                                            scene, meshLoader);
            }
            if (createSphere)
                scene.addSphere(materialId, {centerSource, radiusSource});

            // Connections
            if (emitor.second.size() < dimensionRange.x() ||
                emitor.second.size() > dimensionRange.y())
            {
                ++progress;
                continue;
            }

            if (_geometryParameters.getConnectivityShowConnections())
                for (const auto& dest : emitor.second)
                {
                    const auto centerDest =
                        transforms[dest].getTranslation() * scale;
                    const auto radiusDest =
                        CONNECTION_RADIUS_MULTIPLIER *
                        _geometryParameters.getRadiusMultiplier();

                    // A connection is represented as an 'arrow' starting with a
                    // cone for the 10% of its length, and a cylinder for the
                    // remaining 90%
                    const auto arrowTarget =
                        centerSource + 0.1f * (centerDest - centerSource);
                    scene.addCone(materialId, {centerSource, arrowTarget,
                                               radiusSource, radiusDest});
                    scene.addCylinder(materialId,
                                      {arrowTarget, centerDest, radiusDest});
                }
            ++progress;
        }
    }
    catch (const std::exception& e)
    {
        BRAYNS_ERROR << e.what() << std::endl;
        return false;
    }
    return true;
}
}
