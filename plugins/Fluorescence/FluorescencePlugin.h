/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "FluorescenceParams.h"

#include <brayns/common/types.h>
#include <brayns/network/interface/ActionInterface.h>
#include <brayns/pluginapi/ExtensionPlugin.h>

/**
   The CircuitInfo plugin gives access to circuit information stored
   in sonata-generated circuit files, those which Brion is unable
   to read.
 */
class FluorescencePlugin : public brayns::ExtensionPlugin
{
public:
    FluorescencePlugin();
    ~FluorescencePlugin();
    void init() final;

private:
    /**
     * @brief Adds a rectangle shaped sensor to the scene
     *
     * @return RequestResponse with the sucess status of the request
     */
    brayns::Message _addRectangleSensor(const AddRectangleSensorRequest&);

    /**
     * @brief Adds a disk shaped sensor to the scene
     *
     * @return RequestResponse with the sucess status of the request
     */
    brayns::Message _addDiskSensor(const AddDiskSensorRequest&);

    /**
     * @brief Adds a Fluorescent homogeneus volume to the scene
     *
     * @return RequestResponse with the sucess status of the request
     */
    brayns::Message _addFluorescentVolume(const AddFluorescentVolume&);

    /**
     * @brief Adds a Fluorescent annotated volume to the scene
     *
     * @return RequestResponse with the sucess status of the request
     */
    brayns::Message _addFluorescentAnnotatedVolume(
        const AddFluorescentAnnotatedVolume&);

    /**
     * @brief Adds a Fluorescent binary volume to the scene
     *
     * @return RequestResponse with the sucess status of the request
     */
    brayns::Message _addFluorescentBinaryVolume(
        const AddFluorescentBinaryVolume&);

    /**
     * @brief Adds a Fluorescent grid volume to the scene, whose data is loaded
     *          from a file on disk
     *
     * @return RequestResponse with the sucess status of the request
     */
    brayns::Message _addFluorescentGridVolumeFromFile(
        const AddFluorescentGridFromFileVolume&);

    /**
     * @brief Adds a Fluorescent grid volume whose data is provided in the
     * parameters
     *
     * @return RequestResponse with the sucess status of the request
     */
    brayns::Message _addFluorescentGridVolume(const AddFluorescentGridVolume&);

    /**
     * @brief Adds a Fluorescent scattering volume to the scene
     *
     * @return RequestResponse with the sucess status of the request
     */
    brayns::Message _addFluorescentScatteringVolume(
        const AddFluorescentScatteringVolume&);

    /**
     * @brief Adds a Fluorescent scattering grid volume to the scene, whose data
     *          is provided in the parameters
     *
     * @return RequestResponse with the sucess status of the request
     */
    brayns::Message _addFluorescentScatteringGridVolume(
        const AddFluorescentScatteringGridVolume&);

    /**
     * @brief Adds a Fluorescent scattering grid volume to the scene, whose data
     *          is loaded from a file on disk
     *
     * @return RequestResponse with the sucess status of the request
     */
    brayns::Message _addFluorescentScatteringGridVolumeFromFile(
        const AddFluorescentScatteringGridFromFileVolume&);
};

