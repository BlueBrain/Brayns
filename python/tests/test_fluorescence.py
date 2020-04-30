#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020, Blue Brain Project
# Nadir Román Guerrero <nadir.romanguerrero@epfl.ch>
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
# All rights reserved. Do not distribute without further notice

import brayns
from brayns.plugins.fluorescence import Fluorescence

from .mocks import mock_connected
from .mocks import mock_http_request
from .mocks import mock_not_in_notebook
from .mocks import mock_batch
from .mocks_fluorescence import mock_fluorescene_rpc_request

from nose.tools import assert_equal

from mock import patch


def test_add_rectangle_sensor():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_fluorescene_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        fl = Fluorescence(app)
        response = fl.add_rectangle_sensor(fov=45.0, x_pixels=1, y_pixels=1,
                                           x=1, y=1, height=1)
        assert_equal(response, {'ok'})


def test_add_disk_sensor():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_fluorescene_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        fl = Fluorescence(app)
        response = fl.add_disk_sensor(fov=45.0, x_pixels=1, y_pixels=1,
                                      radius=1.0, inner_radius=0.0,
                                      height=1.0, phi_max=360.0)
        assert_equal(response, {'ok'})


def test_add_fluorescent_volume():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_fluorescene_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        fl = Fluorescence(app)
        response = fl.add_fluorescent_volume(fex=(0, 0, 0), fem=(0, 0, 0),
                                             epsilon=0, c=0, yieldv=0, gf=0)
        assert_equal(response, {'ok'})


def test_add_fluorescent_annotated_volume():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_fluorescene_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        fl = Fluorescence(app)
        response = fl.add_fluorescent_annotated_volume(g=0, ntags=0, fexs=(0, 0, 0),
                                                       fems=(0, 0, 0), epsilons=list(),
                                                       cs=list(), yields=list(),
                                                       gfs=list(), prefix="")
        assert_equal(response, {'ok'})


def test_add_fluorescent_binary_volume():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_fluorescene_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        fl = Fluorescence(app)
        response = fl.add_fluorescent_binary_volume(g=0, fex=(0, 0, 0), fem=(0, 0, 0),
                                                    epsilon=0, c=0, yieldv=0,
                                                    gf=0, prefix="")
        assert_equal(response, {'ok'})


def test_add_fluorescent_grid_volume_fromfile():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_fluorescene_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        fl = Fluorescence(app)
        response = fl.add_fluorescent_grid_volume_fromfile(g=0, fex=(0, 0, 0),
                                                           fem=(0, 0, 0), epsilon=0,
                                                           c=0, yieldv=0, gf=0,
                                                           prefix="")
        assert_equal(response, {'ok'})


def test_add_fluorescent_grid_volume():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_fluorescene_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        fl = Fluorescence(app)
        response = fl.add_fluorescent_grid_volume(g=0, fex=(0, 0, 0),fem=(0, 0, 0),
                                                  epsilon=0, c=0, yieldv=0,
                                                  gf=0, density=list(),
                                                  nx=0, ny=0, nz=0)
        assert_equal(response, {'ok'})


def test_add_fluorescent_scattering_volume():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_fluorescene_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        fl = Fluorescence(app)
        response = fl.add_fluorescent_scattering_volume(g=0, sig_a=(0, 0, 0),
                                                        sig_s=(0, 0, 0), fex=(0, 0, 0),
                                                        fem=(0, 0, 0), m_weight=0,
                                                        epsilon=0, c=0,
                                                        yieldv=0, gf=0,
                                                        s_scale=0, f_scale=0,
                                                        density=0, Le=(0, 0, 0))
        assert_equal(response, {'ok'})


def test_add_fluorescent_scattering_grid_volume():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_fluorescene_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        fl = Fluorescence(app)
        response = fl.add_fluorescent_scattering_grid_volume(g=0, sig_a=(0, 0, 0),
                                                             sig_s=(0, 0, 0), fex=(0, 0, 0),
                                                             fem=(0, 0, 0), m_weight=0,
                                                             epsilon=0, c=0,
                                                             yieldv=0, gf=0,
                                                             s_scale=0, f_scale=0,
                                                             density=list(), Le=(0, 0, 0),
                                                             nx=0, ny=0, nz=0)
        assert_equal(response, {'ok'})


def test_add_fluorescent_scattering_volume_fromfile():
    with patch('rockets.AsyncClient.connected', new=mock_connected), \
         patch('brayns.utils.http_request', new=mock_http_request), \
         patch('brayns.utils.in_notebook', new=mock_not_in_notebook), \
         patch('rockets.Client.request', new=mock_fluorescene_rpc_request), \
         patch('rockets.Client.batch', new=mock_batch):
        app = brayns.Client('localhost:8200')
        fl = Fluorescence(app)
        response = fl.add_fluorescent_scattering_volume_fromfile(g=0, sig_a=(0, 0, 0),
                                                                 sig_s=(0, 0, 0), fex=(0, 0, 0),
                                                                 fem=(0, 0, 0), m_weight=0,
                                                                 epsilon=0, c=0,
                                                                 yieldv=0, gf=0,
                                                                 s_scale=0, f_scale=0,
                                                                 Le=(0, 0, 0), prefix="")
        assert_equal(response, {'ok'})


if __name__ == '__main__':
    import nose
    nose.run(defaultTest=__name__)
