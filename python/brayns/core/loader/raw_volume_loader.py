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
from typing import Any

from brayns.utils import Vector3

from .loader import Loader
from .volume_data_type import VolumeDataType


@dataclass
class RawVolumeLoader(Loader):
    """Raw volume loader.

    Supports .raw file format.

    :param dimensions: Volume 3D grid dimensions.
    :type dimensions: Vector3.
    :param spacing: World-space dimensions of a voxel.
    :type size: Vector3.
    :param data_type: Type of data to interpret the volume bytes.
    :type data_type: str.
    """

    dimensions: Vector3
    spacing: Vector3
    data_type: str

    @classmethod
    @property
    def name(cls) -> str:
        """Get the loader name.

        :return: Loader name.
        :rtype: str
        """
        return "raw-volume"

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "dimensions": list(self.dimensions),
            "spacing": list(self.spacing),
            "data_type": self.data_type,
        }
