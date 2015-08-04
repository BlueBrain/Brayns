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

// OSPRay
#include <ospray/texture/Texture2D.h>

// embree
#include "common/sys/filename.h"

#include <brayns/common/log.h>
#include <map>
#include "TextureLoader.h"

namespace brayns
{

using namespace ospray;

TextureLoader::TextureLoader()
{
}

Texture2D* TextureLoader::loadTexture(const std::string &path, const std::string &fileNameBase)
{
    const embree::FileName fileName = path+"/"+fileNameBase;

    BRAYNS_INFO << "Loading texture from " << fileName << std::endl;

    static std::map<std::string,Texture2D*> textureCache;
    if (textureCache.find(fileName.str()) != textureCache.end())
        return textureCache[fileName.str()];

    Texture2D *tex = NULL;
    const std::string ext = fileName.ext();
    if (ext == "ppm")
    {
        try
        {
            int rc, peekchar;

            // open file
            FILE *file = fopen(fileName.str().c_str(),"r");
            const int LINESZ=10000;
            char lineBuf[LINESZ+1];

            if (!file)
                throw std::runtime_error("#osp:miniSG: could not open texture file '"+fileName.str()+"'.");

            // read format specifier:
            int format=0;
            if( !fscanf(file,"P%i\n",&format) )
            {
                if (format != 6)
                    throw std::runtime_error("#osp:miniSG: can currently load only binary P6 subformats for PPM texture files. "
                                             "Please report this bug at ospray.github.io.");
            }
            else
            {
                throw std::runtime_error("Failed to read data from file");
            }

            // skip all comment lines
            peekchar = getc(file);
            while (peekchar == '#')
            {
                if( !fgets(lineBuf,LINESZ,file) )
                    peekchar = getc(file);
                else
                    throw std::runtime_error("Failed to read data from file");
            }
            ungetc(peekchar,file);

            // read width and height from first non-comment line
            int width=-1,height=-1;
            rc = fscanf(file,"%i %i\n",&width,&height);
            if (rc != 2)
                throw std::runtime_error("#osp:miniSG: could not parse width and height in P6 PPM file '"+fileName.str()+"'. "
                                         "Please report this bug at ospray.github.io, and include named file to reproduce the error.");

            // skip all comment lines
            peekchar = getc(file);
            while (peekchar == '#')
            {
                if( !fgets(lineBuf,LINESZ,file) )
                    peekchar = getc(file);
                else
                    throw std::runtime_error("Failed to read data from file");
            }
            ungetc(peekchar,file);

            // read maxval
            int maxVal=-1;
            rc = fscanf(file,"%i",&maxVal);
            peekchar = getc(file);

            if (rc != 1)
                throw std::runtime_error("#osp:miniSG: could not parse maxval in P6 PPM file '"+fileName.str()+"'. "
                                         "Please report this bug at ospray.github.io, and include named file to reproduce the error.");
            if (maxVal != 255)
                throw std::runtime_error("#osp:miniSG: could not parse P6 PPM file '"+fileName.str()+"': currently supporting only maxVal=255 formats."
                                         "Please report this bug at ospray.github.io, and include named file to reproduce the error.");

            tex = new Texture2D;
            tex->width    = width;
            tex->height   = height;
            tex->data     = new unsigned char[width*height*3];
            if( !fread(tex->data,width*height*3,1,file) )
                throw std::runtime_error("Failed to read data from file");
        }
        catch(std::runtime_error e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
    else
    {
#ifdef BRAYNS_USE_IMAGEMAGICK
        Magick::Image image(fileName.str().c_str());
        // Image* out = new Image4c(image.columns(),image.rows(),fileName);
        tex = new Texture2D;
        tex->width    = image.columns();
        tex->height   = image.rows();
        tex->channels = 4;
        tex->depth    = 4;
        float rcpMaxRGB = 1.0f/float(MaxRGB);
        Magick::Pixels pixel_cache(image);
        Magick::PixelPacket* pixels = pixel_cache.get(0,0,tex->width,tex->height);
        if (!pixels)
        {
            std::cerr << "#osp:minisg: failed to load texture '"+fileName.str()+"'" << std::endl;
            delete tex;
            tex = NULL;
        }
        else
        {
            tex->data = new vec4f[tex->width*tex->height];
            for (size_t y=0; y<tex->height; y++)
            {
                for (size_t x=0; x<tex->width; x++)
                {
                    vec4f c;
                    c.x = float(pixels[y*tex->width+x].red    )*rcpMaxRGB;
                    c.y = float(pixels[y*tex->width+x].green  )*rcpMaxRGB;
                    c.z = float(pixels[y*tex->width+x].blue   )*rcpMaxRGB;
                    c.w = float(pixels[y*tex->width+x].opacity)*rcpMaxRGB;
                    ((vec4f*)tex->data)[x+y*tex->width] = c;
                    //tex->set(x,y,c);
                }
            }
        }
#endif
    }
    BRAYNS_INFO << "Texture " << fileName << " successfully loaded into cache" << std::endl;
    textureCache[fileName.str()] = tex;
    return tex;
}

OSPTexture2D TextureLoader::createTexture2D(Texture2D *msgTex)
{
    if(msgTex == NULL)
    {
      static int numWarnings = 0;
      if (++numWarnings < 10)
        BRAYNS_INFO << "WARNING: material does not have Textures (only warning for the first 10 " \
                       "times)!" << std::endl;
      return NULL;
    }

    static std::map<Texture2D*, OSPTexture2D> alreadyCreatedTextures;
    if (alreadyCreatedTextures.find(msgTex) != alreadyCreatedTextures.end())
      return alreadyCreatedTextures[msgTex];

    //TODO: We need to come up with a better way to handle different possible pixel layouts
    OSPDataType type = OSP_VOID_PTR;

    if (msgTex->depth == 1)
    {
      if( msgTex->channels == 3 ) type = OSP_UCHAR3;
      if( msgTex->channels == 4 ) type = OSP_UCHAR4;
    } else if (msgTex->depth == 4)
    {
      if( msgTex->channels == 3 ) type = OSP_FLOAT3;
      if( msgTex->channels == 4 ) type = OSP_FLOAT3A;
    }

    OSPTexture2D ospTex = ospNewTexture2D( msgTex->width,
                                           msgTex->height,
                                           type,
                                           msgTex->data,
                                           0);

    alreadyCreatedTextures[msgTex] = ospTex;

    ospCommit(ospTex);
    return ospTex;
}

}
