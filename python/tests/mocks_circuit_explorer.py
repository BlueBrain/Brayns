#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2016-2019, Blue Brain Project
#                          Raphael Dumusc <raphael.dumusc@epfl.ch>
#                          Daniel Nachbaur <daniel.nachbaur@epfl.ch>
#                          Cyrille Favreau <cyrille.favreau@epfl.ch>
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
# All rights reserved. Do not distribute without further notice.

import asyncio
import brayns
import rockets

from nose.tools import assert_equal

RESPONSE_OK = {'ok'}


def mock_plugin_ce_load_circuit_request(path, name='Circuit', density=1.0, random_seed=0,
                                        targets=list(), report='', report_type=0, user_data_type=0,
                                        synchronous_mode=True, circuit_color_scheme=0,
                                        mesh_folder='', mesh_filename_pattern='',
                                        mesh_transformation=False, radius_multiplier=1,
                                        radius_correction=0, load_soma=True, load_axon=True,
                                        load_dendrite=True, load_apical_dendrite=True,
                                        use_sdf=False, dampen_branch_thickness_changerate=True,
                                        use_metaballs_for_soma=False, metaballs_section_samples=5,
                                        metaballs_grid_size=20, metaballs_threshold=1,
                                        morphology_color_scheme=0, morphology_quality=0,
                                        max_distance_to_soma=1e6, cell_clipping=False):
    return RESPONSE_OK


def mock_plugin_ce_set_material(model_id, material_id, diffuse_color=(1.0, 1.0, 1.0),
                                specular_color=(1.0, 1.0, 1.0), specular_exponent=20.0, opacity=1.0,
                                reflection_index=0.0, refraction_index=1.0,
                                simulation_data_cast=True, glossiness=1.0, shading_mode=0,
                                emission=0.0, clipped=False):
    return RESPONSE_OK


def mock_plugin_ce_set_materials(model_ids, material_ids, diffuse_colors, specular_colors,
                      specular_exponents=list(), opacities=list(), reflection_indices=list(),
                      refraction_indices=list(), simulation_data_casts=list(), glossinesses=list(),
                      shading_modes=list(), emissions=list(), clips=list()):
    return RESPONSE_OK

def mock_plugin_ce_set_material_range(self, model_id, material_ids, diffuse_color=(1.0, 1.0, 1.0),
                       specular_color=(1.0, 1.0, 1.0), specular_exponent=20.0, opacity=1.0,
                       reflection_index=0.0, refraction_index=1.0, simulation_data_cast=True,
                       glossiness=1.0, shading_mode=0, emission=0.0,
                       clipping_mode=0, user_parameter=0.0):
    return RESPONSE_OK


def mock_plugin_ce_save_model_to_cache(model_id, path):
    return RESPONSE_OK


def mock_plugin_ce_get_focal_distance(coordinates):
    return 0


def mock_plugin_ce_set_material_extra_attributes(model_id):
    return RESPONSE_OK


def mock_plugin_ce_set_camera(origin, direction, up):
    return RESPONSE_OK


def mock_plugin_ce_get_camera():
    return {
        'direction': [0.0, 0.0, 1.0],
        'origin': [0.0, 0.0, 0.0],
        'up': [0.0, 1.0, 0.0]
    }


def mock_plugin_ce_export_frames_to_disk(path, animation_frames, camera_definitions):
    return RESPONSE_OK


def mock_plugin_ce_get_export_frames_progress():
    return {
        'frameNumber': 0,
        'done': False
    }


def mock_plugin_ce_make_movie(output_movie_path, fps_rate, frames_folder_path):
    return RESPONSE_OK


def mock_plugin_ce_cancel_frames_export():
    return RESPONSE_OK


def mock_plugin_ce_add_grid(min_value, max_value, interval, radius, opacity, show_axis, colored):
    return RESPONSE_OK

def mock_plugin_ce_trace_anterograde(model_id, source_cells_gid,  target_cells_gid, source_cells_color, target_cells_color, non_connected_color):
    return RESPONSE_OK

def mock_plugin_ce_add_sphere(center, radius, color, name):
    return RESPONSE_OK


def mock_plugin_ce_add_pill(p1, p2, radius, color, name):
    return RESPONSE_OK


def mock_plugin_ce_add_conepill(p1, p2, radius1, radius2, color, name):
    return RESPONSE_OK


def mock_plugin_ce_add_sigmoidpill(p1, p2, radius1, radius2, color, name):
    return RESPONSE_OK


def mock_plugin_ce_add_cylinder(center, up, radius, color, name):
    return RESPONSE_OK


def mock_plugin_ce_add_box(minCorner, maxCorner, color, name):
    return RESPONSE_OK


def mock_plugin_ce_get_material_ids(model_id):
    return {
        'ids': [0, 1, 2, 3, 4, 5, 6, 7, 8]
    }


def mock_ce_rpc_request(self, method, params=None, response_timeout=None):
    if method == 'add-model':
        return RESPONSE_OK
    if method == 'add-grid':
        return RESPONSE_OK
    if method == 'add-column':
        return RESPONSE_OK
    if method == 'save-model-to-cache':
        return RESPONSE_OK
    if method == 'load-circuit':
        return RESPONSE_OK
    if method == 'set-material':
        return RESPONSE_OK
    if method == 'set-materials':
        return RESPONSE_OK
    if method == 'set-material-range':
        return RESPONSE_OK
    if method == 'set-material-extra-attributes':
        return RESPONSE_OK
    if method == 'set-odu-camera':
        return RESPONSE_OK
    if method == 'get-odu-camera':
        return RESPONSE_OK
    if method == 'export-frames-to-disk':
        return RESPONSE_OK
    if method == 'get-export-frames-progress':
        return RESPONSE_OK
    if method == 'make-movie':
        return RESPONSE_OK
    if method == 'cancel-frames-export':
        return RESPONSE_OK
    if method == 'trace-anterograde':
        return RESPONSE_OK
    if method == 'add-sphere':
        return RESPONSE_OK
    if method == 'add-pill':
        return RESPONSE_OK
    if method == 'add-cylinder':
        return RESPONSE_OK
    if method == 'add-box':
        return RESPONSE_OK
    if method == 'get-material-ids':
        return RESPONSE_OK
    return None
