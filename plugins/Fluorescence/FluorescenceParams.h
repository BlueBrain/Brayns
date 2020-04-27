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

#include <brayns/common/types.h>

#define COMMON_PARAMS \
    bool parsed{true}; \
    std::string parseMessage{""}; \


#define COMMON_SENSOR_PROPS \
    floats rotation; \
    floats translation; \
    floats scale; \
    std::string reference; \
    int xpixels; \
    int ypixels; \
    float fov; \


#define COMMON_VOLUME_PROPS \
    floats p0; \
    floats p1; \
    std::string name; \

/**
 * @brief The AddDiskSensorRequest struct
 * Struct to add a disk shaped sensor to the scene
 */
struct AddDiskSensorRequest
{
    COMMON_PARAMS
    COMMON_SENSOR_PROPS
    float height;
    float radius;
    float innerRadius;
    float phi;
};
bool from_json(AddDiskSensorRequest& request, const std::string& payload);

/**
 * @brief The AddRectangleSensorRequest struct
 * Struct to add a rectangle shaped sensor to the scene
 */
struct AddRectangleSensorRequest
{
    COMMON_PARAMS
    COMMON_SENSOR_PROPS
    float height;
    float x;
    float y;
};
bool from_json(AddRectangleSensorRequest& request, const std::string& payload);

/**
 * @brief The AddFluorescentVolume struct
 * Struct to add a Fluorescent homogeneous volume
 * enclosed in a box
 */
struct AddFluorescentVolume
{
    COMMON_PARAMS
    COMMON_VOLUME_PROPS
    floats fex;
    floats fem;
    float epsilon;
    float c;
    float yield;
    float gf;
};
bool from_json(AddFluorescentVolume& request, const std::string& payload);

/**
 * @brief The AddFluorescentAnnotatedVolume struct
 * Struct to add a Fluorescent annotated volume with
 * multiple tags, enclosed in a box. Data is loaded
 * from a a file given a path.
 */
struct AddFluorescentAnnotatedVolume
{
    COMMON_PARAMS
    COMMON_VOLUME_PROPS
    float g;
    int32_t ntags;
    floats fexs;
    floats fems;
    floats epsilons;
    floats cs;
    floats yields;
    floats gfs;
    std::string prefix;
};
bool from_json(AddFluorescentAnnotatedVolume& request, const std::string& payload);

/**
 * @brief The AddFluorescentBinaryVolume struct
 * Struct used to add a Fluorescent binary volume,
 * enclosed in a box. Data is loaded from a file given a
 * path
 */
struct AddFluorescentBinaryVolume
{
    COMMON_PARAMS
    COMMON_VOLUME_PROPS
    float g;
    floats fex;
    floats fem;
    float epsilon;
    float c;
    float yield;
    float gf;
    std::string prefix;
};
bool from_json(AddFluorescentBinaryVolume& request, const std::string& payload);

/**
 * @brief The AddFluorescentGridFromFileVolume struct
 * Struct used to add a Fluorescent grid volume whose data will
 * be loaded from disk given a path. The volume will be enclosed
 * in a box
 */
struct AddFluorescentGridFromFileVolume
{
    COMMON_PARAMS
    COMMON_VOLUME_PROPS
    float g;
    floats fex;
    floats fem;
    float epsilon;
    float c;
    float yield;
    float gf;
    std::string prefix;
};
bool from_json(AddFluorescentGridFromFileVolume& request, const std::string& payload);

/**
 * @brief The AddFluorescentGridVolume struct
 * Struct used to add a Fluorescent grid volume whose data is provided.
 * The dimensions of the grid must be provided aswell and they must match
 * in size the number of density elements. The volume will be enclosed
 * in a box
 */
struct AddFluorescentGridVolume
{
    COMMON_PARAMS
    COMMON_VOLUME_PROPS
    float g;
    floats fex;
    floats fem;
    float epsilon;
    float c;
    float yield;
    float gf;
    floats density;
    int nx;
    int ny;
    int nz;
};
bool from_json(AddFluorescentGridVolume& request, const std::string& payload);

/**
 * @brief The AddFluorescentScatteringVolume struct
 * Struct used to add a homogeneus fluorescence scattering volume.
 * The volume will be enclosed in a box.
 */
struct AddFluorescentScatteringVolume
{
    COMMON_PARAMS
    COMMON_VOLUME_PROPS
    floats absorption;
    floats scattering;
    floats fem;
    floats fex;
    float mweight;
    float epsilon;
    float c;
    float yield;
    float sscale;
    float fscale;
    float g;
    float gf;
    float density;
    floats Le;
};
bool from_json(AddFluorescentScatteringVolume& request, const std::string& payload);

/**
 * @brief The AddFluorescentScatteringGridFromFileVolume struct
 * Struct used to add a fluorescence scattering grid volume.
 * The volume will be enclosed in a box. The data will be loaded
 * from disk from the given path
 */
struct AddFluorescentScatteringGridFromFileVolume
{
    COMMON_PARAMS
    COMMON_VOLUME_PROPS
    floats absorption;
    floats scattering;
    floats fem;
    floats fex;
    float mweight;
    float epsilon;
    float c;
    float yield;
    float sscale;
    float fscale;
    float g;
    float gf;
    floats Le;
    std::string prefix;
};
bool from_json(AddFluorescentScatteringGridFromFileVolume& request, const std::string& payload);

/**
 * @brief The AddFluorescentScatteringGridFromFileVolume struct
 * Struct used to add a fluorescence scattering grid volume.
 * The volume will be enclosed in a box. The density is given
 * on a per cell basis. The grid dimensions are given as well,
 * and must match, in size, the number of density elements
 */
struct AddFluorescentScatteringGridVolume
{
    COMMON_PARAMS
    COMMON_VOLUME_PROPS
    floats absorption;
    floats scattering;
    floats fem;
    floats fex;
    float mweight;
    float epsilon;
    float c;
    float yield;
    float sscale;
    float fscale;
    float g;
    float gf;
    floats Le;
    floats density;
    int nx;
    int ny;
    int nz;

};
bool from_json(AddFluorescentScatteringGridVolume& request, const std::string& payload);

/**
 * @brief The RequestResponse struct
 * Struct to send back the result of a given request. The error field will hold
 * a numerical value. If it is different than 0, an error occoured and the message
 * field will carry further information.
 */
struct RequestResponse
{
    int error;
    std::string message;
};
std::string to_json(const RequestResponse& response);

#endif
