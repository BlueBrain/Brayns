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
    GLenum colorFormat;
    GLenum depthFormat;
    GLvoid* colorBuffer;
    GLvoid* depthBuffer;
};

class ScreenSpaceProcessor
{
public:
    ScreenSpaceProcessor();

    void draw( const ScreenSpaceProcessorData& buffersData );

private:
   ScreenSpaceProcessorData buffersData_;
};

}


#endif //SCREENSPACEPROCESSOR_H
