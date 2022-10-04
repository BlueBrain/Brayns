/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "NRRDReader.h"

#include "data/DataParser.h"
#include "header/HeaderParser.h"
#include "header/HeaderUtils.h"

#include <brayns/utils/FileReader.h>

#include <api/VoxelFactory.h>

NRRDReader::NRRDReader(const brayns::LoaderProgress &progressCallback)
    : _callback(progressCallback)
{
}

Atlas NRRDReader::read(const std::string &path, std::string_view data, VoxelType type)
{
    _callback.updateProgress("Parsing NRRD header", 0.2f);
    auto header = HeaderParser::parse(path, data);
    auto size = HeaderUtils::get3DSize(header);

    _callback.updateProgress("Parsing NRRD data", 0.4f);
    auto parsedData = DataParser::parse(header, data);
    auto voxelFactory = VoxelFactory::createDefault();
    auto voxels = voxelFactory.create(type, *parsedData);

    _callback.updateProgress("Transforming data", 0.6f);
    return Atlas(size, std::move(voxels));
}

Atlas NRRDReader::read(const std::string &path, VoxelType type)
{
    auto content = brayns::FileReader::read(path);
    return read(path, content, type);
}
