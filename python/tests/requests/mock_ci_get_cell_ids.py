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
    'description': 'Return the list of GIDs from a circuit',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'path': {
                    'description': 'Path to the circuit config file',
                    'type': 'string'
                },
                'targets': {
                    'description': 'Targets to query',
                    'items': {
                        'type': 'string'
                    },
                    'type': 'array'
                }
            },
            'required': [
                'path'
            ],
            'title': 'CIGetCellIdsParams',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Info',
    'returns': {
        'additionalProperties': False,
        'properties': {
            'ids': {
                'description': 'List of cell GIDs',
                'items': {
                    'minimum': 0,
                    'type': 'integer'
                },
                'type': 'array'
            }
        },
        'required': [
            'ids'
        ],
        'title': 'CIGetCellIdsResult',
        'type': 'object'
    },
    'title': 'ci-get-cell-ids',
    'type': 'method'
}

params = {
    'path': '/home/acfleury/Source/bin/TestData/share/BBPTestData/circuitBuilding_1000neurons/BlueConfig',
    'targets': [
        'allmini50',
        'mini50'
    ]
}

result = {
    'check': 0.6952529626378067
}
