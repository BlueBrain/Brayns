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
from dataclasses import dataclass, field

from ..plugins import BbpLoader
from .cells_cli import CellsCli
from .morphology_cli import MorphologyCli
from .report_cli import ReportCli


@dataclass
class CircuitCli:

    path: str = ''
    cells: CellsCli = field(default_factory=CellsCli)
    report: ReportCli = field(default_factory=ReportCli)
    morphology: MorphologyCli = field(default_factory=MorphologyCli)

    def register(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            '--model_path',
            required=True,
            metavar='PATH',
            help='Path of the model to load',
        )
        self.cells.register(parser)
        self.report.register(parser)
        self.morphology.register(parser)

    def parse(self, args: argparse.Namespace) -> None:
        self.path = args.circuit_path
        self.cells.parse(args)
        self.report.parse(args)
        self.morphology.parse(args)

    def create_bbp_loader(self) -> BbpLoader:
        return BbpLoader(
            cells=self.cells.create_bbp_cells(),
            report=self.report.create_bbp_report(),
            morphology=self.morphology.create_morphology(),
        )
