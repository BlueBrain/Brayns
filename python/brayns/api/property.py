# Copyright (c) 2021 EPFL/Blue Brain Project
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

from dataclasses import dataclass

from . import typename


@dataclass
class Property:

    typename: str
    name: str
    description: str
    required: bool = False
    read_only: bool = False

    @staticmethod
    def from_schema(name: str, schema: dict, required: bool = False):
        return Property(
            typename=typename.from_schema(schema),
            name=name,
            description=schema.get('description', ''),
            required=required,
            read_only=schema.get('readOnly', False)
        )
