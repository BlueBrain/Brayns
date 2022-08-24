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
import sys
from abc import ABC, abstractmethod


class Cli(ABC):

    @abstractmethod
    def register(self, parser: argparse.ArgumentParser) -> None:
        pass

    @abstractmethod
    def load(self, args: argparse.Namespace) -> None:
        pass

    def parse(self, argv: list[str] = sys.argv) -> None:
        parser = argparse.ArgumentParser()
        self.register(parser)
        args = parser.parse_args(argv)
        self.load(args)
