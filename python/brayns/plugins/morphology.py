# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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
from enum import Enum
from typing import Any, ClassVar

from brayns.core import Loader


class GeometryType(Enum):
    """Type of geometry to use when loading a morphology.

    :param ORIGINAL: Use raw geometries dimensions.
    :param SMOOTH: Smooth radius changes between geometries for better visual.
    :param SECTION_SMOOTH: Smooth radius change between whole sections.
    :param CONSTANT_RADII: Apply the same radius to all geometries.
    """

    ORIGINAL = "original"
    SMOOTH = "smooth"
    SECTION_SMOOTH = "section_smooth"
    CONSTANT_RADII = "constant_radii"
    SPHERES = "spheres"


@dataclass
class Morphology:
    """Describe how to load a morphology.

    If no sections are selected (default), a sphere of ``radius_multiplier`` is used.

    :param radius_multiplier: Radius scaling, defaults to 1.
    :type radius_multiplier: float, optional
    :param load_soma: Wether to load somas, defaults to False.
    :type load_soma: bool, optional
    :param load_axon: Wether to load axons, defaults to False.
    :type load_axon: bool, optional
    :param load_dendrites: Wether to load dendrites, defaults to False.
    :type load_dendrites: bool, optional
    :param geometry_type: How to load geometries, defaults to smooth.
    :type geometry_type: GeometryType, optional
    :param resampling: Min cos of angle between two segments to merge them (disabled if > 1).
    :type resampling: float, optional
    :param subsampling: Step factor to skip morphology samples.
    :type subsampling: int, optional
    """

    @staticmethod
    def full() -> Morphology:
        """Return settings to load all sections of a morphology.

        Returns:
            Morphology: Settings for full morphology loading.
        """
        return Morphology(load_soma=True, load_dendrites=True, load_axon=True)

    radius_multiplier: float = 1.0
    load_soma: bool = False
    load_axon: bool = False
    load_dendrites: bool = False
    geometry_type: GeometryType = GeometryType.SMOOTH
    resampling: float = 2
    subsampling: int = 1


def serialize_morphology(morphology: Morphology) -> dict[str, Any]:
    return {
        "radius_multiplier": morphology.radius_multiplier,
        "load_soma": morphology.load_soma,
        "load_axon": morphology.load_axon,
        "load_dendrites": morphology.load_dendrites,
        "geometry_type": morphology.geometry_type.value,
        "resampling": morphology.resampling,
        "subsampling": morphology.subsampling,
    }


class MorphologyColorMethod:
    """Available coloring methods for morphologies."""

    MORPHOLOGY_SECTION: ClassVar[str] = "morphology section"
    MORPHOLOGY_SECTION_GEOMETRY: ClassVar[str] = "morphology section geometry"


@dataclass
class MorphologyLoader(Loader):
    """Loader for morphology files.

    :param morphology: How to load the morphologies, default constructed.
    :type morphology: Morphology, optional
    """

    SWC: ClassVar[str] = "swc"
    H5: ClassVar[str] = "h5"
    ASC: ClassVar[str] = "asc"

    morphology: Morphology = field(default_factory=Morphology.full)

    @classmethod
    @property
    def name(cls) -> str:
        return "Neuron Morphology loader"

    def get_properties(self) -> dict[str, Any]:
        return serialize_morphology(self.morphology)
