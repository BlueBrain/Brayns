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

#include <brayns/common/types.h>

namespace
{
    const size_t DEFAULT_TRANSFER_FUNCTION_SAMPLE_SIZE = 512;
    const brayns::Vector2f DEFAULT_TRANSFER_FUNCTION_RANGE = { -92.0915, 49.5497 };
}

namespace brayns
{

class TransferFunction
{

public:

    TransferFunction();

    /**
     * @brief Clears the data held by the transfert fonction
     */
    void clear();

    /**
     * @brief Generates arrays of linear interpolated values according to control points, for each
     *        attribute of the transfer function. The red, green, blue and alpha attributes are
     *        stored in the form of an RGBA tupple of floats.
     * @param sampleSize Sample size defining the number of values that will be generated for each
     *        attribute.
    */
    void resample( size_t sampleSize = DEFAULT_TRANSFER_FUNCTION_SAMPLE_SIZE );

    /**
     * @brief Gets control points for a given attribute
     * @param attribute Attribute for which control points are requested
     * @return A vector of control points for the given attribute
     */
    Vector2fs& getControlPoints( TransferFunctionAttribute attribute );

    /**
     * @brief Get attribute as string
     * @param attribute Given attribute in the form of a TransferFunctionAttribute enum
     * @return String representation of the attribute. Undefined if the attribute does not exist
     */
    std::string getAttributeAsString( TransferFunctionAttribute attribute );

    /**
     * @brief Get attribute from string
     * @param attribute Given attribute in the form of a string
     * @return A TransferFunctionAttribute value correponding to the given string, TF_UNDEFINED if
     *         the string does not match any of the attributes
     */
    TransferFunctionAttribute getAttributeFromString( const std::string& attribute );

    /**
     * @brief Get diffuse color map generated by the resample function
     * @return A vector of RGBA values
     */
    Vector4fs& getDiffuseColors() { return _diffuseColors; }

    /**
     * @brief Get emission intensities generated by the resample function
     * @return A vector of floats
     */
    floats& getEmissionIntensities() { return _emissionIntensities; }

    /**
     * @brief Get transfer function range of values
     * @return A tuple of 2 floats with min and max value
     */
    const Vector2f& getValuesRange() const { return _valuesRange; }
    void setValuesRange( const Vector2f& valuesRange ) { _valuesRange = valuesRange; }

private:

    std::map< TransferFunctionAttribute, Vector2fs > _controlPoints;

    Vector4fs _diffuseColors;
    floats _emissionIntensities;
    Vector2f _valuesRange;
};

}

#endif // TRANSFERFUNCTION_H
