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

#ifndef FLUORESCENCEPARAMS_H
#define FLUORESCENCEPARAMS_H

#include <brayns/common/ActionMessage.h>
#include <brayns/common/types.h>

#define COMMON_SENSOR_PROPS \
    MESSAGE_ENTRY(std::vector<float>, rotation, "Sensor 3D Rotation") \
    MESSAGE_ENTRY(std::vector<float>, translation, "Sensor 3D Translation") \
    MESSAGE_ENTRY(std::vector<float>, scale, "Sensor 3D Scale") \
    MESSAGE_ENTRY(std::string, reference, "Sensor name") \
    MESSAGE_ENTRY(int32_t, xpixels, "Sensor pixel capture width") \
    MESSAGE_ENTRY(int32_t, ypixels, "Sensor pixel capture height") \
    MESSAGE_ENTRY(double, fov, "Sensor capture field of view") \


#define COMMON_VOLUME_PROPS \
    MESSAGE_ENTRY(std::vector<float>, p0, "Minimum axis-aligned volume bound") \
    MESSAGE_ENTRY(std::vector<float>, p1, "Maximum axis-aligned volume bound") \
    MESSAGE_ENTRY(std::string, name, "Volume scene name") \

/**
 * @brief The AddDiskSensorRequest struct
 * Struct to add a disk shaped sensor to the scene
 */
struct AddDiskSensorRequest : public brayns::Message
{
    MESSAGE_BEGIN(AddDiskSensorRequest)
    COMMON_SENSOR_PROPS
    MESSAGE_ENTRY(double, height, "Sensor disk height")
    MESSAGE_ENTRY(double, radius, "Sensor disk radius")
    MESSAGE_ENTRY(double, innerRadius, "Sensor disk inner radius")
    MESSAGE_ENTRY(double, phi, "Sensor available surface (Max = 2*PI)")
};

/**
 * @brief The AddRectangleSensorRequest struct
 * Struct to add a rectangle shaped sensor to the scene
 */
struct AddRectangleSensorRequest : public brayns::Message
{
    MESSAGE_BEGIN(AddRectangleSensorRequest)
    COMMON_SENSOR_PROPS
    MESSAGE_ENTRY(double, height, "Sensor rectangle height")
    MESSAGE_ENTRY(double, x, "Sensor rectangle width")
    MESSAGE_ENTRY(double, y, "Sensor rectangle height")
};

/**
 * @brief The AddFluorescentVolume struct
 * Struct to add a Fluorescent homogeneous volume
 * enclosed in a box
 */
struct AddFluorescentVolume : public brayns::Message
{
    MESSAGE_BEGIN(AddFluorescentVolume)
    COMMON_VOLUME_PROPS
    MESSAGE_ENTRY(std::vector<float>, fex, "Fluorescence excitatory spectrum")
    MESSAGE_ENTRY(std::vector<float>, fem, "Fluorescence emissive spectrum")
    MESSAGE_ENTRY(double, epsilon, "Fluorescence epsilon treshold")
    MESSAGE_ENTRY(double, c, "Fluorescence concentration")
    MESSAGE_ENTRY(double, yield, "Fluorescence quantum yield")
    MESSAGE_ENTRY(double, gf, "Fluorescence anisotropy parameter")
};

/**
 * @brief The AddFluorescentAnnotatedVolume struct
 * Struct to add a Fluorescent annotated volume with
 * multiple tags, enclosed in a box. Data is loaded
 * from a a file given a path.
 */
struct AddFluorescentAnnotatedVolume : public brayns::Message
{
    MESSAGE_BEGIN(AddFluorescentAnnotatedVolume)
    COMMON_VOLUME_PROPS
    MESSAGE_ENTRY(double, g, "Anisotropy parameter")
    MESSAGE_ENTRY(int32_t, ntags, "Number of tags")
    MESSAGE_ENTRY(std::vector<float>, fexs, "Fluorescence exictatory spectrums")
    MESSAGE_ENTRY(std::vector<float>, fems, "Fluorescence emissive spectrums")
    MESSAGE_ENTRY(std::vector<float>, epsilons, "Fluorescence epsilon tresholds")
    MESSAGE_ENTRY(std::vector<float>, cs, "Fluorescence concentrations")
    MESSAGE_ENTRY(std::vector<float>, yields, "Fluorescence quantum yields")
    MESSAGE_ENTRY(std::vector<float>, gfs, "Fluorescence anisotropy parameters")
    MESSAGE_ENTRY(std::string, prefix, "Path to volume density file")
};

/**
 * @brief The AddFluorescentBinaryVolume struct
 * Struct used to add a Fluorescent binary volume,
 * enclosed in a box. Data is loaded from a file given a
 * path
 */
struct AddFluorescentBinaryVolume : public brayns::Message
{
    MESSAGE_BEGIN(AddFluorescentBinaryVolume)
    COMMON_VOLUME_PROPS
    MESSAGE_ENTRY(double, g, "Anisotropy parameter")
    MESSAGE_ENTRY(std::vector<float>, fex, "Fluorescence exictatory spectrum")
    MESSAGE_ENTRY(std::vector<float>, fem, "Fluorescence emissive spectrum")
    MESSAGE_ENTRY(double, epsilon, "Fluorescence epsilon treshold")
    MESSAGE_ENTRY(double, c, "Fluorescence concentration")
    MESSAGE_ENTRY(double, yield, "Fluorescence quantum yield")
    MESSAGE_ENTRY(double, gf, "Fluorescence anisotropy parameter")
    MESSAGE_ENTRY(std::string, prefix, "Path to the volume density file")
};

/**
 * @brief The AddFluorescentGridFromFileVolume struct
 * Struct used to add a Fluorescent grid volume whose data will
 * be loaded from disk given a path. The volume will be enclosed
 * in a box
 */
struct AddFluorescentGridFromFileVolume : public brayns::Message
{
    MESSAGE_BEGIN(AddFluorescentGridFromFileVolume)
    COMMON_VOLUME_PROPS
    MESSAGE_ENTRY(double, g, "Anisotropy parameter")
    MESSAGE_ENTRY(std::vector<float>, fex, "Fluorescence excitatory spectrum")
    MESSAGE_ENTRY(std::vector<float>, fem, "Fluorescence emissive spectrum")
    MESSAGE_ENTRY(double, epsilon, "Fluorescence epsilon treshold")
    MESSAGE_ENTRY(double, c, "Fluorescence concentration")
    MESSAGE_ENTRY(double, yield, "Fluorescence quantum yield")
    MESSAGE_ENTRY(double, gf, "Fluorescence anysotropy parameter")
    MESSAGE_ENTRY(std::string, prefix, "Path to the volume density file")
};

/**
 * @brief The AddFluorescentGridVolume struct
 * Struct used to add a Fluorescent grid volume whose data is provided.
 * The dimensions of the grid must be provided aswell and they must match
 * in size the number of density elements. The volume will be enclosed
 * in a box
 */
struct AddFluorescentGridVolume : public brayns::Message
{
    MESSAGE_BEGIN(AddFluorescentGridVolume)
    COMMON_VOLUME_PROPS
    MESSAGE_ENTRY(double, g, "Anisotropy parameter")
    MESSAGE_ENTRY(std::vector<float>, fex, "Fluorescence exictatory spectrum")
    MESSAGE_ENTRY(std::vector<float>, fem, "Fluorescence emissive spectrum")
    MESSAGE_ENTRY(double, epsilon, "Fluorescence epsilon treshold")
    MESSAGE_ENTRY(double, c, "Fluorescence concentration")
    MESSAGE_ENTRY(double, yield, "Fluorescence quantum yield")
    MESSAGE_ENTRY(double, gf, "Fluorescence anisotropy parameter")
    MESSAGE_ENTRY(std::vector<float>, density, "Density grid value")
    MESSAGE_ENTRY(int32_t, nx, "Gird x dimension")
    MESSAGE_ENTRY(int32_t, ny, "Grid y dimension")
    MESSAGE_ENTRY(int32_t, nz, "Grid z dimension")
};

/**
 * @brief The AddFluorescentScatteringVolume struct
 * Struct used to add a homogeneus fluorescence scattering volume.
 * The volume will be enclosed in a box.
 */
struct AddFluorescentScatteringVolume : public brayns::Message
{
    MESSAGE_BEGIN(AddFluorescentScatteringVolume)
    COMMON_VOLUME_PROPS
    MESSAGE_ENTRY(std::vector<float>, absorption, "Absoption spectrum")
    MESSAGE_ENTRY(std::vector<float>, scattering, "Scattering spectrum")
    MESSAGE_ENTRY(std::vector<float>, fem, "Fluorescence emissive spectrum")
    MESSAGE_ENTRY(std::vector<float>, fex, "Fluorescence excitatory spectrum")
    MESSAGE_ENTRY(double, mweight, "Weight")
    MESSAGE_ENTRY(double, epsilon, "Fluorescence epsilon treshold")
    MESSAGE_ENTRY(double, c, "Fluorescence concentration")
    MESSAGE_ENTRY(double, yield, "Fluorescence quantum yield")
    MESSAGE_ENTRY(double, sscale, "Scattering scaling")
    MESSAGE_ENTRY(double, fscale, "Fluorescence scaling")
    MESSAGE_ENTRY(double, g, "Anisotropy parameter")
    MESSAGE_ENTRY(double, gf, "Fluorescence anisotropy parameter")
    MESSAGE_ENTRY(double, density, "Constant volume density")
    MESSAGE_ENTRY(std::vector<float>, Le, "Radiance spectrum")
};

/**
 * @brief The AddFluorescentScatteringGridFromFileVolume struct
 * Struct used to add a fluorescence scattering grid volume.
 * The volume will be enclosed in a box. The data will be loaded
 * from disk from the given path
 */
struct AddFluorescentScatteringGridFromFileVolume : public brayns::Message
{
    MESSAGE_BEGIN(AddFluorescentScatteringGridFromFileVolume)
    COMMON_VOLUME_PROPS
    MESSAGE_ENTRY(std::vector<float>, absorption, "Absoption spectrum")
    MESSAGE_ENTRY(std::vector<float>, scattering, "Scattering spectrum")
    MESSAGE_ENTRY(std::vector<float>, fem, "Fluorescence emissive spectrum")
    MESSAGE_ENTRY(std::vector<float>, fex, "Fluorescence excitatory spectrum")
    MESSAGE_ENTRY(double, mweight, "Weight")
    MESSAGE_ENTRY(double, epsilon, "Fluorescence epsilon treshold")
    MESSAGE_ENTRY(double, c, "Fluorescence concentration")
    MESSAGE_ENTRY(double, yield, "Fluorescence quantum yield")
    MESSAGE_ENTRY(double, sscale, "Scattering scaling")
    MESSAGE_ENTRY(double, fscale, "Fluorescence scaling")
    MESSAGE_ENTRY(double, g, "Anisotropy parameter")
    MESSAGE_ENTRY(double, gf, "Fluorescence anisotropy parameter")
    MESSAGE_ENTRY(std::vector<float>, Le, "Radiance spectrum")
    MESSAGE_ENTRY(std::string, prefix, "Path to volume density file")
};

/**
 * @brief The AddFluorescentScatteringGridFromFileVolume struct
 * Struct used to add a fluorescence scattering grid volume.
 * The volume will be enclosed in a box. The density is given
 * on a per cell basis. The grid dimensions are given as well,
 * and must match, in size, the number of density elements
 */
struct AddFluorescentScatteringGridVolume : public brayns::Message
{
    MESSAGE_BEGIN(AddFluorescentScatteringGridVolume)
    COMMON_VOLUME_PROPS
    MESSAGE_ENTRY(std::vector<float>, absorption, "Absoption spectrum")
    MESSAGE_ENTRY(std::vector<float>, scattering, "Scattering spectrum")
    MESSAGE_ENTRY(std::vector<float>, fem, "Fluorescence emissive spectrum")
    MESSAGE_ENTRY(std::vector<float>, fex, "Fluorescence excitatory spectrum")
    MESSAGE_ENTRY(double, mweight, "Weight")
    MESSAGE_ENTRY(double, epsilon, "Fluorescence epsilon treshold")
    MESSAGE_ENTRY(double, c, "Fluorescence concentration")
    MESSAGE_ENTRY(double, yield, "Fluorescence quantum yield")
    MESSAGE_ENTRY(double, sscale, "Scattering scaling")
    MESSAGE_ENTRY(double, fscale, "Fluorescence scaling")
    MESSAGE_ENTRY(double, g, "Anisotropy parameter")
    MESSAGE_ENTRY(double, gf, "Fluorescence anisotropy parameter")
    MESSAGE_ENTRY(std::vector<float>, Le, "Radiance spectrum")
    MESSAGE_ENTRY(std::vector<float>, density, "Grid density values")
    MESSAGE_ENTRY(int32_t, nx, "Gird x dimension")
    MESSAGE_ENTRY(int32_t, ny, "Grid y dimension")
    MESSAGE_ENTRY(int32_t, nz, "Grid z dimension")
};

#endif
