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
    'description': 'Get the current state of the animation parameters',
    'params': [],
    'plugin': 'Core',
    'returns': {
        'additionalProperties': False,
        'properties': {
            'current': {
                'description': 'Current frame index',
                'minimum': 0,
                'type': 'integer'
            },
            'delta': {
                'description': 'Frame delta',
                'type': 'integer'
            },
            'dt': {
                'description': 'Frame time',
                'type': 'number'
            },
            'frame_count': {
                'description': 'Animation frame count',
                'minimum': 0,
                'type': 'integer'
            },
            'playing': {
                'description': 'Animation is playing',
                'type': 'boolean'
            },
            'unit': {
                'description': 'Time unit',
                'type': 'string'
            }
        },
        'title': 'AnimationParameters',
        'type': 'object'
    },
    'title': 'get-animation-parameters',
    'type': 'method'
}

params = None

result = {
    'check': 0.6139597045366021
}
