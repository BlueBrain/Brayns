/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jafet Villafranca Diaz <jafet.villafrancadiaz@epfl.ch>
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

#include "NESTLoader.h"

#include <brayns/common/geometry/Sphere.h>
#include <brayns/common/log.h>
#include <brayns/common/types.h>
#include <brayns/io/simulation/SpikeSimulationHandler.h>

#if (BRAYNS_USE_BRION)
#include <H5Cpp.h>
#endif
#include <algorithm>
#include <fstream>

namespace
{
const uint32_t NEST_MAGIC = 0xf0a;
const uint32_t NEST_VERSION = 1;
const uint32_t NEST_HEADER_SIZE = 2 * sizeof(uint32_t);
const float NEST_TIMESTEP = 0.1f;
const uint32_t NEST_OFFSET = 2;
const float DEFAULT_ALPHA = 1.f;
}

namespace brayns
{
NESTLoader::NESTLoader(const GeometryParameters& geometryParameters)
    : _geometryParameters(geometryParameters)
    , _spikesStart(0.f)
    , _spikesEnd(0.f)
{
}

#if (BRAYNS_USE_BRION)
void NESTLoader::importCircuit(const std::string& filepath, Scene& scene)
{
    BRAYNS_INFO << "Loading NEST cells from circuit " << filepath << std::endl;

    H5::H5File neurons(filepath.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

    H5::DataSet posDataset;
    posDataset = neurons.openDataSet("/x");

    hsize_t dims[2];
    H5::DataSpace dspace = posDataset.getSpace();
    dspace.getSimpleExtentDims(dims);

    _frameSize = dims[0];

    floats xPos(_frameSize);
    floats yPos(_frameSize);
    floats zPos(_frameSize);
    int16_ts xColor(_frameSize);
    int16_ts yColor(_frameSize);
    int16_ts zColor(_frameSize);

    posDataset.read(xPos.data(), H5::PredType::NATIVE_FLOAT);
    posDataset = neurons.openDataSet("/y");
    posDataset.read(yPos.data(), H5::PredType::NATIVE_FLOAT);
    posDataset = neurons.openDataSet("/z");
    posDataset.read(zPos.data(), H5::PredType::NATIVE_FLOAT);
    posDataset = neurons.openDataSet("/colorx");
    posDataset.read(xColor.data(), H5::PredType::NATIVE_INT16);
    posDataset = neurons.openDataSet("/colory");
    posDataset.read(yColor.data(), H5::PredType::NATIVE_INT16);
    posDataset = neurons.openDataSet("/colorz");
    posDataset.read(zColor.data(), H5::PredType::NATIVE_INT16);

    std::map<size_t, Vector4f> materials;
    for (uint64_t gid = 0; gid < _frameSize; ++gid)
    {
        // Create a unique index for the combination of R,G and B values. This
        // index will then be used to identify the color that should be applied
        // to the sphere by the renderer
        const size_t index =
            xColor[gid] * 65536 + yColor[gid] * 256 + zColor[gid];
        materials[index] =
            Vector4f(float(xColor[gid]) / 256.f, float(yColor[gid]) / 256.f,
                     float(zColor[gid]) / 256.f, DEFAULT_ALPHA);
    }

    auto& transferFunction = scene.getTransferFunction();
    transferFunction.clear();

    // Realign materials
    std::map<size_t, float> materialMapping;
    size_t i = 0;
    transferFunction.getDiffuseColors().resize(materials.size(), Vector4f());
    for (const auto& material : materials)
    {
        transferFunction.getDiffuseColors()[i] = material.second;
        materialMapping[material.first] = i;
        ++i;
    }
    transferFunction.setValuesRange(Vector2f(0.f, materialMapping.size()));

    BRAYNS_INFO << "Number of materials: " << materialMapping.size()
                << std::endl;

    SpheresMap& spheres = scene.getSpheres();
    spheres[0].reserve(_frameSize);
    _positions.reserve(_frameSize);
    const float radius = _geometryParameters.getRadiusMultiplier();

    for (uint64_t gid = 0; gid < _frameSize; ++gid)
    {
        // Create a unique index for the combination of R,G and B values. This
        // index will then be used to identify the color that should be applied
        // to the sphere by the renderer
        const size_t index =
            xColor[gid] * 65536 + yColor[gid] * 256 + zColor[gid];
        const Vector3f center(xPos[gid], yPos[gid], zPos[gid]);
        _positions.push_back(center);
        scene.addSphere(0,
                        {center, radius, 0.f, {materialMapping[index], 0.f}});
        updateProgress("Loading neurons...", spheres[0].size(), _frameSize);
    }

    BRAYNS_INFO << "Finished loading " << _frameSize << " neurons" << std::endl;
}

bool NESTLoader::importSpikeReport(const std::string& filename)
{
    if (!_loadBinarySpikes(filename))
    {
        BRAYNS_ERROR << "No valid binary .spikes file found" << std::endl;
        return false;
    }

    const uint64_t nbFrames = (_spikesEnd - _spikesStart) / NEST_TIMESTEP;
    _spikingTimes.resize(_frameSize, -1.f);

    BRAYNS_INFO << "Cache file does not exist, creating it" << std::endl;
    const std::string& cacheFile = _geometryParameters.getNESTCacheFile();
    std::ofstream file(cacheFile, std::ios::binary);

    if (!file.is_open())
    {
        BRAYNS_ERROR << "Failed to create cache file" << std::endl;
        return false;
    }

    SpikeSimulationHandler simulationHandler(_geometryParameters);
    // Write header
    simulationHandler.setNbFrames(nbFrames);
    simulationHandler.setFrameSize(_frameSize);
    simulationHandler.writeHeader(file);

    BRAYNS_INFO << "Spike report contains " << nbFrames << " frames of "
                << _frameSize << " values each" << std::endl;

    // Write body
    for (float timestamp = _spikesStart; timestamp < _spikesEnd;
         timestamp += NEST_TIMESTEP)
    {
        _load(timestamp);
        simulationHandler.writeFrame(file, _spikingTimes);
        if (file.bad())
            throw std::runtime_error(
                "Could not write cache file (disk full?), aborting");
    }
    file.close();

    BRAYNS_INFO << "----------------------------------------" << std::endl;
    BRAYNS_INFO << "Number of frames: " << nbFrames << std::endl;
    BRAYNS_INFO << "Frame size      : " << _frameSize << std::endl;
    BRAYNS_INFO << "----------------------------------------" << std::endl;
    return true;
}

bool NESTLoader::_loadBinarySpikes(const std::string& spikesFilename)
{
    std::ifstream file(spikesFilename, std::ios::binary);
    file.seekg(0, std::ios::end);
    const size_t fileSize = file.tellg();
    file.seekg(0);

    _nbElements =
        (fileSize - NEST_HEADER_SIZE) / (sizeof(uint32_t) + sizeof(float));
    BRAYNS_INFO << "Loading " << _nbElements << " spikes from "
                << spikesFilename << std::endl;

    // Parse header
    uint32_t magic;
    file.read((char*)&magic, sizeof(uint32_t));

    if (NEST_MAGIC != magic)
        return false;

    uint32_t version;
    file.read((char*)&version, sizeof(uint32_t));
    if (NEST_VERSION != version)
        return false;

    // Parse elements
    uint32_t gid;
    float value;
    _values.reserve(_nbElements);
    _gids.reserve(_nbElements);
    size_t i = 0;
    while (!file.eof())
    {
        file.read((char*)&value, sizeof(float));
        _values.push_back(value);
        file.read((char*)&gid, sizeof(uint32_t));
        _gids.push_back(gid);
        ++i;
        updateProgress("Loading spikes...", i, _nbElements);
    }

    _spikesStart = _values[0];             // First spike timestamp after header
    _spikesEnd = _values[_nbElements - 1]; // Last spike timestamp

    BRAYNS_INFO << "Loaded " << i << " elements out of " << _nbElements
                << std::endl;
    BRAYNS_INFO << "Spikes interval: [" << _spikesStart << " - " << _spikesEnd
                << "]" << std::endl;

    file.close();
    return true;
}

bool NESTLoader::_load(const float timestamp)
{
    const float start = timestamp;
    const float end = timestamp + NEST_TIMESTEP;

    BRAYNS_DEBUG << "Loading spikes at timestamp: " << timestamp << " ("
                 << start << " ms)" << std::endl;

    size_t nbSpikes = 0;
    for (size_t i = 0; i < _nbElements; ++i)
    {
        const float time = _values[i];
        // With the next check, simulation only plays forward because the value
        // that is stored
        // is the last time the spike occurred for this neuron.
        if (time < start)
            continue;

        if (time >= end)
            break;

        const uint32_t gid = _gids[i] - NEST_OFFSET;

        // We store the frame on which the spike happens, as the renderer keeps
        // track of the current timestamp
        assert(gid < _spikingTimes.size());
        _spikingTimes[gid] = time;

        ++nbSpikes;
    }
    BRAYNS_INFO << "Nb Spikes for timestamp " << timestamp << " [" << start
                << "-" << end << "]: " << nbSpikes << std::endl;

    return true;
}
#else
void NESTLoader::importCircuit(const std::string&, Scene&, size_t&)
{
    BRAYNS_ERROR << "Brion is required to load circuits" << std::endl;
}

bool NESTLoader::importSpikeReport(const std::string&)
{
    BRAYNS_ERROR << "Brion is required to load circuits" << std::endl;
    return false;
}

bool NESTLoader::_loadBinarySpikes(const std::string&)
{
    BRAYNS_ERROR << "Brion is required to load circuits" << std::endl;
    return false;
}

bool NESTLoader::_load(const float)
{
    BRAYNS_ERROR << "Brion is required to load circuits" << std::endl;
    return false;
}

#endif
}
