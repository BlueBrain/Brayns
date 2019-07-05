#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2016-2019, Blue Brain Project
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

from brayns.plugins.circuit_explorer import CircuitExplorer

from nose.tools import assert_true, assert_equal, raises
from .mocks import *


def get_circuit_explorer():
    app = brayns.Client('localhost:8200')
    ce = CircuitExplorer(app)
    clean_models()
    return ce


def clean_models():
    app = brayns.Client('localhost:8200')
    model_ids = list()
    for model in app.scene.models:
        model_ids.append(model['id'])
    app.remove_model(array=model_ids)


def test_load_circuit():
    ce = get_circuit_explorer()
    try:
        ce.load_circuit(path='/tmp/CircuitConfig', targets=['A', 'B'])
    except Exception as e:
        assert_equal(e.__class__, rockets.request_error.RequestError)



def test_set_material():
    ce = get_circuit_explorer()
    response = ce.set_material(
        model_id=0, material_id=0, diffuse_color=(1, 1, 1), specular_color=(1, 1, 1))
    assert_equal(response, 'OK')


def test_set_materials():
    ce = get_circuit_explorer()
    simulation_data_casts = list()
    opacities = list()
    refraction_indices = list()
    reflection_indices = list()
    shading_modes = list()
    diffuse_colors = list()
    specular_colors = list()
    specular_exponents = list()
    material_ids = list()
    glossinesses = list()
    emissions = list()
    clips = list()

    palette = [(1, 1, 1), (1, 1, 1)]

    nb_materials = len(palette)
    for i in range(nb_materials):
        material_ids.append(i)
        simulation_data_casts.append(False)
        opacities.append(1)
        refraction_indices.append(1.5)
        reflection_indices.append(0)
        color = palette[i]
        specular_exponents.append(100)
        glossinesses.append(1.0)
        diffuse_colors.append(color)
        specular_colors.append((1, 1, 1))
        shading_modes.append(ce.SHADING_MODE_CARTOON)
        emissions.append(0)
        clips.append(True)

    response = ce.set_materials(
        model_ids=[0], material_ids=material_ids,
        simulation_data_casts=simulation_data_casts,
        opacities=opacities, reflection_indices=reflection_indices,
        shading_modes=shading_modes,
        diffuse_colors=diffuse_colors, specular_colors=specular_colors,
        specular_exponents=specular_exponents, glossinesses=glossinesses,
        emissions=emissions, refraction_indices=refraction_indices)
    assert_equal(response, 'OK')


def test_save_to_cache():
    ce = get_circuit_explorer()
    response = ce.save_to_cache(model_id=0, path='/tmp/test')
    assert_equal(response, 'OK')


def test_get_focal_distance():
    ce = get_circuit_explorer()
    response = ce.set_camera(origin=(0, 0, 0), direction=(0, 0, 1), up=(0, 1, 0))
    assert_equal(response, 'OK')

    focal_distance = ce.get_focal_distance(coordinates=(0.5, 0.5))
    assert_equal(focal_distance, 0.0)


def test_set_material_extra_attributes():
    ce = get_circuit_explorer()
    response = ce.set_material_extra_attributes(model_id=0)
    assert_equal(response, 'OK')


def test_set_camera():
    ce = get_circuit_explorer()
    response = ce.set_camera(origin=(0, 0, 0), direction=(0, 0, 1), up=(0, 1, 0))
    assert_equal(response, 'OK')


def test_get_camera():
    ce = get_circuit_explorer()
    response = ce.set_camera(origin=(0, 0, 0), direction=(0, 0, 1), up=(0, 1, 0))
    assert_equal(response, 'OK')

    response = ce.get_camera()
    assert_equal(response, {
        'direction': [0.0, 0.0, 1.0],
        'origin': [0.0, 0.0, 0.0],
        'up': [0.0, 1.0, 0.0]})


def test_export_frames_to_disk():
    ce = get_circuit_explorer()
    response = ce.export_frames_to_disk(
        path='/tmp',
        animation_frames=[0],
        camera_definitions=[[(0.0, 0.0, 0.0), (0.0, 0.0, 1.0), (0.0, 1.0, 0.0)]])
    assert_equal(response, 'OK')


def test_cancel_frames_export():
    ce = get_circuit_explorer()
    response = ce.cancel_frames_export()
    assert_equal(response, 'OK')


def test_add_grid():
    ce = get_circuit_explorer()
    response = ce.add_grid(min_value=0, max_value=100, interval=10)
    assert_equal(response, 'OK')


if __name__ == '__main__':
    import nose
    nose.run(defaultTest=__name__)
