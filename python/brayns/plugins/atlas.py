# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

from __future__ import annotations

from abc import ABC, abstractmethod
from dataclasses import dataclass, field
from typing import Any

from brayns.core import Model, deserialize_model
from brayns.network import Instance
from brayns.utils import Color4, Vector


class AtlasUsecase(ABC):
    """Base class for all atlas visualization usecases."""

    @classmethod
    @property
    @abstractmethod
    def name(cls) -> str:
        pass

    @abstractmethod
    def get_properties(self) -> dict[str, Any]:
        pass


class AtlasDensity(AtlasUsecase):
    """Display the volume density using the model color ramp."""

    @classmethod
    @property
    def name(cls) -> str:
        return "Density"

    def get_properties(self) -> dict[str, Any]:
        return {}


class AtlasFlatmapAreas(AtlasUsecase):
    """Display the volumne flatmap areas."""

    @classmethod
    @property
    def name(cls) -> str:
        return "Flatmap areas"

    def get_properties(self) -> dict[str, Any]:
        return {}


class ColumnPosition(Vector[int]):
    """Column position XZ.

    :param x: X component.
    :type x: float
    :param z: Z component.
    :type z: float
    """

    def __new__(cls, x: int = 0, z: int = 0) -> ColumnPosition:
        return super().__new__(cls, x, z)

    @property
    def x(self) -> float:
        return self[0]

    @property
    def z(self) -> float:
        return self[1]


@dataclass
class ColumnNeighbor:
    """Column neighbor position and color.

    The position is relative to the main column coordinates.
    """

    position: ColumnPosition
    color: Color4


@dataclass
class AtlasColumnHighlight(AtlasUsecase):
    """Highlight a column and (optionally) its neighbors.

    :param position: Column coordinates.
    :type position: ColumnPosition
    :param color: Color to apply to the column.
    :type color: Color4
    :param neighbors: Neighbors to color, defaults to empty.
    :type neighbors: list[ColumnNeighbor], optional
    """

    position: ColumnPosition
    color: Color4
    neighbors: list[ColumnNeighbor] = field(default_factory=list)

    @classmethod
    @property
    def name(cls) -> str:
        return "Highlight columns"

    def get_properties(self) -> dict[str, Any]:
        return {
            "xz_coordinate": list(self.position),
            "color": list(self.color),
            "neighbours": [
                {
                    "relative_xz": list(neighbor.position),
                    "color": list(neighbor.color),
                }
                for neighbor in self.neighbors
            ],
        }


class AtlasOrientationField(AtlasUsecase):
    """Display the volume data as an orientation field."""

    @classmethod
    @property
    def name(cls) -> str:
        return "Orientation field"

    def get_properties(self) -> dict[str, Any]:
        return {}


class AtlasShellOutline(AtlasUsecase):
    """Display the volume using its shell outline.

    This is the default atlas volume display mode and is always supported.
    """

    @classmethod
    @property
    def name(cls) -> str:
        return "Outline mesh shell"

    def get_properties(self) -> dict[str, Any]:
        return {}


def get_atlas_usecases(instance: Instance, model_id: int) -> list[str]:
    """Return the list of visualization usecases supported by an atlas volume.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: ID of the atlas volume model.
    :type model_id: int
    :return: Names of the supported usecase (see ``AtlasUsecase.name``)
    :rtype: list[str]
    """
    params = {"model_id": model_id}
    return instance.request("get-available-atlas-usecases", params)


def visualize_atlas_usecase(
    instance: Instance, model_id: int, usecase: AtlasUsecase
) -> Model:
    """Create a model to visualize an atlas volume for a given usecase.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: ID of the atlas volume model.
    :type model_id: int
    :param usecase: Usecase description.
    :type usecase: AtlasUsecase
    :return: Model created for the usecase.
    :rtype: Model
    """
    params = {
        "model_id": model_id,
        "use_case": usecase.name,
        "params": usecase.get_properties(),
    }
    result = instance.request("visualize-atlas-usecase", params)
    return deserialize_model(result)
