/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "StreamlineData.h"

#include <brayns/utils/FileReader.h>

#include <map>

namespace dti
{
/**
 * @brief Read the streamline geometry file by line, ignore those lines for which the RowFilterCallback returns false
 */
class RowStreamlineMapReader
{
public:
    template<typename RowFilterCallback>
    static std::map<uint64_t, StreamlineData> read(const std::string &path, const RowFilterCallback &filter)
    {
        std::map<uint64_t, StreamlineData> result;

        const auto content = brayns::FileReader::read(path);
        std::istringstream stream(content);

        size_t row = 0;
        size_t index = 0;

        while (stream.good())
        {
            std::string line;
            std::getline(stream, line);

            if (!line.empty() && filter(row))
            {
                std::istringstream lineStream(line);
                uint64_t nbPoints;
                lineStream >> nbPoints;

                if (!lineStream.good() || nbPoints == 0)
                {
                    throw std::runtime_error("Row " + std::to_string(row));
                }

                auto &streamline = result[row];

                streamline.linealIndex = index++;
                streamline.points = _readPoints(lineStream, nbPoints);
            }

            ++row;
        }

        return result;
    }

private:
    static std::vector<brayns::Vector3f> _readPoints(std::istringstream &lineStream, size_t nbPoints)
    {
        std::vector<brayns::Vector3f> points;
        points.reserve(nbPoints);

        for (uint64_t i = 0; i < nbPoints && lineStream.good(); ++i)
        {
            brayns::Vector3f point;
            lineStream >> point.x >> point.y >> point.z;
            points.push_back(point);
        }

        return points;
    }
};
}
