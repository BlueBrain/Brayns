/* Copyright (c) 2015, Stefan Eilemann <eile@equalizergraphics.com>
*                      Daniel Nachbaur <danielnachbaur@gmail.com>
*                      Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License version 2.1 as published
* by the Free Software Foundation.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <GL/glew.h>
#include <iostream>

#include "shaders.h"

#undef glewGetContext
#define glewGetContext() glewContext

namespace brayns
{

bool compile( const unsigned shader, const char* source )
{
    glShaderSource( shader, 1, &source, 0 );
    glCompileShader( shader );
    GLint status;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
    if( !status )
    {
        GLchar errorLog[1024] = {0};
        glGetShaderInfoLog( shader, 1024, 0, errorLog );
        std::cout << "Failed to compile shader " << shader << ": " << errorLog
                  << std::endl;
        return false;
    }
    return true;
}

bool linkProgram( const unsigned program, const char* vertexShaderSource,
                  const char* fragmentShaderSource )
{
    if( !program || !vertexShaderSource || !fragmentShaderSource )
    {
        std::cout << "Failed to link shader program " << program << ": No valid "
                     "shader program, vertex or fragment source." << std::endl;
        return false;
    }

    const GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
    if( !compile( vertexShader, vertexShaderSource ))
    {
        glDeleteShader( vertexShader );
        return false;
    }

    const GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
    if( !compile( fragmentShader, fragmentShaderSource ))
    {
        glDeleteShader( fragmentShader );
        return false;
    }

    glAttachShader( program, vertexShader );
    glAttachShader( program, fragmentShader );
    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );

    glLinkProgram( program );
    GLint status;
    glGetProgramiv( program, GL_LINK_STATUS, &status );
    if( !status )
    {
        GLchar errorLog[1024] = {0};
        glGetProgramInfoLog( program, 1024, 0, errorLog );
        std::cout << "Failed to link shader program " << program << ": "
                  << errorLog << std::endl;
        return false;
    }
    return true;
}

}
