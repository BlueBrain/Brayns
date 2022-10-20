/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "Systems.h"

namespace brayns
{
void Systems::init(Components &components)
{
    if (_init)
    {
        _init->execute(components);
    }
}

CommitResult Systems::commit(Components &components)
{
    if (_commit)
    {
        return _commit->execute(components);
    }
    return {};
}

void Systems::preRender(const ParametersManager &parameters, Components &components)
{
    if (_preRender)
    {
        _preRender->execute(parameters, components);
    }
}

InspectResultData Systems::inspect(const InspectContext &context, Components &components)
{
    if (_inspect)
    {
        return _inspect->execute(context, components);
    }
    return InspectResultData();
}

Bounds Systems::computeBounds(const Matrix4f &matrix, Components &components)
{
    if (_bounds)
    {
        return _bounds->compute(matrix, components);
    }
    return {};
}
}
