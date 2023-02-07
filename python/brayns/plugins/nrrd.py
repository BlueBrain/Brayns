# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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
from enum import Enum
from typing import Any, ClassVar

from brayns.core import Loader


class VoxelType(Enum):
    """Defines the representation of the voxels of an NRRD volume.

    :param SCALAR: Each voxel holds a single scalar value.
    :param ORIENTATION: Each voxel holds a quaternion (orientation field).
    :param FLATMAP: Each voxel is a pair of integer.
    """

    SCALAR = "scalar"
    ORIENTATION = "orientation"
    FLATMAP = "flatmap"


@dataclass
class NrrdLoader(Loader):
    """Loader for NRRD volumes.

    :param voxel_type: Defines the representation of the volume voxels.
    :type voxel_type: VoxelType
    """

    NRRD: ClassVar[str] = "nrrd"

    voxel_type: VoxelType

    @classmethod
    @property
    def name(cls) -> str:
        return "NRRD loader"

    def get_properties(self) -> dict[str, Any]:
        return {"type": self.voxel_type.value}
