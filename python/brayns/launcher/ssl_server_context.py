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
from typing import Optional


@dataclass
class SslServerContext:

    private_key_file: Optional[str] = None
    private_key_passphrase: Optional[str] = None
    certificate_file: Optional[str] = None
    ca_location: Optional[str] = None

    def get_command_line(self) -> list[str]:
        args = []
        if self.private_key_file is not None:
            args.append('--private-key-file')
            args.append(self.private_key_file)
        if self.private_key_passphrase is not None:
            args.append('--private-key-passphrase')
            args.append(self.private_key_passphrase)
        if self.certificate_file is not None:
            args.append('--certificate-file')
            args.append(self.certificate_file)
        if self.ca_location is not None:
            args.append('--ca-location')
            args.append(self.ca_location)
        return args
