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
from dataclasses import dataclass, field

from brayns.plugins import BbpReport, BbpReportType

from ..cli import Cli


@dataclass
class BbpReportCli(Cli):

    type: str | None = None
    available_types: list[str] = field(default_factory=lambda: [
        report.value
        for report in BbpReportType
    ])
    name: str | None = None

    def register(self, parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            '--report_type',
            choices=self.available_types,
            metavar='TYPE',
            help='Type of the report to load if any',
        )
        parser.add_argument(
            '--report_name',
            metavar='NAME',
            help='Name of the report to load if necessary',
        )

    def load(self, args: argparse.Namespace) -> None:
        self.type = args.report_type
        self.name = args.report_name

    def create_report(self) -> BbpReport:
        return BbpReport(
            type=BbpReportType(self.type),
            name=self.name,
        )
