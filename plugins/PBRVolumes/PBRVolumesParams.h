/* Copyright (c) 2020, EPFL/Blue Brain Project
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

#ifndef PBRVOLUMES_H
#define PBRVOLUMES_H

#include <brayns/common/types.h>

// Commom parameters used to give information
// about wether the request was sucessfuly parsed
// from JSON
#define COMMON_PARAMS \
    bool parsed{true}; \
    std::string parseMessage{""}; \

// Commom parameters for all type of volumes
#define COMMON_VOLUME_PROPS \
    std::string name; \
    floats absorption; \
    floats scattering; \
    float scale; \
    float g; \
    floats Le; \

/**
 * @brief The AddHomogeneusVolume struct
 * Struct to add a constant density volume
 * enclosed in a box defined by p0 (min) and
 * p1 (max)
 */
struct AddHomogeneusVolume
{
    COMMON_PARAMS
    COMMON_VOLUME_PROPS
    float density;
    floats p0;
    floats p1;
};
bool from_json(AddHomogeneusVolume& request, const std::string& payload);

/**
 * @brief The AddHomogeneusVolumeToModel struct
 * Struct to add a constant density volume enclosed
 * in the shapes of the model defined by its model ID
 */
struct AddHomogeneusVolumeToModel
{
    COMMON_PARAMS
    COMMON_VOLUME_PROPS
    float density;
    int32_t modelId;
};
bool from_json(AddHomogeneusVolumeToModel& request, const std::string& payload);

/**
 * @brief The AddHeterogeneusVolume struct
 * Struct to add an heterogeneus density volume
 * (whose density varies between min density and
 * max density) enclosed in a box defined by p0 (min)
 * and p1 (max). Only available for PBRT-v2. PBRT-v3
 * will convert it into a Homogeneus volume taking as
 * density (scale) the middle value between min and max
 */
struct AddHeterogeneusVolume
{
    COMMON_PARAMS
    COMMON_VOLUME_PROPS
    float minDensity;
    float maxDensity;
    floats p0;
    floats p1;
};
bool from_json(AddHeterogeneusVolume& request, const std::string& payload);

/**
 * @brief The AddGridVolume struct
 * Struct to add a grid Volume enclosed in a box
 * defined by p0 (min) and p1 (max). The dimensions
 * of the grid ar given by nx, ny, nz, and the values
 * of each cell is given in desnity. The number of
 * elements in density must match nx*ny*nz. For PBRT-v2
 * the gridType property defines the type of grid
 * ('grid', 'binarygrid', 'vsdgrid')
 */
struct AddGridVolume
{
    COMMON_PARAMS
    COMMON_VOLUME_PROPS
    std::string gridType;
    floats p0;
    floats p1;
    floats density;
    int32_t nx;
    int32_t ny;
    int32_t nz;
};
bool from_json(AddGridVolume& request, const std::string& payload);

/**
 * @brief The AddVolumeResponse struct
 * Data returned after requesting to add a volume.
 * An error 0 indicates the operation was sucessful.
 * Any other value indicates an error and the message
 * field will carry information about the error.
 */
struct AddVolumeResponse
{
    int error;
    std::string message;
};
std::string to_json(const AddVolumeResponse& response);

#endif
