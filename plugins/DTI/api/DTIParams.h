/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#ifndef DTIPARAMS_H
#define DTIPARAMS_H

#include "../io/DTITypes.h"
#include <brayns/common/types.h>

/**
 * @brief The Streamlines struct handles a set of streamlines. Indices are used
 * to specify the first point of each streamline
 */
struct StreamlinesDescriptor
{
    std::string name;
    Gids gids;
    Indices indices;
    Vertices vertices;
    float radius{1.f};
    float opacity{1.f};
};

bool from_json(StreamlinesDescriptor &param, const std::string &payload);

struct SpikeSimulationDescriptor
{
    size_t modelId;
    Gids gids;
    std::vector<float> timestamps;
    float dt{0.1f};
    float endTime{1.f};
    float timeScale{1.f};
    float decaySpeed{0.1f};
    float restIntensity{0.25f};
    float spikeIntensity{0.75f};
};

bool from_json(SpikeSimulationDescriptor &param, const std::string &payload);

// DB import
struct ImportStreamlines
{
    std::string connectionString;
    std::string filename;
};
bool from_json(ImportStreamlines &param, const std::string &payload);

// Streamlines
struct LoadStreamlines
{
    std::string connectionString;
    std::string sqlStatement;
    std::string name;
    float radius{1.f};
    size_t colorScheme{0};
    size_t nbMaxPoints{0};
};
bool from_json(LoadStreamlines &param, const std::string &payload);

#endif // DTIPARAMS_H
