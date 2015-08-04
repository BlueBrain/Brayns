/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <ospray/common/OSPCommon.h>

namespace brayns
{

struct Texture2D : public ospray::RefCount
{
  int   channels; //Number of color channels per pixel
  int   depth;    //Bytes per color channel
  int   width;    //Pixels per row
  int   height;   //Pixels per column
  void* data;   //Pointer to binary texture data
};

class TextureLoader
{
public:
    TextureLoader();

    Texture2D* loadTexture(
            const std::string &path,
            const std::string &fileNameBase);

    OSPTexture2D createTexture2D(Texture2D *msgTex);
};

}

#endif // TEXTURELOADER_H
