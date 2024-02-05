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
    'description': 'Draw a point cloud representing the number of connections for a given frame and simulation value',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'epsilon': {
                    'description': 'The value epsilon',
                    'type': 'number'
                },
                'frame': {
                    'description': 'The frame of the simulation in which to apply',
                    'minimum': 0,
                    'type': 'integer'
                },
                'model_id': {
                    'description': 'The ID of the model to save',
                    'minimum': 0,
                    'type': 'integer'
                },
                'value': {
                    'description': 'The value',
                    'type': 'number'
                }
            },
            'required': [
                'model_id',
                'frame',
                'value',
                'epsilon'
            ],
            'title': 'SetConnectionsPerValueMessage',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Explorer',
    'returns': {},
    'title': 'set-connections-per-value',
    'type': 'method'
}

params = {
    'model_id': 0,
    'frame': 0,
    'value': 10,
    'epsilon': 1
}

result = {
    'check': 0.15983912279339407
}
