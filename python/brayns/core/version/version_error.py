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

from dataclasses import dataclass

from brayns.utils import Error


@dataclass
class VersionError(Error):
    """Error raised when a version mismatch occurs.

    :param local: Local (Python API) version.
    :type local: str
    :param local: Remote (Binary) version.
    :type local: str
    """

    local: str
    remote: str

    def __str__(self) -> str:
        """Format exception.

        :return: Exception description.
        :rtype: str
        """
        local, remote = self.local, self.remote
        return f'Version mismatch with server {local=}, {remote=}'
