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

from typing import Any

from brayns.instance.instance import Instance
from brayns.plugins.color.color_method import ColorMethod


class MockCircuitInstance(Instance):

    def __init__(self) -> None:
        self.method = ''
        self.params = None
        self.not_colored = [1, 2, 3]
        self.methods = [method.value for method in ColorMethod]
        self.values = ['test1', 'test2']

    def request(self, method: str, params: Any = None) -> Any:
        self.method = method
        self.params = params
        if method == 'color-circuit-by-id':
            return self.not_colored
        if method == 'color-circuit-by-method':
            return None
        if method == 'color-circuit-by-single-color':
            return None
        if method == 'get-circuit-color-methods':
            return {'methods': self.methods}
        if method == 'get-circuit-color-method-variables':
            return {'variables': self.values}
        raise RuntimeError('Invalid request')
