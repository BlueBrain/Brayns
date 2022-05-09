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

from dataclasses import dataclass
from typing import Optional


@dataclass(frozen=True)
class BbpReport:

    type: str
    name: Optional[str] = None
    spike_transition_time: Optional[float] = None

    @staticmethod
    def none() -> 'BbpReport':
        return BbpReport(type='none')

    @staticmethod
    def spikes(spike_transition_time: float = 1.0) -> 'BbpReport':
        return BbpReport(
            type='spikes',
            spike_transition_time=spike_transition_time
        )

    @staticmethod
    def compartment(name: str) -> 'BbpReport':
        return BbpReport(type='compartment', name=name)

    def serialize(self) -> dict:
        message = {
            'report_type': self.type
        }
        if self.name is not None:
            message['report_name'] = self.name
        if self.spike_transition_time is not None:
            message['spike_transition_time'] = self.spike_transition_time
        return message
