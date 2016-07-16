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

#include "ColorMapLoader.h"

namespace brayns
{

const uint DEFAULT_ALPHA = 255;

ColorMapLoader::ColorMapLoader()
{
}

const Vector4uis& ColorMapLoader::getValues() const
{
    return _values;
}

void ColorMapLoader::setValues( const Vector4uis& values )
{
    _values.clear();
    for( auto value: values )
        _values.push_back( value );
}

bool ColorMapLoader::loadFromFile( const std::string& filename )
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
            _values.push_back( Vector4ui(
                lineData[0], lineData[1], lineData[2], DEFAULT_ALPHA ));
            break;
        case 4:
            _values.push_back( Vector4ui(
                lineData[0], lineData[1], lineData[2], lineData[3] ));
            break;
        default:
            BRAYNS_ERROR << "Invalid line: " << line << std::endl;
            validParsing = false;
            break;
        }
    }

    file.close();
    return validParsing;
}

void ColorMapLoader::assignColorMapToTexture( ScenePtr& scene )
{
    const uint64_t nbChannels = 4;
    const uint64_t width = _values.size();
    Texture2DPtr texture( new Texture2D );
    texture->setType( TT_DIFFUSE );
    texture->setNbChannels( nbChannels );
    texture->setDepth( 1 );
    texture->setHeight( 1 );
    texture->setWidth( width );

    const size_t totalSize = width * nbChannels;

    std::vector< unsigned char > data;
    data.reserve( totalSize );
    for( const auto& value: _values )
    {
        data.push_back( value.x() );
        data.push_back( value.y() );
        data.push_back( value.z() );
        data.push_back( value.w() );
    }

    texture->setRawData( data.data(), totalSize );
    scene->getTextures()[TEXTURE_NAME_SIMULATION_COLOR_MAP] = texture;
}


}
