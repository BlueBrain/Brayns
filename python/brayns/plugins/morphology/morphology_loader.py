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

from dataclasses import dataclass, field
from typing import Any, ClassVar

from brayns.core import Loader

from .morphology import Morphology
from .serialize_morphology import serialize_morphology


@dataclass
class MorphologyLoader(Loader):
    """Loader for morphology files.

    :param morphology: How to load the morphologies, default constructed.
    :type morphology: Morphology, optional
    """

    SWC: ClassVar[str] = 'swc'
    H5: ClassVar[str] = 'h5'
    ASC: ClassVar[str] = 'asc'

    morphology: Morphology = field(default_factory=Morphology)

    @classmethod
    @property
    def name(cls) -> str:
        return 'Neuron Morphology loader'

    def get_properties(self) -> dict[str, Any]:
        return serialize_morphology(self.morphology)
