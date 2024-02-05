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
    'description': 'Retreive the list of ID of the materials in given model',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'model_id': {
                    'description': 'Model ID',
                    'minimum': 0,
                    'type': 'integer'
                }
            },
            'required': [
                'model_id'
            ],
            'title': 'GetMaterialIdsParams',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Explorer',
    'returns': {
        'additionalProperties': False,
        'properties': {
            'ids': {
                'description': 'List of material ID',
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
        'title': 'GetMaterialIdsResult',
        'type': 'object'
    },
    'title': 'get-material-ids',
    'type': 'method'
}

params = {
    'model_id': 1
}

result = {
    'check': 0.7978532730007717
}
