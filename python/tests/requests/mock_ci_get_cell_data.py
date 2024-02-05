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
    'description': 'Return data attached to one or many cells',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'ids': {
                    'description': 'List of cell IDs',
                    'items': {
                        'minimum': 0,
                        'type': 'integer'
                    },
                    'type': 'array'
                },
                'path': {
                    'description': 'Path to circuit config file',
                    'type': 'string'
                },
                'properties': {
                    'description': 'Desired properties',
                    'items': {
                        'type': 'string'
                    },
                    'type': 'array'
                }
            },
            'required': [
                'path',
                'ids',
                'properties'
            ],
            'title': 'CIGetCellDataParams',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Info',
    'returns': {
        'additionalProperties': False,
        'properties': {
            'etypes': {
                'description': 'Requested cell e-types',
                'items': {
                    'type': 'string'
                },
                'type': 'array'
            },
            'layers': {
                'description': 'Requested cell layers',
                'items': {
                    'type': 'string'
                },
                'type': 'array'
            },
            'morphology_classes': {
                'description': 'Requested cell morphology classes',
                'items': {
                    'type': 'string'
                },
                'type': 'array'
            },
            'mtypes': {
                'description': 'Requested cell m-types',
                'items': {
                    'type': 'string'
                },
                'type': 'array'
            },
            'orientations': {
                'description': 'Requested cell orientations',
                'items': {
                    'type': 'number'
                },
                'type': 'array'
            },
            'positions': {
                'description': 'Requested cell positions',
                'items': {
                    'type': 'number'
                },
                'type': 'array'
            }
        },
        'required': [
            'etypes',
            'mtypes',
            'morphology_classes',
            'layers',
            'positions',
            'orientations'
        ],
        'title': 'CIGetCellDataResult',
        'type': 'object'
    },
    'title': 'ci-get-cell-data',
    'type': 'method'
}

params = {
    'path': '/home/acfleury/Source/bin/TestData/share/BBPTestData/circuitBuilding_1000neurons/BlueConfig',
    'ids': [
        3
    ],
    'properties': [
        'etype',
        'mtype',
        'morphology_class',
        'layer',
        'position',
        'orientation'
    ]
}

result = {
    'check': 0.1911923912468798
}
