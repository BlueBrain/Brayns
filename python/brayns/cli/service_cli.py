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

import argparse
from dataclasses import dataclass

from ..service import Bundle


@dataclass
class ServiceCli:

    port: int = 5000
    executable: str = 'braynsService'
    ospray: str = ''

    def register(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            '--port',
            type=int,
            default=self.port,
            metavar='NUMBER',
            help='Brayns service websocket server port',
        )
        parser.add_argument(
            '--executable',
            default=self.executable,
            metavar='PATH',
            help='braynsService executable path',
        )
        parser.add_argument(
            '--ospray',
            default=self.ospray,
            metavar='PATH',
            help='OSPRAY dev libs (for local dev builds)',
        )

    def parse(self, args: argparse.Namespace) -> None:
        self.port = args.port
        self.executable = args.executable
        self.ospray = args.ospray

    def create_bundle(self) -> Bundle:
        return Bundle(
            port=self.port,
            service_executable=self.executable,
            service_env={'LD_LIBRARY_PATH': self.ospray},
        )
