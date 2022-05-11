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

from brayns.core.common.color import Color
from brayns.instance.instance import Instance
from brayns.plugins.common.color_method import ColorMethod


@dataclass
class CircuitColorByMethod:

    method: ColorMethod
    colors: dict[str, Color]

    @staticmethod
    def get_available_methods(instance: Instance, model_id: int) -> list[ColorMethod]:
        params = {'model_id': model_id}
        result = instance.request('get-circuit-color-methods', params)
        return [
            ColorMethod(method)
            for method in result['methods']
        ]

    @staticmethod
    def get_available_values(instance: Instance, model_id: int, method: ColorMethod) -> list[str]:
        params = {
            'model_id': model_id,
            'method': method.value
        }
        result = instance.request('get-circuit-color-method-variables', params)
        return result['variables']

    def apply(self, instance: Instance, model_id: int) -> None:
        params = {
            'model_id': model_id,
            'method': self.method.value,
            'color_info': [
                {
                    'variable': value,
                    'color': list(color)
                }
                for value, color in self.colors.items()
            ]
        }
        instance.request('color-circuit-by-method', params)
