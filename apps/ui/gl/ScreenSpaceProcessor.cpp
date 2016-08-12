/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <vector>

#include <apps/ui/gl/shaders/vertQuad.glsl.h>
#include <apps/ui/gl/shaders/fragQuad.glsl.h>

#include <brayns/common/log.h>

#include "ScreenSpaceProcessor.h"

namespace brayns
{

ScreenSpaceProcessor::ScreenSpaceProcessor()
{}

void ScreenSpaceProcessor::init( uint32_t width, uint32_t height )
{
    glewInit();

    quadVertices_[ 0 ] = -1.0f; quadVertices_[ 1 ] = -1.0f; quadVertices_[ 2 ] = 0.0f;
    quadVertices_[ 3 ] = -1.0f; quadVertices_[ 4 ] = 1.0f; quadVertices_[ 5 ] = 0.0f;
    quadVertices_[ 6 ] = 1.0f; quadVertices_[ 7 ] = 1.0f; quadVertices_[ 8 ] = 0.0f;
    quadVertices_[ 9 ] = 1.0f; quadVertices_[ 10 ] = -1.0f; quadVertices_[ 11 ] = 0.0f;

    quadIndices_[ 0 ] = 0; quadIndices_[ 1 ] = 1; quadIndices_[ 2 ] = 2;
    quadIndices_[ 3 ] = 0; quadIndices_[ 4 ] = 2; quadIndices_[ 5 ] = 3;

    glGenBuffers( 1, &quadVertexeId_);
    glGenBuffers( 1, &quadIndexId_);

    glBindBuffer( GL_ARRAY_BUFFER, quadVertexeId_ );
    glBufferData( GL_ARRAY_BUFFER, sizeof(float) * 12, &quadVertices_[0], GL_STATIC_DRAW );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndexId_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6, &quadIndices_[0], GL_STATIC_DRAW );

    quadShader_ = glCreateProgram();
    if( !linkProgram( quadShader_, vertQuad_glsl, fragQuad_glsl ))
        BRAYNS_ERROR << "Error: cannot compile shader" << std::endl;

    createColorTexture_( width, height );
    createDepthTexture_( width, height );
}

void ScreenSpaceProcessor::resize( uint32_t width, uint32_t height )
{
    glViewport(0,0,width,height);
    glBindTexture( GL_TEXTURE_2D, colorTexture_ );

    std::vector< uint8_t > emptyData( width * height * 4u, 0u );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, &emptyData[0]);

    glBindTexture( GL_TEXTURE_2D, depthTexture_ );

    std::vector< float > emptyDataFloat( width * height, 0u );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0,
                  GL_LUMINANCE, GL_FLOAT, &emptyDataFloat[0]);

    glBindTexture( GL_TEXTURE_2D,0 );
}

void ScreenSpaceProcessor::clear()
{
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ScreenSpaceProcessor::draw( const ScreenSpaceProcessorData& buffersData )
{
    glUseProgram( quadShader_ );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, colorTexture_ );
    glUniform1i(glGetUniformLocation( quadShader_, "colorBuffer"), 0);

    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, buffersData.width, buffersData.height,
                     buffersData.colorFormat, buffersData.colorType, buffersData.colorBuffer );

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, depthTexture_ );
    glUniform1i(glGetUniformLocation( quadShader_, "depthBuffer"), 1);

    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, buffersData.width, buffersData.height,
                     buffersData.depthFormat, buffersData.depthType, buffersData.depthBuffer );

    renderTexturedQuad_();

    glUseProgram( 0 );
    glBindTexture( GL_TEXTURE_2D,0 );
}

void ScreenSpaceProcessor::renderTexturedQuad_()
{
    glEnableVertexAttribArray( 0 );

    glBindBuffer(GL_ARRAY_BUFFER, quadVertexeId_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndexId_ );

    glDrawElements(GL_TRIANGLES, 6 , GL_UNSIGNED_INT, 0 );

    glDisableVertexAttribArray( 0 );
}

void ScreenSpaceProcessor::createColorTexture_( uint32_t width, uint32_t height )
{
    glEnable( GL_TEXTURE_2D );
    glGenTextures( 1, &colorTexture_ );
    glBindTexture( GL_TEXTURE_2D, colorTexture_ );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    std::vector<uint8_t> emptyData( width * height * 4u, 0u );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, &emptyData[0]);

    glBindTexture( GL_TEXTURE_2D,0 );
}

void ScreenSpaceProcessor::createDepthTexture_( uint32_t width, uint32_t height )
{
    glEnable( GL_TEXTURE_2D );
    glGenTextures( 1, &depthTexture_ );
    glBindTexture( GL_TEXTURE_2D, depthTexture_ );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    std::vector<float> emptyData( width * height, 0.0f );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0,
                  GL_LUMINANCE, GL_FLOAT, &emptyData[0]); //Maybe GL_DEPTH_COMPONENT.

    glBindTexture( GL_TEXTURE_2D,0 );
}

}
