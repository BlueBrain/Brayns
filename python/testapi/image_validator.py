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

import pathlib
from dataclasses import dataclass


@dataclass
class ImageValidator:

    threshold: float = 1.0
    erase: bool = True

    def validate_file(self, test: pathlib.Path, ref: pathlib.Path) -> None:
        with test.open('rb') as file:
            data = file.read()
        if self.erase:
            test.unlink()
        self.validate_data(data, ref)

    def validate_data(self, test: bytes, ref: pathlib.Path) -> None:
        with ref.open('rb') as file:
            data = file.read()
        self.validate(test, data)

    def validate(self, test: bytes, ref: bytes) -> None:
        mse = self._get_mse(test, ref)
        if mse > self.threshold:
            raise RuntimeError(f'Image difference {mse} > {self.threshold}')

    def _get_mse(self, data1: bytes, data2: bytes) -> int:
        return sum(
            (i - j) ** 2
            for i, j in zip(data1, data2)
        ) / len(data1) / len(data2)
