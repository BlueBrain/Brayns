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
    'description': 'Update the corresponding material with the given properties',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'clipping_mode': {
                    'description': 'The choosen material clipping mode',
                    'enum': [
                        'No clipping',
                        'Plane',
                        'Sphere'
                    ],
                    'type': 'string'
                },
                'diffuse_color': {
                    'description': 'Diffuse reflection color RGB normalized',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 3,
                    'minItems': 3,
                    'type': 'array'
                },
                'emission': {
                    'description': 'The emissive property of a material',
                    'type': 'number'
                },
                'glossiness': {
                    'description': 'The glossy component of a material',
                    'type': 'number'
                },
                'material_id': {
                    'description': 'The ID that identifies this material',
                    'minimum': 0,
                    'type': 'integer'
                },
                'model_id': {
                    'description': 'The model which this material belongs to',
                    'minimum': 0,
                    'type': 'integer'
                },
                'opacity': {
                    'description': 'The transparency of the material (0 to 1)',
                    'type': 'number'
                },
                'reflection_index': {
                    'description': 'The index of reflection of the material surface',
                    'type': 'number'
                },
                'refraction_index': {
                    'description': 'The index of refraction of a transparent material',
                    'type': 'number'
                },
                'shading_mode': {
                    'description': 'The chosen shading mode',
                    'enum': [
                        'None',
                        'Diffuse',
                        'Electron',
                        'Cartoon',
                        'Electron transparency',
                        'Perlin',
                        'Diffuse transparency',
                        'Checker'
                    ],
                    'type': 'string'
                },
                'simulation_data_cast': {
                    'description': 'Wether to cast the user parameter for simulation',
                    'type': 'boolean'
                },
                'specular_color': {
                    'description': 'Specular reflection RGB normalized',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 3,
                    'minItems': 3,
                    'type': 'array'
                },
                'specular_exponent': {
                    'description': 'The specular exponent to sharpen the specular reflection',
                    'type': 'number'
                },
                'user_parameter': {
                    'description': 'A custom parameter passed to the simulation',
                    'type': 'number'
                }
            },
            'title': 'Material',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Explorer',
    'returns': {},
    'title': 'set-material',
    'type': 'method'
}

params = {
    'model_id': 0,
    'material_id': 0,
    'clipping_mode': 'plane',
    'diffuse_color': [
        0.6,
        0.6,
        0.6
    ],
    'emission': 1,
    'glossiness': 2,
    'opacity': 2,
    'reflection_index': 2,
    'refraction_index': 2,
    'shading_mode': 'diffuse',
    'simulation_data_cast': True,
    'specular_color': [
        0.89,
        0.89,
        0.89
    ],
    'specular_exponent': 11,
    'user_parameter': 3
}

result = {
    'check': 0.21570713901825467
}
