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

#include "TransferFunction.h"

#include <brayns/common/log.h>

namespace brayns
{

TransferFunction::TransferFunction()
{
    _diffuseColors.resize( 256, { 1, 0, 0, 0.5 } );
    _emissionIntensities.resize( 256, 1 );
    _contributions.resize( 256, 1 );
    _valuesRange = { 0, 255.f };
}

void TransferFunction::clear()
{
    _diffuseColors.clear();
    _emissionIntensities.clear();
    _contributions.clear();
    _valuesRange = Vector2f();
}

}
