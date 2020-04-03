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

#ifndef GEOMETRYPARAMETERS_H
#define GEOMETRYPARAMETERS_H

#include "AbstractParameters.h"

#include <brayns/common/types.h>

SERIALIZATION_ACCESS(GeometryParameters)

namespace brayns
{
/** Manages geometry parameters
 */
class GeometryParameters : public AbstractParameters
{
public:
    /**
       Parse the command line parameters and populates according class members
     */
    GeometryParameters();

    /** @copydoc AbstractParameters::print */
    void print() final;

    ProteinColorScheme getColorScheme() const { return _colorScheme; }
    GeometryQuality getGeometryQuality() const { return _geometryQuality; }
    float getRadiusMultiplier() const { return _radiusMultiplier; }
    /**
     * Defines what memory mode should be used between Brayns and the
     * underlying renderer
     */
    MemoryMode getMemoryMode() const { return _memoryMode; };
    const std::set<BVHFlag>& getDefaultBVHFlags() const
    {
        return _defaultBVHFlags;
    }

protected:
    void parse(const po::variables_map& vm) final;

    // Scene
    std::set<BVHFlag> _defaultBVHFlags;

    // Geometry
    ProteinColorScheme _colorScheme{ProteinColorScheme::none};
    GeometryQuality _geometryQuality{GeometryQuality::high};
    float _radiusMultiplier{1};

    // System parameters
    MemoryMode _memoryMode{MemoryMode::shared};

    SERIALIZATION_FRIEND(GeometryParameters)
};
} // namespace brayns
#endif // GEOMETRYPARAMETERS_H
