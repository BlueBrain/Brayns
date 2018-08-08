/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <brayns/common/log.h>
#include <brayns/common/material/Texture2D.h>
#include <brayns/common/scene/Scene.h>

#include <fstream>
#include <sstream>

#include "TransferFunctionLoader.h"

namespace
{
const float DEFAULT_ALPHA = 1.f;
}

namespace brayns
{
bool loadTransferFunctionFromFile(const std::string& filename,
                                  const Vector2f& range,
                                  TransferFunction& transferFunction)
{
    BRAYNS_INFO << "Loading transfer function color map from " << filename
                << std::endl;
    std::ifstream file(filename, std::ios::in);
    if (!file.good())
    {
        BRAYNS_ERROR << "Could not open file " << filename << std::endl;
        return false;
    }

    bool validParsing = true;
    std::string line;

    transferFunction.clear();

    size_t nbEntries = 0;

    bool firstLine = true;

    Vector4fs diffuseColors;

    while (validParsing && std::getline(file, line))
    {
        std::vector<double> lineData;
        std::stringstream lineStream(line);

        double value;
        while (lineStream >> value)
            lineData.push_back(value);

        switch (lineData.size())
        {
        case 1:
        {
            // Special case where some files store the number of entries on the
            // first line
            if (!firstLine)
                validParsing = false;
            break;
        }
        case 3:
        {
            diffuseColors.emplace_back(lineData[0], lineData[1], lineData[2],
                                       DEFAULT_ALPHA);
            break;
        }
        case 4:
        {
            diffuseColors.emplace_back(lineData[0], lineData[1], lineData[2],
                                       lineData[3]);
            break;
        }
        default:
            validParsing = false;
            break;
        }

        if (!validParsing)
            BRAYNS_ERROR << "Invalid line: " << line << std::endl;

        if (!firstLine)
        {
            bool rangeOK = true;
            for (auto v : lineData)
                if (v < 0.0 || v > 1.0)
                    rangeOK = false;

            if (!rangeOK)
            {
                BRAYNS_ERROR << "Number not in range 0..1 on line: " << line
                             << std::endl;
                validParsing = false;
            }
        }

        ++nbEntries;
        firstLine = false;
    }

    transferFunction.getDiffuseColors() = diffuseColors;

    transferFunction.setValuesRange(range);
    BRAYNS_INFO << "Transfer function values range: " << range << std::endl;
    file.close();
    return validParsing;
}
}
