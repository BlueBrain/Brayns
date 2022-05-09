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

from dataclasses import dataclass

from brayns.core.material.material import Material


@dataclass
class GlassMaterial(Material):

    refraction_index: float = 1.5

    @classmethod
    @property
    def name(self) -> str:
        return 'glass'

    @classmethod
    def deserialize(cls, message: dict) -> 'GlassMaterial':
        return cls._from_dict(
            message,
            refraction_index=message['index_of_refraction']
        )

    def serialize(self) -> dict:
        return self._to_dict({
            'index_of_refraction': self.refraction_index
        })
