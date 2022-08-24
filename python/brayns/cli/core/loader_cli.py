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
from dataclasses import dataclass

from ...core import Loader


@dataclass
class LoaderCli(ABC):

    path: str = ''

    @abstractmethod
    def register_additional_args(self, parser: argparse.ArgumentParser) -> None:
        pass

    @abstractmethod
    def parse_additional_args(self, args: argparse.Namespace) -> None:
        pass

    @abstractmethod
    def create_loader(self) -> Loader:
        pass

    def register(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            '--path',
            required=True,
            metavar='PATH',
            help='Path of the model file to render',
        )
        self.register_additional_args(parser)

    def parse(self, args: argparse.Namespace) -> None:
        self.path = args.path
        self.parse_additional_args(args)
