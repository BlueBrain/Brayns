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


class ProteinColorScheme(Enum):
    """Define how to color a protein model.

    :param NONE: Doesn't color the proteins.
    :param BY_ID: One color per protein.
    :param PROTEIN_ATOMS: One color per atom.
    :param PROTEIN_CHAINS: One color per protein chain.
    :param PROTEIN_RESIDUES: One color per protein residue.
    """

    NONE = "none"
    BY_ID = "by_id"
    PROTEIN_ATOMS = "protein_atoms"
    PROTEIN_CHAINS = "protein_chains"
    PROTEIN_RESIDUES = "protein_residues"


@dataclass
class ProteinLoader(Loader):
    """Loader for protein files (PDB).

    :param color_scheme: Describe how to color the proteins, defaults to none.
    :type color_scheme: ProteinColorScheme, optional
    :param radius_multiplier: Radius multiplier of the atoms, defaults to 1.
    :type radius_multiplier: float
    """

    PDB: ClassVar[str] = "pdb"
    PDB1: ClassVar[str] = "pdb1"

    color_scheme: ProteinColorScheme = ProteinColorScheme.NONE
    radius_multiplier: float = 1.0

    @classmethod
    @property
    def name(cls) -> str:
        return "protein"

    def get_properties(self) -> dict[str, Any]:
        return {
            "color_scheme": self.color_scheme.value,
            "radius_multiplier": self.radius_multiplier,
        }
