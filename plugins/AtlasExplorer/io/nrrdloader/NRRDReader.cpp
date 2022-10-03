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

namespace
{
class AtlasTypeChecker
{
public:
    static void check(const NRRDHeader &header, AtlasType type)
    {
        switch (type)
        {
        case AtlasType::flatmap:
        {
            if (header.dimensions < 4)
            {
                throw std::invalid_argument("Flatmaps must be 4-dimensional");
            }
            if (header.sizes[0] != 2)
            {
                throw std::invalid_argument("Flatmaps must have voxels with 2 elements");
            }
            break;
        case AtlasType::orientation:
        {
            if (header.dimensions < 4)
            {
                throw std::invalid_argument("Orientation volumes must be 4-dimensional");
            }
            if (header.sizes[0] != 4)
            {
                throw std::invalid_argument("Orientation volumes must have voxels with 4 elements");
            }
            break;
        }
        default:
        }
        }
    }
};

class AtlasDataBuilder
{
public:
    static AtlasData build(const NRRDHeader &header, std::unique_ptr<IDataMangler> parsedData, AtlasType type)
    {
        AtlasData atlas;
        atlas.type = type;
        atlas.voxelSize = HeaderUtils::getVoxelDimension(header);
        atlas.size = HeaderUtils::get3DSize(header);
        atlas.spacing = HeaderUtils::get3DDimensions(header);
        atlas.data = std::move(parsedData);
        return atlas;
    }
};
}

AtlasData NRRDReader::read(
    const std::string &path,
    std::string_view data,
    const brayns::LoaderProgress &callback,
    AtlasType type)
{
    callback.updateProgress("Parsing NRRD header", 0.2f);
    auto header = HeaderParser::parse(path, data);
    AtlasTypeChecker::check(header, type);

    callback.updateProgress("Parsing NRRD data", 0.4f);
    auto parsedData = DataParser::parse(header, data);

    callback.updateProgress("Transforming data", 0.6f);
    return AtlasDataBuilder::build(header, std::move(parsedData), type);
}

AtlasData NRRDReader::read(const std::string &path, AtlasType type)
{
    auto content = brayns::FileReader::read(path);
    auto data = std::string_view(content);

    auto header = HeaderParser::parse(path, data);
    AtlasTypeChecker::check(header, type);
    auto parsedData = DataParser::parse(header, data);
    return AtlasDataBuilder::build(header, std::move(parsedData), type);
}
