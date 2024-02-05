# Copyright 2015-2024 Blue Brain Project/EPFL
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

schema = {
    'async': False,
    'description': 'Return information about a specific report from a circuit',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'path': {
                    'description': 'Path to the circuit BlueConfig from which to get the report',
                    'type': 'string'
                },
                'report': {
                    'description': 'Name of the report from where to get the information',
                    'type': 'string'
                }
            },
            'required': [
                'path',
                'report'
            ],
            'title': 'CIGetReportInfoParams',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Info',
    'returns': {
        'additionalProperties': False,
        'properties': {
            'data_unit': {
                'description': 'Unit of the report values. Can be (mV, mA, ...)',
                'type': 'string'
            },
            'end_time': {
                'description': 'Time at which the simulation ends',
                'type': 'number'
            },
            'frame_count': {
                'description': 'Number of simulation frames in the report',
                'minimum': 0,
                'type': 'integer'
            },
            'frame_size': {
                'description': 'Number of values per frame in the report',
                'minimum': 0,
                'type': 'integer'
            },
            'start_time': {
                'description': 'Time at which the simulation starts',
                'type': 'number'
            },
            'time_step': {
                'description': 'Time between two consecutive simulation frames',
                'type': 'number'
            },
            'time_unit': {
                'description': 'Unit of the report time values',
                'type': 'string'
            }
        },
        'required': [
            'start_time',
            'end_time',
            'time_step',
            'data_unit',
            'time_unit',
            'frame_count',
            'frame_size'
        ],
        'title': 'CIGetReportInfoResult',
        'type': 'object'
    },
    'title': 'ci-get-report-info',
    'type': 'method'
}

params = {
    'path': '/home/acfleury/Source/bin/TestData/share/BBPTestData/circuitBuilding_1000neurons/BlueConfig',
    'report': 'somas'
}

result = {
    'check': 0.9441877530564329
}
