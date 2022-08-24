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

from ...plugins import BbpLoader
from ..core import LoaderCli
from .bbp_cells_cli import BbpCellsCli
from .bbp_report_cli import BbpReportCli
from .morphology_cli import MorphologyCli


@dataclass
class BbpLoaderCli(LoaderCli):

    cells: BbpCellsCli = field(default_factory=BbpCellsCli)
    report: BbpReportCli = field(default_factory=BbpReportCli)
    morphology: MorphologyCli = field(default_factory=MorphologyCli)

    def register_additional_args(self, parser: argparse.ArgumentParser) -> None:
        self.cells.register(parser)
        self.report.register(parser)
        self.morphology.register(parser)

    def load_additional_args(self, args: argparse.Namespace) -> None:
        self.cells.load(args)
        self.report.load(args)
        self.morphology.load(args)

    def create_loader(self) -> BbpLoader:
        return BbpLoader(
            cells=self.cells.create_cells(),
            report=self.report.create_report(),
            morphology=self.morphology.create_morphology(),
        )
