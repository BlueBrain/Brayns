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

from __future__ import annotations

import argparse
from dataclasses import dataclass

from ...plugins import BbpCells


@dataclass
class BbpCellsCli:

    density: float = 0.1
    targets: list[str] | None = None
    gids: list[int] | None = None

    def register(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            '--density',
            type=float,
            default=self.density,
            metavar='VALUE',
            help='Density of target(s) cells to load (0-1)',
        )
        parser.add_argument(
            '--targets',
            nargs='*',
            metavar='NAME',
            help='Names of targets to load, all if unspecified',
        )
        parser.add_argument(
            '--gids',
            type=int,
            nargs='*',
            metavar='GID',
            help='GIDs of cells to load (override density and targets)',
        )

    def parse(self, args: argparse.Namespace) -> None:
        self.density = args.density
        self.gids = args.gids
        self.targets = args.targets

    def create_cells(self) -> BbpCells:
        return BbpCells(
            density=self.density,
            targets=self.targets,
            gids=self.gids,
        )
