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
    'description': 'Set synapses specific attributes for a given model',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'circuit_configuration': {
                    'description': 'Path to the circuit configuration file',
                    'type': 'string'
                },
                'gid': {
                    'description': 'Target cell GID',
                    'type': 'integer'
                },
                'html_colors': {
                    'description': 'List of rgb colors in hexadecimal',
                    'items': {
                        'type': 'string'
                    },
                    'type': 'array'
                },
                'light_emission': {
                    'description': 'Emission parameter for the synapse material',
                    'type': 'number'
                },
                'radius': {
                    'description': 'Synapse geometry radius',
                    'type': 'number'
                }
            },
            'required': [
                'circuit_configuration',
                'gid',
                'html_colors',
                'light_emission',
                'radius'
            ],
            'title': 'SynapseAttributes',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Explorer',
    'returns': {},
    'title': 'set-synapses-attributes',
    'type': 'method'
}

params = {
    'circuit_configuration': '/home/acfleury/Source/bin/TestData/share/BBPTestData/circuitBuilding_1000neurons/BlueConfig',
    'gid': 0,
    'html_colors': [
        '#AABB99'
    ],
    'light_emission': 12,
    'radius': 11
}

result = {
    'check': 0.46787636353068063
}
