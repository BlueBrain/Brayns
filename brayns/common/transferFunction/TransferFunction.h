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

#ifndef TRANSFERFUNCTION_H
#define TRANSFERFUNCTION_H

#include <brayns/common/BaseObject.h>
#include <brayns/common/types.h>

namespace brayns
{
class TransferFunction : public BaseObject
{
public:
    TransferFunction();

    /**
     * @brief Clears the data held by the transfert fonction
     */
    void clear();

    /**
     * @brief Get diffuse colors
     * @return A vector of RGB floats
     */
    Vector4fs& getDiffuseColors() { return _diffuseColors; }
    /**
     * @brief Get emission color intensities
     * @return A vector of floats
     */
    Vector3fs& getEmissionIntensities() { return _emissionIntensities; }
    /**
     * @brief Get contributions
     * @return A vector of floats
     */
    floats& getContributions() { return _contributions; }
    /**
     * @brief Get transfer function range of values
     * @return A tuple of 2 floats with min and max value
     */
    Vector2f& getValuesRange() { return _valuesRange; }
    void setValuesRange(const Vector2f& valuesRange)
    {
        _updateValue(_valuesRange, valuesRange);
    }

private:
    Vector4fs _diffuseColors;
    Vector3fs _emissionIntensities;
    floats _contributions;
    Vector2f _valuesRange;
};
}

#endif // TRANSFERFUNCTION_H
