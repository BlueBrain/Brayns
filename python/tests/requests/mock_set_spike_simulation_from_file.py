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
    'description': 'Add a spike simulation loaded from a file to a model',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'decay_speed': {
                    'description': 'Speed of spike decay',
                    'type': 'number'
                },
                'dt': {
                    'description': 'Simulation time step',
                    'type': 'number'
                },
                'model_id': {
                    'description': 'The ID of the loaded model',
                    'minimum': 0,
                    'type': 'integer'
                },
                'path': {
                    'description': 'Path to BlueConfig file',
                    'type': 'string'
                },
                'rest_intensity': {
                    'description': 'Rest intensity',
                    'type': 'number'
                },
                'spike_intensity': {
                    'description': 'Spike intensity',
                    'type': 'number'
                },
                'time_scale': {
                    'description': 'Simulation time scale',
                    'type': 'number'
                }
            },
            'required': [
                'model_id',
                'dt',
                'time_scale',
                'decay_speed',
                'rest_intensity',
                'spike_intensity',
                'path'
            ],
            'title': 'SetSpikeSimulationFromFileMessage',
            'type': 'object'
        }
    ],
    'plugin': 'DTI',
    'returns': {},
    'title': 'set-spike-simulation-from-file',
    'type': 'method'
}

params = {
    'model_id': 0,
    'dt': 1,
    'time_scale': 1,
    'decay_speed': 0.1,
    'spike_intensity': 1,
    'rest_intensity': 0,
    'path': 'test'
}

result = {
    'check': 0.16558802040821907
}
