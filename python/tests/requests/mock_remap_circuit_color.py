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
    'description': 'Remap the circuit colors to the specified scheme',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'model_id': {
                    'description': 'The model to remap',
                    'minimum': 0,
                    'type': 'integer'
                },
                'scheme': {
                    'description': 'New color scheme',
                    'enum': [
                        'None',
                        'By id',
                        'By type',
                        'By layer',
                        'By mtype',
                        'By etype',
                        'By target',
                        'Single material'
                    ],
                    'type': 'string'
                }
            },
            'required': [
                'model_id',
                'scheme'
            ],
            'title': 'RemapCircuitColorParams',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Explorer',
    'returns': {
        'additionalProperties': False,
        'properties': {
            'updated': {
                'description': 'Check if the colors of the model changed',
                'type': 'boolean'
            }
        },
        'required': [
            'updated'
        ],
        'title': 'RemapCircuitColorResult',
        'type': 'object'
    },
    'title': 'remap-circuit-color',
    'type': 'method'
}

params = {
    'model_id': 0,
    'scheme': 'by_id'
}

result = {
    'check': 0.6271791650668253
}
