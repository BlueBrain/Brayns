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

#include "PBRVolumesParams.h"

#include <brayns/common/types.h>
#include <brayns/network/interface/ActionInterface.h>
#include <brayns/pluginapi/ExtensionPlugin.h>

/**
   The CircuitInfo plugin gives access to circuit information stored
   in sonata-generated circuit files, those which Brion is unable
   to read.
 */
class PBRVolumesPlugin : public brayns::ExtensionPlugin
{
public:
    PBRVolumesPlugin();
    ~PBRVolumesPlugin();
    void init() final;

private:
    /**
     * @brief Adds a new model with a homogeneus (constant) density
     *        volume to the scene, enclosed in a regular grid
     * @return brayns::Message with information about request sucess
     */
    brayns::Message _addHomogeneusVolume(const AddHomogeneusVolume&);

    /**
     * @brief Adds a homogeneus (constant) density volume to be enclosed
     *        inside the shapes of a given model
     * @return brayns::Message with information about request sucess
     */
    brayns::Message _addHomogeneusVolumeToModel(
        const AddHomogeneusVolumeToModel&);

    /**
     * @brief Adds a new model with a heterogeneus (non constant) density
     *        volume to the scene, enclosed in a regular grid
     * @return brayns::Message with information about request sucess
     */
    brayns::Message _addHeterogeneusVolume(const AddHeterogeneusVolume&);

    /**
     * @brief Adds a new model with a grid volume (the density
     *        is specified on a per grid cell basis) to the scene,
     *        enclosed in a regular grid
     * @return brayns::Message with information about request sucess
     */
    brayns::Message _addGridVolume(const AddGridVolume&);
};

