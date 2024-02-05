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
    'description': 'Add a visual 3D grid to the scene',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'max_value': {
                    'description': 'Positive square grid length from world origin',
                    'type': 'number'
                },
                'min_value': {
                    'description': 'Negative square grid length from world origin',
                    'type': 'number'
                },
                'plane_opacity': {
                    'description': 'Opacity of the grid mesh material',
                    'type': 'number'
                },
                'radius': {
                    'description': 'Radius of the cylinder that will be placed at each cell',
                    'type': 'number'
                },
                'show_axis': {
                    'description': 'Wether to show a world aligned axis',
                    'type': 'boolean'
                },
                'steps': {
                    'description': 'Number of divisions',
                    'type': 'number'
                },
                'use_colors': {
                    'description': 'Use colors on the grid axes',
                    'type': 'boolean'
                }
            },
            'required': [
                'min_value',
                'max_value',
                'steps',
                'radius',
                'plane_opacity',
                'show_axis',
                'use_colors'
            ],
            'title': 'AddGridMessage',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Explorer',
    'returns': {},
    'title': 'add-grid',
    'type': 'method'
}

params = {
    'max_value': 3,
    'min_value': 3,
    'plane_opacity': 0.5,
    'radius': 0.01,
    'show_axis': True,
    'steps': 8,
    'use_colors': True
}

result = {
    'check': 0.11004324455452119
}
