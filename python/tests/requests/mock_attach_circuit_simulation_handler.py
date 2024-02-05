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
    'description': 'Dynamically loads and attach a simulation to a loaded model',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'circuit_configuration': {
                    'description': 'Simulation configuration file path',
                    'type': 'string'
                },
                'model_id': {
                    'description': 'The model to which to attach the handler',
                    'minimum': 0,
                    'type': 'integer'
                },
                'report_name': {
                    'description': 'The name of the report to attach',
                    'type': 'string'
                }
            },
            'required': [
                'model_id',
                'circuit_configuration',
                'report_name'
            ],
            'title': 'AttachCircuitSimulationHandlerMessage',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Explorer',
    'returns': {},
    'title': 'attach-circuit-simulation-handler',
    'type': 'method'
}

params = {
    'model_id': 0,
    'circuit_configuration': '/home/acfleury/Source/bin/TestData/share/BBPTestData/circuitBuilding_1000neurons/BlueConfig',
    'report_name': 'somas'
}

result = {
    'check': 0.5203508416362483
}
