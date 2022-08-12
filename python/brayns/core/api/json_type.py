# Copyright (c) 2015-2022 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# Responsible Author: adrien.fleury@epfl.ch
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 3.0 as published
# by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

from enum import Enum


class JsonType(Enum):
    """Enumeration of available JSON types.

    The value is the string from the JSON schema standard.

    :param UNDEFINED: Any type is allowed.
    :param NULL: No types are allowed.
    :param BOOLEAN: Boolean.
    :param INTEGER: Integral number.
    :param NUMBER: Any number.
    :param STRING: String.
    :param ARRAY: Array.
    :param OBJECT: Object.
    """

    UNDEFINED = 'undefined'
    NULL = 'null'
    BOOLEAN = 'boolean'
    INTEGER = 'integer'
    NUMBER = 'number'
    STRING = 'string'
    ARRAY = 'array'
    OBJECT = 'object'
