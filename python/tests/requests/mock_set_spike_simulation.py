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
    'description': 'Add a spike simulation to a model',
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
                'end_time': {
                    'description': 'Simulation normalized end time',
                    'type': 'number'
                },
                'gids': {
                    'description': 'List of cel GIDs',
                    'items': {
                        'minimum': 0,
                        'type': 'integer'
                    },
                    'type': 'array'
                },
                'model_id': {
                    'description': 'The ID of the loaded model',
                    'minimum': 0,
                    'type': 'integer'
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
                    'description': 'Time scale',
                    'type': 'number'
                },
                'timestamps': {
                    'description': 'List of spike timestamps',
                    'items': {
                        'type': 'number'
                    },
                    'type': 'array'
                }
            },
            'required': [
                'model_id',
                'gids',
                'timestamps',
                'dt',
                'end_time',
                'time_scale',
                'decay_speed',
                'rest_intensity',
                'spike_intensity'
            ],
            'title': 'SetSpikeSimulationMessage',
            'type': 'object'
        }
    ],
    'plugin': 'DTI',
    'returns': {},
    'title': 'set-spike-simulation',
    'type': 'method'
}

params = {
    'model_id': 0,
    'gids': [
        0,
        1,
        2
    ],
    'timestamps': [
        0,
        1,
        2
    ],
    'dt': 1,
    'end_time': 10,
    'time_scale': 1,
    'decay_speed': 0.1,
    'spike_intensity': 1,
    'rest_intensity': 0
}

result = {
    'check': 0.8363647002012455
}
