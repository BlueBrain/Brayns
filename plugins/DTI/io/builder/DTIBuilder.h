/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "IDTIBuilder.h"
#include "common/StreamlineData.h"

#include <map>
namespace dti
{
class DTIBuilder final : public IDTIBuilder
{
public:
    void reset() override;
    void readGidRowFile(const std::string &path) override;
    void readStreamlinesFile(const std::string &path) override;
    void buildGeometry(float radius, brayns::Model &model) override;
    void buildSimulation(const std::string &path, float spikeDecayTime, brayns::Model &model) override;

private:
    std::map<uint64_t, StreamlineData> _streamlines;
};
}
