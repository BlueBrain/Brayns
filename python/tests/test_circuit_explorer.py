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

from brayns.plugins.circuit_explorer import CircuitExplorer

from mock import patch

from .mocks import *
from .mocks_circuit_explorer import *


def test_load_circuit():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.load_circuit(path='/tmp/CircuitConfig', targets=['A', 'B'], gids=[1, 2])
        assert_equal(response, {'ok'})


def test_set_material():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.set_material(
            model_id=0, material_id=0, diffuse_color=(1, 1, 1), specular_color=(1, 1, 1))
        assert_equal(response, {'ok'})


def test_set_materials():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
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
        assert_equal(response, {'ok'})

def test_set_material_range():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.set_material_range(
            model_id=0, material_ids=[0], diffuse_color=(1, 1, 1), specular_color=(1, 1, 1))
        assert_equal(response, {'ok'})


def test_save_to_cache():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.save_model_to_cache(model_id=0, path='/tmp/test')
        assert_equal(response, {'ok'})


def test_set_material_extra_attributes():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.set_material_extra_attributes(model_id=0)
        assert_equal(response, {'ok'})


def test_set_camera():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.set_camera(origin=(0, 0, 0), direction=(0, 0, 1), up=(0, 1, 0))
        assert_equal(response, {'ok'})


def test_get_camera():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.get_camera()
        assert_equal(response, {'ok'})


def test_export_frames_to_disk():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.export_frames_to_disk(
            path='/tmp',
            animation_frames=[0],
            camera_definitions=[[(0.0, 0.0, 0.0), (0.0, 0.0, 1.0), (0.0, 1.0, 0.0), 0.0, 100.0]])
        assert_equal(response, {'ok'})


def test_get_export_frames_progress():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.get_export_frames_progress()
        assert_equal(response, {'ok'})


def test_make_movie():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.make_movie(
            output_movie_path='/tmp',
            fps_rate=0,
            frames_folder_path='/tmp',
            erase_frames=False)
        assert_equal(response, {'ok'})


def test_cancel_frames_export():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.cancel_frames_export()
        assert_equal(response, {'ok'})

def test_trace_anterograde():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.trace_anterograde(
            model_id=0,
            source_cells_gid=list(),
            target_cells_gid=list())
        assert_equal(response, {'ok'})


def test_add_grid():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.add_grid(min_value=0, max_value=100, interval=10, radius=1, opacity=1,
                               show_axis=True, colored=True)
        assert_equal(response, {'ok'})


def test_add_column():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.add_column(radius=1)
        assert_equal(response, {'ok'})


def test_add_sphere():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.add_sphere(
            center=(0,0,0),
            radius=1.0,
            color=(1,1,1,1))
        assert_equal(response, {'ok'})


def test_add_pill():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.add_pill(
            p1=(-1,0,0),
            p2=(1,0,0),
            radius=1.0,
            color=(1,1,1,1))
        assert_equal(response, {'ok'})


def test_add_conepill():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.add_conepill(
            p1=(-1,0,0),
            p2=(1,0,0),
            radius1=1.0,
            radius2=1.0,
            color=(1,1,1,1))
        assert_equal(response, {'ok'})

def test_add_sigmoidpill():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.add_sigmoidpill(
            p1=(-1,0,0),
            p2=(1,0,0),
            radius1=1.0,
            radius2=1.0,
            color=(1,1,1,1))
        assert_equal(response, {'ok'})


def test_add_cylinder():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.add_cylinder(
            center=(0,0,0),
            up=(1,0,0),
            radius=1.0,
            color=(1,1,1,1))
        assert_equal(response, {'ok'})


def test_add_box():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.add_box(
            minCorner=(-1,-1,-1),
            maxCorner=(1,1,1),
            color=(1,1,1,1))
        assert_equal(response, {'ok'})


def test_get_material_ids():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_ce_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        ce = CircuitExplorer(app)
        response = ce.get_material_ids(0)
        assert_equal(response, {'ok'})


if __name__ == '__main__':
    import nose
    nose.run(defaultTest=__name__)
