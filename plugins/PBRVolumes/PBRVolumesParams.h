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

#include <brayns/network/messages/ActionMessage.h>
#include <brayns/common/types.h>

// Commom parameters for all type of volumes
#define COMMON_VOLUME_PROPS \
    MESSAGE_ENTRY(std::string, name, "Volume scene name") \
    MESSAGE_ENTRY(std::vector<float>, absorption, "Absorption spectrum") \
    MESSAGE_ENTRY(std::vector<float>, scattering, "Scattering spectrum") \
    MESSAGE_ENTRY(double, scale, "Spectrums scale") \
    MESSAGE_ENTRY(double, g, "Anisotropy parameter") \
    MESSAGE_ENTRY(std::vector<float>, le, "Radiance spectrum") \

/**
 * @brief The AddHomogeneusVolume struct
 * Struct to add a constant density volume
 * enclosed in a box defined by p0 (min) and
 * p1 (max)
 */
struct AddHomogeneusVolume : public brayns::Message
{
    MESSAGE_BEGIN(AddHomogeneusVolume)
    COMMON_VOLUME_PROPS
    MESSAGE_ENTRY(double, density, "Constant volume density")
    MESSAGE_ENTRY(std::vector<float>, p0, "Volume minimum bound")
    MESSAGE_ENTRY(std::vector<float>, p1, "Volume maximum bound")
};

/**
 * @brief The AddHomogeneusVolumeToModel struct
 * Struct to add a constant density volume enclosed
 * in the shapes of the model defined by its model ID
 */
struct AddHomogeneusVolumeToModel : public brayns::Message
{
    MESSAGE_BEGIN(AddHomogeneusVolumeToModel)
    COMMON_VOLUME_PROPS
    MESSAGE_ENTRY(double, density, "Constant volume density")
    MESSAGE_ENTRY(int32_t, modelId, "Model to which to add the volume")
};

/**
 * @brief The AddHeterogeneusVolume struct
 * Struct to add an heterogeneus density volume
 * (whose density varies between min density and
 * max density) enclosed in a box defined by p0 (min)
 * and p1 (max). Only available for PBRT-v2. PBRT-v3
 * will convert it into a Homogeneus volume taking as
 * density (scale) the middle value between min and max
 */
struct AddHeterogeneusVolume : public brayns::Message
{
    MESSAGE_BEGIN(AddHeterogeneusVolume)
    COMMON_VOLUME_PROPS
    MESSAGE_ENTRY(double, minDensity, "Density minimum value")
    MESSAGE_ENTRY(double, maxDensity, "Density maximum value")
    MESSAGE_ENTRY(std::vector<float>, p0, "Volume minimum bound")
    MESSAGE_ENTRY(std::vector<float>, p1, "Volume maximum bound")
};

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
struct AddGridVolume : public brayns::Message
{
    MESSAGE_BEGIN(AddGridVolume)
    COMMON_VOLUME_PROPS
    MESSAGE_ENTRY(std::string, gridType, "Type of grid (PBRT-V2 only, values \"grid\" or \"vsd\"")
    MESSAGE_ENTRY(std::vector<float>, p0, "Volume minimum bound")
    MESSAGE_ENTRY(std::vector<float>, p1, "Volume maximum bound")
    MESSAGE_ENTRY(std::vector<float>, density, "Volume grid density values")
    MESSAGE_ENTRY(int32_t, nx, "Grid X dimension")
    MESSAGE_ENTRY(int32_t, ny, "Grid Y dimension")
    MESSAGE_ENTRY(int32_t, nz, "Grid Z dimension")
};

#endif
