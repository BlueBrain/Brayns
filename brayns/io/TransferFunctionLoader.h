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

#ifndef TRANSFERFUNCTIONLOADER_H
#define TRANSFERFUNCTIONLOADER_H

#include <brayns/common/types.h>

namespace brayns
{
/** Loads transfer function from text file where every line contains a space
 * separated
 *  list of integers for the following attributes:
 *  red: 0..1 value for the RGBA red component
 *  green: 0..1 value for the RGBA green component
 *  blue: 0..1 value for the RGBA blue component
 *  alpha: 0..1 value for the RGBA alpha component defining the opacity of the
 *     surface. This component is optional and is set to DEFAULT_ALPHA if not
 * present
 *  emission: 0..1 value for the light emission component defining the intensity
 * of light
 *     emitted by the surface. This component is optional and is set to
 * DEFAULT_EMISSION if
 *     not present
 */
class TransferFunctionLoader
{
public:
    TransferFunctionLoader();

    /**
     * @brief Loads values from a transfer function file
     * @param filename Full file name of the transfer function file
     * @param range Range of values to which the transfer function is applied
     * @param scene Scene holding the transfer function
     * @return True if the transfer function file was successfully loaded, false
     *         otherwise
     */
    bool loadFromFile(const std::string& filename, const Vector2f& range,
                      Scene& scene);

private:
    Vector2f _range;
};
}

#endif // TRANSFERFUNCTIONLOADER_H
