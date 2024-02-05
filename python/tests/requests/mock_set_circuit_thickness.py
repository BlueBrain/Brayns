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
    'description': 'Modify the geometry radiuses (spheres, cones, cylinders and SDF geometries)',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'model_id': {
                    'description': 'ID of the circuit model',
                    'minimum': 0,
                    'type': 'integer'
                },
                'radius_multiplier': {
                    'description': 'Scaling factor',
                    'type': 'number'
                }
            },
            'required': [
                'model_id',
                'radius_multiplier'
            ],
            'title': 'SetCircuitThicknessMessage',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Explorer',
    'returns': {},
    'title': 'set-circuit-thickness',
    'type': 'method'
}

params = {
    'model_id': 0,
    'radius_multiplier': 0.5
}

result = {
    'check': 0.7663088144106025
}
