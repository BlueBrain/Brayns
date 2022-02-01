/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 *
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#include "BaseObject.h"

namespace brayns
{
BaseObject::BaseObject(const BaseObject &)
    : _modified(true)
{
}

BaseObject &BaseObject::operator=(const BaseObject &rhs)
{
    if (this == &rhs)
        return *this;

    _modified = true;
    return *this;
}

bool BaseObject::isModified() const
{
    return _modified;
}

void BaseObject::resetModified()
{
    _modified = false;
}

void BaseObject::markModified(const bool triggerCallback)
{
    _modified = true;
    if (_modifiedCallback && triggerCallback)
        _modifiedCallback(*this);
}

void BaseObject::onModified(const ModifiedCallback &callback)
{
    _modifiedCallback = callback;
}

void BaseObject::clearModifiedCallback()
{
    _modifiedCallback = ModifiedCallback();
}
} // namespace brayns
