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

from enum import Enum


class Plugin(Enum):
    """All built-in plugins for braynsService.

    Plugins are loaded when the backend instance is started and cannot be
    changed afterward.

    The value is the name of the plugin dynamic library (.so).
    """

    CIRCUIT_EXPLORER = 'braynsCircuitExplorer'
    ATLAS_EXPLORER = 'braynsAtlasExplorer'
    CYLINDRIC_CAMERA = 'braynsCylindricCamera'
    DTI = 'braynsDTI'
    MOLECULE_EXPLORER = 'braynsMoleculeExplorer'

    @classmethod
    @property
    def all(cls) -> list[str]:
        """Shortcut to get all the plugin names."""
        return [plugin.value for plugin in Plugin]
