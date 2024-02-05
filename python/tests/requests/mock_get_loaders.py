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
    'description': 'Get all loaders',
    'params': [],
    'plugin': 'Core',
    'returns': {
        'items': {
            'additionalProperties': False,
            'properties': {
                'extensions': {
                    'description': 'Supported file extensions',
                    'items': {
                        'type': 'string'
                    },
                    'type': 'array'
                },
                'name': {
                    'description': 'Loader name',
                    'type': 'string'
                },
                'properties': {
                    'description': 'Loader properties',
                    'type': 'object'
                }
            },
            'required': [
                'name',
                'extensions',
                'properties'
            ],
            'title': 'LoaderInfo',
            'type': 'object'
        },
        'type': 'array'
    },
    'title': 'get-loaders',
    'type': 'method'
}

params = None

result = {
    'check': 0.4859338607355117
}
