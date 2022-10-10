# Copyright (c) 2015-2022 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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
from typing import Any, ClassVar

from brayns.core import Loader

from .protein_color_scheme import ProteinColorScheme


@dataclass
class ProteinLoader(Loader):

    PDB: ClassVar[str] = 'pdb'
    PDB1: ClassVar[str] = 'pdb1'

    color_scheme: ProteinColorScheme = ProteinColorScheme.NONE
    radius_multiplier: float = 1.0

    @classmethod
    @property
    def name(cls) -> str:
        return 'protein'

    def get_properties(self) -> dict[str, Any]:
        return {
            'color_scheme': self.color_scheme.value,
            'radius_multiplier': self.radius_multiplier,
        }
