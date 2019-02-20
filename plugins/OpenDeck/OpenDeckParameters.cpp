/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "OpenDeckParameters.h"

namespace brayns
{
OpenDeckParameters::OpenDeckParameters()
    : _props("OpenDeck plugin parameters")
{
    _props.setProperty(
        {PARAM_RESOLUTION_SCALING, 1.0,
         Property::MetaData{"OpenDeck native resolution scale",
                            "OpenDeck native resolution scale"}});
    _props.setProperty({PARAM_CAMERA_SCALING, 1.0,
                        Property::MetaData{"OpenDeck camera scaling",
                                           "OpenDeck camera scaling"}});
}
}
