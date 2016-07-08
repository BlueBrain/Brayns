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

#ifndef COLORMAPLOADER_H
#define COLORMAPLOADER_H

#include <brayns/common/types.h>

namespace brayns
{

/** Loads color map from text file where every line contains a space separated
 *  list of integers for the following attributes:
 *  R: 0..255 value for the RGBA red component
 *  G: 0..255 value for the RGBA green component
 *  B: 0..255 value for the RGBA blue component
 *  A: 0..255 value for the RGBA alpha component defining the opacity of the
 *     surface. This component is optional and is set to 255 if not present
 */
class ColorMapLoader
{
public:
    ColorMapLoader();

    /**
     * @brief getValues Gets all the values for the loaded colormap
     * @return A vector of Vector4f containing the RGBA values for the colormap
     */
    const Vector4uis& getValues() const;

    /**
     * @brief setValues Sets or replaces all colormap values
     * @param values A vector of Vector4ui containing the RGBA values for the
     *        color map
     */
    void setValues(const Vector4uis& values);

    /**
     * @brief Loads values from a colormap file
     * @param filename Full file name of the colormap file
     * @return true if the colormap file was successfully loaded, false
     *         otherwise
     */
    bool loadFromFile( const std::string& filename );

    /**
     * @brief Assigns the current colormap to the MATERIAL_SIMULATION_COLORMAP
     *        system material. Values are stored in a diffuse texture.
     * @param scene Scene handling the materials
     */
    void assignColorMapToTexture( ScenePtr& scene );

private:

    Vector4uis _values;

};

}

#endif // COLORMAPLOADER_H
