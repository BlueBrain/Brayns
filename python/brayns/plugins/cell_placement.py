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

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Any

from brayns.core import Loader

from .morphology import Morphology, serialize_morphology


@dataclass
class CellPlacementLoader(Loader):
    """circuit.morhpologies.h5 file loader.

    Loads morphology circuits from the circuit builder intermediate files.

    :param morphologies_folder: Path to the folder containing the morphologies.
    :type morphologies_folder: str
    :param density: Density of morphologies to load [0-1].
    :type density: float
    :param extension: Optional morphology file extension to load.
    :type extension: str | None, optional
    :param ids: IDs of the nodes to load, overrides density if not None.
    :type ids: list[str] | None, optional
    :param morphology: Morphology loading settings.
    :type morphology: Morphology, optional
    """

    morphologies_folder: str
    density: float = 1.0
    extension: str | None = None
    ids: list[int] | None = None
    morphology: Morphology = field(default_factory=Morphology)

    @classmethod
    @property
    def name(cls) -> str:
        return "Cell placement loader"

    def get_properties(self) -> dict[str, Any]:
        message: dict[str, Any] = {
            "morphology_folder": self.morphologies_folder,
            "morphology_parameters": serialize_morphology(self.morphology),
        }
        if self.extension is not None:
            message["extension"] = self.extension
        if self.density != 1.0:
            message["percentage"] = self.density
        if self.ids is not None:
            message["ids"] = self.ids
        return message
