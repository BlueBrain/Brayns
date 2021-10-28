# Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

import os

MESH_FILE = '/home/acfleury/Test/circuit5000_99983_decimated.off'
BLUECONFIG_FILE = '/home/acfleury/Source/bin/TestData/share/BBPTestData/circuitBuilding_1000neurons/BlueConfig'

get_animation_parameters = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-animation-parameters'
}

set_animation_parameters = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-animation-parameters',
    'params': {
        'current': 0,
        'delta': 1,
        'dt': 0,
        'frame_count': 0,
        'playing': False,
        'unit': 'ms'
    }
}

get_camera = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-camera'
}

set_camera = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-camera',
    'params': {
        'orientation': [0.5, 0, 0, 0.866],
        'position': [600, 1000, 1000],
        'target': [0, 0, -1],
        'current': 'orthographic'
    }
}

image_jpeg = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'image-jpeg'
}

image_streaming_mode = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'image-streaming-mode',
    'params': {
        'type': 'quanta'
    }
}

trigger_jpeg_stream = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'trigger-jpeg-stream',
    'params': {}
}

get_renderer = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-renderer'
}

set_renderer = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-renderer',
    'params': {
        'accumulation': False,
        'background_color': [1, 0, 0],
        'current': 'basic',
        'head_light': False,
        'max_accum_frames': 20,
        'samples_per_pixel': 2,
        'subsampling': 8,
        'variance_threshold': 0.1
    }
}

get_version = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-version'
}

get_application_parameters = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-application-parameters'
}

set_application_parameters = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-application-parameters',
    'params': {
        'image_stream_fps': 90,
        'jpeg_compression': 30,
        'viewport': [1920, 1080]
    }
}

get_volume_parameters = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-volume-parameters'
}

set_volume_parameters = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-volume-parameters',
    'params': {
        'volume_dimensions': [1, 1, 1],
        'volume_element_spacing': [0.1, 0.1, 0.1],
        'volume_offset': [1, 1, 1],
        'gradient_shading': True,
        'single_shade': False,
        'pre_integration': True,
        'adaptive_max_sampling_rate': 3,
        'adaptive_sampling': False,
        'sampling_rate': 0.2,
        'specular': [1, 1, 1],
        'clip_box': {
            'min': [0, 0, 0],
            'max': [1, 1, 1]
        }
    }
}

get_scene = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-scene'
}

set_scene = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-scene',
    'params': {

    }
}

get_statistics = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-statistics',
    'params': {}
}

inspect = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'inspect',
    'params': {
        'position': [1, 1]
    }
}

quit = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'quit'
}

exit_later = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'exit-later',
    'params': {
        'minutes': 1
    }
}

reset_camera = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'reset-camera'
}

snapshot = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'snapshot',
    'params': {
        'size': [1920, 1080],
        'format': 'jpg'
    }
}

request_model_upload = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'request-model-upload',
    'params': {
        'name': 'My Mesh',
        'path': MESH_FILE,
        'loader_name': 'mesh',
        'loader_properties': {
            'geometryQuality': 1,
        },
        'type': 'off',
        'size': os.path.getsize(MESH_FILE),
        'chunks_id': 'OldId'
    }
}

chunk = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'chunk',
    'params': {
        'id': 'OldId',
    }
}

get_environment_map = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-environment-map'
}

set_environment_map = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-environment-map',
    'params': {
        'filename': '/home/acfleury/Test/images/Horizon.jpg'
    }
}

add_model = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'add-model',
    'params': {
        'name': 'My Mesh',
        'path': MESH_FILE,
        'loader_name': 'mesh',
        'loader_properties': {
            'geometryQuality': 1,
        }
    }
}

remove_model = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'remove-model',
    'params': {
        'ids': list(range(25))
    }
}

update_model = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'update-model',
    'params': {
        'id': 0,
        'name': 'Adrien',
        'visible': False,
        'bounding_box': True,
        'transformation': {
            'translation': [1, 0, 0]
        }
    }
}

get_model = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-model',
    'params': {
        'id': 0
    }
}

get_model_properties = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-model-properties',
    'params': {
        'id': 0
    }
}

set_model_properties = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-model-properties',
    'params': {
        'id': 0,
        'properties': {
            'test': 'test',
            'number': 1
        }
    }
}

model_properties_schema = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'model-properties-schema',
    'params': {
        'id': 0
    }
}

get_model_transfer_function = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-model-transfer-function',
    'params': {
        'id': 0
    }
}

set_model_transfer_function = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-model-transfer-function',
    'params': {
        'id': 0,
        'transfer_function': {
            'colormap': {
                'colors': [
                    [0, 0, 0],
                    [1, 1, 0]
                ],
                'name': 'test'
            },
            'opacity_curve': [
                [0, 0],
                [0.5, 0.5]
            ],
            'range': [0, 200]
        }
    }
}

add_clip_plane = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'add-clip-plane',
    'params': {
        'plane': [1, 1, 1, 0]
    }
}

get_clip_planes = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-clip-planes'
}

update_clip_plane = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'update-clip-plane',
    'params': {
        'id': 0,
        'plane': [0.5, 0.5, 0.5, 0]
    }
}

remove_clip_planes = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'remove-clip-planes',
    'params': {
        'ids': [0, 1]
    }
}

get_instances = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-instances',
    'params': {
        'id': 0,
        'result_range': [0, 130]
    }
}

update_instance = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'update-instance',
    'params': {
        'model_id': 0,
        'instance_id': 0,
        'transformation': {
            'translation': [1, 1, 1]
        },
        'bounding_box': False,
        'visible': False,
    }
}

get_loaders = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-loaders'
}

loaders_schema = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'loaders-schema'
}

get_camera_params = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-camera-params'
}

set_camera_params = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-camera-params',
    'params': {
        'apertureRadius': 1,
        'enableClippingPlanes': False,
        'focusDistance': 2,
        'fovy': 50
    }
}

get_renderer_params = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-renderer-params'
}

set_renderer_params = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-renderer-params',
    'params': {
        'volumeSpecularExponent': 22,
        'volumeAlphaCorrection': 0.6,
        'useHardwareRandomizer': True,
        'softShadowsSamples': 2
    }
}

get_lights = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-lights'
}

add_light_directional = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'add-light-directional',
    'params': {
        'color': [1, 0, 0],
        'intensity': 0.75,
        'is_visible': True,
        'direction': [0, 0, 1],
        'angular_diameter': 1
    }
}

add_light_sphere = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'add-light-sphere',
    'params': {
        'color': [1, 0, 0],
        'intensity': 0.75,
        'is_visible': True,
        'position': [0, 0, 0],
        'radius': 1
    }
}

add_light_quad = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'add-light-quad',
    'params': {
        'color': [1, 0, 0],
        'intensity': 0.75,
        'is_visible': True,
        'position': [0, 0, 0],
        'edge1': [0, 1, 0],
        'edge2': [0, 0, 1]
    }
}

add_light_spot = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'add-light-spot',
    'params': {
        'color': [1, 0, 0],
        'intensity': 0.75,
        'is_visible': True,
        'position': [0, 0, 0],
        'direction': [0, 0, 1],
        'opening_angle': 30,
        'penumbra_angle': 10,
        'radius': 1
    }
}

add_light_ambient = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'add-light-ambient',
    'params': {
        'color': [1, 0, 0],
        'intensity': 0.75,
        'is_visible': True
    }
}

remove_lights = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'remove-lights',
    'params': {
        'ids': [1, 2, 3, 4, 5]
    }
}

clear_lights = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'clear-lights'
}

get_material = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-material',
    'params': {
        'model_id': 1,
        'material_id': 1
    }
}

get_material_ids = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-material-ids',
    'params': {
        'model_id': 1
    }
}

set_material = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-material',
    'params': {
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
        'material_id': 0,
        'model_id': 0,
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
}

set_materials = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-materials',
    'params': {
        'materials': [
            {
                'model_id': 0,
                'material_id': 1,
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
            },
            {
                'model_id': 1,
                'material_id': 1,
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
        ]
    }
}

set_material_range = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-material-range',
    'params': {
        'model_id': 0,
        'material_ids': [0, 18446744073709551614],
        'properties': {
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
    }
}

set_material_extra_attributes = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-material-extra-attributes',
    'params': {
        'model_id': 0
    }
}

set_synapses_attributes = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-synapses-attributes',
    'params': {
        'circuit_configuration': BLUECONFIG_FILE,
        'gid': 0,
        'html_colors': ['#AABB99'],
        'light_emission': 12,
        'radius': 11
    }
}

save_model_to_cache = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'save-model-to-cache',
    'params': {
        'model_id': 0,
        'path': '/home/acfleury/Test/model.txt'
    }
}

set_connections_per_value = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-connections-per-value',
    'params': {
        'model_id': 0,
        'frame': 0,
        'value': 10,
        'epsilon': 1
    }
}

set_metaballs_per_simulation_value = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-metaballs-per-simulation-value',
    'params': {
        'model_id': 0,
        'frame': 0,
        'value': 10,
        'epsilon': 1,
        'grid_size': 1,
        'threshold': 1
    }
}

get_odu_camera = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-odu-camera'
}

set_odu_camera = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-odu-camera',
    'params': {
        'origin': [0, 0, 0],
        'direction': [0, 1, 0],
        'up': [0, 0, 1],
        'aperture_radius': 1,
        'focus_distance': 2
    }
}

attach_cell_growth_handler = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'attach-cell-growth-handler',
    'params': {
        'model_id': 0,
        'nb_frames': 10
    }
}

attach_circuit_simulation_handler = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'attach-circuit-simulation-handler',
    'params': {
        'model_id': 0,
        'circuit_configuration': BLUECONFIG_FILE,
        'report_name': 'somas'
    }
}

export_frames_to_disk = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'export-frames-to-disk',
    'params': {
        'path': '/home/acfleury/Test/simulation',
        'format': 'png',
        'name_after_step': False,
        'quality': 100,
        'spp': 1,
        'start_frame': 0,
        'animation_information': [0, 1, 2, 3],
        'camera_information': 4 * [0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 2]
    }
}

get_export_frames_progress = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'get-export-frames-progress'
}

make_movie = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'make-movie',
    'params': {
        'dimensions': [1920, 1080],
        'erase_frames': True,
        'fps_rate': 1,
        'frames_file_extension': 'png',
        'frames_folder_path': '/home/acfleury/Test/simulation',
        'output_movie_path': '/home/acfleury/Test/movie.mp4'
    }
}

trace_anterograde = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'trace-anterograde',
    'params': {
        'model_id': 0,
        'cell_gids': [0, 1, 2],
        'target_cell_gids': [3],
        'source_cell_color': [0, 0, 1, 1],
        'connected_cells_color': [0, 1, 0, 1],
        'non_connected_cells_color': [1, 0, 0, 1]
    }
}

add_grid = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'add-grid',
    'params': {
        'max_value': 3,
        'min_value': 3,
        'plane_opacity': 0.5,
        'radius': 0.01,
        'show_axis': True,
        'steps': 8,
        'use_colors': True,
    }
}

add_column = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'add-column',
    'params': {
        'radius': 1
    }
}

add_sphere = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'add-sphere',
    'params': {
        'center': [0, 0, 0],
        'radius': 0.1,
        'color': [1, 0, 0, 1],
        'name': 'test'
    }
}

add_pill = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'add-pill',
    'params': {
        'type': 'sigmoidpill',
        'p1': [0, -0.2, 0],
        'p2': [0, 0.2, 0],
        'radius1': 0.1,
        'radius2': 0.2,
        'color': [1, 0, 0, 1],
        'name': 'test'
    }
}

add_cylinder = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'add-cylinder',
    'params': {
        'center': [0, 0, 0],
        'up': [0, 0.2, 0],
        'radius': 0.1,
        'color': [1, 0, 0, 1],
        'name': 'test'
    }
}

add_box = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'add-box',
    'params': {
        'min_corner': [-0.2, -0.2, -0.2],
        'max_corner': [0.2, 0.2, 0.2],
        'color': [1, 0, 0, 1],
        'name': 'test'
    }
}

remap_circuit_color = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'remap-circuit-color',
    'params': {
        'model_id': 0,
        'scheme': 'by_id'
    }
}

color_cells = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'color-cells',
    'params': {
        'model_id': 0,
        'gids': ['0,1-3', '4-7'],
        'colors': [1, 0, 0, 0, 1, 0]
    }
}

mirror_model = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'mirror-model',
    'params': {
        'model_id': 0,
        'mirror_axis': [0, 1, 0]
    }
}

set_circuit_thickness = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-circuit-thickness',
    'params': {
        'model_id': 0,
        'radius_multiplier': 0.5
    }
}

add_streamlines = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'add-streamlines',
    'params': {
        'name': 'test',
        'gids': [0, 1, 2],
        'indices': [0, 1, 2],
        'vertices': [0, 1, 2],
        'opacity': 1,
        'radius': 0.1,
        'color_scheme': 0
    }
}

set_spike_simulation = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-spike-simulation',
    'params': {
        'model_id': 0,
        'gids': [0, 1, 2],
        'timestamps': [0, 1, 2],
        'dt': 1,
        'end_time': 10,
        'time_scale': 1,
        'decay_speed': 0.1,
        'spike_intensity': 1,
        'rest_intensity': 0
    }
}

set_spike_simulation_from_file = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'set-spike-simulation-from-file',
    'params': {
        'model_id': 0,
        'dt': 1,
        'time_scale': 1,
        'decay_speed': 0.1,
        'spike_intensity': 1,
        'rest_intensity': 0,
        'path': 'test'
    }
}

ci_info = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'ci-info',
    'params': {
        'path': BLUECONFIG_FILE
    }
}

ci_get_cell_data = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'ci-get-cell-data',
    'params': {
        'path': BLUECONFIG_FILE,
        'ids': [3],
        'properties': [
            'etype',
            'mtype',
            'morphology_class',
            'layer',
            'position',
            'orientation'
        ]
    }
}

ci_get_cell_ids = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'ci-get-cell-ids',
    'params': {
        'path': BLUECONFIG_FILE,
        'targets': ['allmini50', 'mini50']
    }
}

ci_get_cell_ids_from_model = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'ci-get-cell-ids-from-model',
    'params': {
        'model_id': 0
    }
}

ci_get_reports = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'ci-get-reports',
    'params': {
        'path': BLUECONFIG_FILE
    }
}

ci_get_report_info = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'ci-get-report-info',
    'params': {
        'path': BLUECONFIG_FILE,
        'report': 'somas'
    }
}

ci_get_targets = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'ci-get-targets',
    'params': {
        'path': BLUECONFIG_FILE
    }
}

ci_get_afferent_cell_ids = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'ci-get-afferent-cell-ids',
    'params': {
        'path': BLUECONFIG_FILE,
        'sources': [1, 2, 3, 4]
    }
}

ci_get_efferent_cell_ids = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'ci-get-efferent-cell-ids',
    'params': {
        'path': BLUECONFIG_FILE,
        'sources': [1, 2, 3, 4]
    }
}

ci_get_projections = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'ci-get-projections',
    'params': {
        'path': BLUECONFIG_FILE
    }
}

ci_get_projection_efferent_cell_ids = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'ci-get-projection-efferent-cell-ids',
    'params': {
        'path': BLUECONFIG_FILE,
        'projection': 'test',
        'sources': [1, 2, 3, 4]
    }
}

cancel = {
    'jsonrpc': '2.0',
    'id': 1,
    'method': 'cancel',
    'params': {
        'id': 1
    }
}
