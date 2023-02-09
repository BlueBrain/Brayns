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

"""
Plugins subpackage.

It includes all science related features and optional components.
"""

from .atlas import (
    AtlasColumnHighlight,
    AtlasDensity,
    AtlasDistanceType,
    AtlasFlatmapAreas,
    AtlasLayerDistance,
    AtlasOrientationField,
    AtlasShellOutline,
    AtlasUsecase,
    ColumnNeighbor,
    ColumnPosition,
    get_atlas_usecases,
)
from .bbp import BbpCells, BbpLoader, BbpReport, BbpReportType
from .cell_placement import CellPlacementLoader
from .circuit import CircuitColorMethod, get_circuit_ids, set_circuit_thickness
from .cylindric_camera import CylindricProjection
from .dti import DtiLoader
from .morphology import GeometryType, Morphology, MorphologyLoader
from .nrrd import NrrdLoader, VoxelType
from .protein import ProteinColorScheme, ProteinLoader
from .sonata import (
    SonataEdgePopulation,
    SonataLoader,
    SonataNodePopulation,
    SonataNodes,
    SonataReport,
    SonataReportType,
)
from .xyz import XyzLoader

__all__ = [
    "AtlasColumnHighlight",
    "AtlasDensity",
    "AtlasDistanceType",
    "AtlasFlatmapAreas",
    "AtlasLayerDistance",
    "AtlasOrientationField",
    "AtlasShellOutline",
    "AtlasUsecase",
    "BbpCells",
    "BbpLoader",
    "BbpReport",
    "BbpReportType",
    "CellPlacementLoader",
    "CircuitColorMethod",
    "ColumnNeighbor",
    "ColumnPosition",
    "CylindricProjection",
    "DtiLoader",
    "GeometryType",
    "get_atlas_usecases",
    "get_circuit_ids",
    "Morphology",
    "MorphologyLoader",
    "NrrdLoader",
    "ProteinColorScheme",
    "ProteinLoader",
    "set_circuit_thickness",
    "SonataEdgePopulation",
    "SonataLoader",
    "SonataNodePopulation",
    "SonataNodes",
    "SonataReport",
    "SonataReportType",
    "VoxelType",
    "XyzLoader",
]
