/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#ifndef SCENEPARAMETERS_H
#define SCENEPARAMETERS_H

#include "AbstractParameters.h"

SERIALIZATION_ACCESS(SceneParameters)

namespace brayns
{
class SceneParameters final : public AbstractParameters
{
public:
    SceneParameters();

    /** @copydoc AbstractParameters::print */
    void print() final;

    const std::string& getColorMapFilename() const { return _colorMapFilename; }
    const Vector2f& getColorMapRange() const { return _colorMapRange; }
    /**
       file name of the environment map
    */
    const std::string& getEnvironmentMap() const { return _environmentMap; }
protected:
    void parse(const po::variables_map& vm) final;

    std::string _colorMapFilename;
    Vector2f _colorMapRange{0.f, 255.f};
    std::string _environmentMap;

    SERIALIZATION_FRIEND(SceneParameters)
};
}
#endif // SCENEPARAMETERS_H
