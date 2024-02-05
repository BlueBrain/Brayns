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
    'description': 'Return general info about a circuit',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'path': {
                    'description': 'Path of the circuit config file',
                    'type': 'string'
                }
            },
            'required': [
                'path'
            ],
            'title': 'CIInfoParams',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Info',
    'returns': {
        'additionalProperties': False,
        'properties': {
            'cells_count': {
                'description': 'Number of cells in this circuit',
                'minimum': 0,
                'type': 'integer'
            },
            'cells_properties': {
                'description': 'List of available cell properties',
                'items': {
                    'type': 'string'
                },
                'type': 'array'
            },
            'e_types': {
                'description': 'List of electrical types available in this circuit',
                'items': {
                    'type': 'string'
                },
                'type': 'array'
            },
            'm_types': {
                'description': 'List of morphology types available in this circuit',
                'items': {
                    'type': 'string'
                },
                'type': 'array'
            },
            'reports': {
                'description': 'List of report names',
                'items': {
                    'type': 'string'
                },
                'type': 'array'
            },
            'spike_report': {
                'description': 'Path to the spike report file',
                'type': 'string'
            },
            'targets': {
                'description': 'List of target names',
                'items': {
                    'type': 'string'
                },
                'type': 'array'
            }
        },
        'required': [
            'cells_count',
            'cells_properties',
            'm_types',
            'e_types',
            'targets',
            'reports',
            'spike_report'
        ],
        'title': 'CIInfoResult',
        'type': 'object'
    },
    'title': 'ci-info',
    'type': 'method'
}

params = {
    'path': '/home/acfleury/Source/bin/TestData/share/BBPTestData/circuitBuilding_1000neurons/BlueConfig'
}

result = {
    'check': 0.31498816031075283
}
