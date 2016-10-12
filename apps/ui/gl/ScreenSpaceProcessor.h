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

#ifndef SCREENSPACEPROCESSOR_H
#define SCREENSPACEPROCESSOR_H

#include <GL/glew.h>

#include <apps/ui/gl/shaders.h>

namespace brayns
{

struct ScreenSpaceProcessorData
{
    uint32_t width;
    uint32_t height;

    GLenum colorFormat;
    GLenum colorType;
    GLenum depthFormat;
    GLenum depthType;
    GLvoid* colorBuffer;
    GLvoid* depthBuffer;
};

class ScreenSpaceProcessor
{
public:
    ScreenSpaceProcessor();

    void init( uint32_t width, uint32_t height );

    void clear();
    void resize( uint32_t width, uint32_t height );
    void draw( const ScreenSpaceProcessorData& buffersData );

private:

   void renderTexturedQuad_();
   void createColorTexture_( uint32_t width, uint32_t height );
   void createDepthTexture_( uint32_t width, uint32_t height );

   //ScreenSpaceProcessorData buffersData_;

   //textures
   GLuint colorTexture_;
   GLuint depthTexture_;
   //GLuint normalTexture_;
   //GLuint lightTexture_;

   //Quad
   GLfloat quadVertices_[ 12 ];
   GLint quadIndices_[ 6 ];
   GLuint quadVertexeId_;
   GLuint quadIndexId_;

   //Shaders
   GLuint quadShader_;
};

}

#endif //SCREENSPACEPROCESSOR_H
