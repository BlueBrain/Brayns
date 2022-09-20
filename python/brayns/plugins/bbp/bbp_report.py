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

from dataclasses import dataclass

from .bbp_report_type import BbpReportType


@dataclass
class BbpReport:
    """BlueConfig report selection.

    Use one of the factory methods to create this object.
    """

    type: BbpReportType
    name: str | None = None
    spike_transition_time: float | None = None

    @staticmethod
    def none() -> BbpReport:
        """No reports to load."""
        return BbpReport(
            type=BbpReportType.NONE,
        )

    @staticmethod
    def spikes(spike_transition_time: float = 1.0) -> BbpReport:
        """Spike report with optional transition time.

        time unit of transition depends on the report but it can be used to make
        this transition faster or slower (ie 2 = twice slower).
        """
        return BbpReport(
            type=BbpReportType.SPIKES,
            spike_transition_time=spike_transition_time,
        )

    @staticmethod
    def compartment(name: str) -> BbpReport:
        """Compartment report with given name."""
        return BbpReport(
            type=BbpReportType.COMPARTMENT,
            name=name,
        )
