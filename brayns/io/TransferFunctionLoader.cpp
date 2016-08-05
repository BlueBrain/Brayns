/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#include "TransferFunctionLoader.h"

namespace
{
const float DEFAULT_ALPHA = 1.f;
const float DEFAULT_EMISSION = 0.f;
}

namespace brayns
{

TransferFunctionLoader::TransferFunctionLoader()
{
}

bool TransferFunctionLoader::loadFromFile(
    const std::string& filename,
    Scene& scene )
{
    BRAYNS_INFO << "Loading color map from " << filename << std::endl;
    std::ifstream file( filename, std::ios::in );
    if( !file.good( ))
    {
        BRAYNS_ERROR << "Could not open file " << filename << std::endl;
        return false;
    }

    bool validParsing = true;
    std::string line;

    TransferFunction& transferFunction = scene.getTransferFunction();
    transferFunction.clear();

    while( validParsing && std::getline( file, line ))
    {
        std::vector< uint > lineData;
        std::stringstream lineStream(line);

        size_t value;
        while( lineStream >> value )
            lineData.push_back(value);

        switch( lineData.size() )
        {
        case 3:
        {
            Vector4f diffuse(
                lineData[0] / 255.f,
                lineData[1] / 255.f,
                lineData[2] / 255.f,
                DEFAULT_ALPHA );
            transferFunction.getDiffuseColors().push_back( diffuse );
            transferFunction.getEmissionIntensities().push_back( DEFAULT_EMISSION );
            break;
        }
        case 4:
        {
            Vector4f diffuse(
                lineData[0] / 255.f,
                lineData[1] / 255.f,
                lineData[2] / 255.f,
                lineData[3] / 255.f );
            transferFunction.getDiffuseColors().push_back( diffuse );
            transferFunction.getEmissionIntensities().push_back( DEFAULT_EMISSION );
            break;
        }
        case 5:
        {
            Vector4f diffuse(
                lineData[0] / 255.f,
                lineData[1] / 255.f,
                lineData[2] / 255.f,
                lineData[3] / 255.f );
            transferFunction.getDiffuseColors().push_back( diffuse );
            transferFunction.getEmissionIntensities().push_back( lineData[3] / 255.f );
            break;
        }
        default:
            BRAYNS_ERROR << "Invalid line: " << line << std::endl;
            validParsing = false;
            break;
        }
    }

    file.close();
    return validParsing;
}

}
