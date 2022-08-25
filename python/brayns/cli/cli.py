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
from abc import ABC, abstractmethod
from dataclasses import dataclass, field
from typing import TypeVar

T = TypeVar('T', bound='Cli')


@dataclass
class Cli(ABC):

    description: str = field(default='', init=False)

    @abstractmethod
    def register(self, parser: argparse.ArgumentParser) -> None:
        pass

    @abstractmethod
    def load(self, args: argparse.Namespace) -> None:
        pass

    def with_description(self: T, description: str) -> T:
        self.description = description
        return self

    def parse(self, argv: list[str]) -> None:
        parser = argparse.ArgumentParser(
            description=self.description,
            formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        )
        self.register(parser)
        args = parser.parse_args(argv)
        self.load(args)
