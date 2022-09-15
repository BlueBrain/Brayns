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

from .sonata_report_type import SonataReportType


@dataclass
class SonataReport:
    """Describe a SONATA report to load.

    Use one of the factory methods to create this object.
    """

    type: SonataReportType
    name: str | None = None
    spike_transition_time: float | None = None

    @staticmethod
    def none() -> SonataReport:
        """No report to load."""
        return SonataReport(
            type=SonataReportType.NONE,
        )

    @staticmethod
    def spikes(spike_transition_time: float = 1.0) -> SonataReport:
        """Spike report with transition time (units from report)."""
        return SonataReport(
            type=SonataReportType.SPIKES,
            spike_transition_time=spike_transition_time
        )

    @staticmethod
    def compartment(name: str) -> SonataReport:
        """Compartment report with given name."""
        return SonataReport(
            type=SonataReportType.COMPARTMENT,
            name=name,
        )

    @staticmethod
    def summation(name: str) -> SonataReport:
        """Simulation report with given name."""
        return SonataReport(
            type=SonataReportType.SUMMATION,
            name=name,
        )

    @staticmethod
    def synapse(name: str) -> SonataReport:
        """Synapse report with given name."""
        return SonataReport(
            type=SonataReportType.SYNAPSE,
            name=name,
        )

    @staticmethod
    def bloodflow_pressure(name: str) -> SonataReport:
        """Bloodflow pressure report with given name."""
        return SonataReport(
            type=SonataReportType.BLOODFLOW_PRESSURE,
            name=name,
        )

    @staticmethod
    def bloodflow_speed(name: str) -> SonataReport:
        """Bloodflow speed report with given name."""
        return SonataReport(
            type=SonataReportType.BLOODFLOW_SPEED,
            name=name,
        )

    @staticmethod
    def bloodflow_radii(name: str) -> SonataReport:
        """Bloodflow radii report with given name."""
        return SonataReport(
            type=SonataReportType.BLOODFLOW_RADII,
            name=name,
        )
