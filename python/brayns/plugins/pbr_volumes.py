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
# All rights reserved. Do not distribute without further notice.

"""Provides a class that wraps the API exposed by the braynsPBRVolumes plug-in"""


class PBRVolumes:
    """PBRVolumes, a class that wraps the API exposed by the braynsPBRVolumes plug-in"""

    # Network defaults
    DEFAULT_RESPONSE_TIMEOUT = 360

    def __init__(self, client):
        """Creates a new PBRVolumes object"""
        self.cli = client.rockets_client

    # pylint: disable=R0913
    def add_homogeneus_volume(self, sig_a, sig_s, g=0, scale=1, Le=(1, 1, 1), density=1,
                              p0=(0, 0, 0), p1=(1, 1, 1), name=""):
        """
        Adds a homegeneus (constant density) volume to the scene enclosed in a box

        :param list sig_a: Absorption spectrum
        :param list sig_s: Scattering spectrum
        :param float g: Anysotropy factor
        :param float scale: Spectrum scaling
        :param list Le: Volume radiance
        :param float density: constant density value
        :param list p0: box minimum bound
        :param list p1: box maximum bound
        :param str name: Scene object name
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params["name"] = name
        params["absorption"] = sig_a
        params["scattering"] = sig_s
        params["scale"] = scale
        params["g"] = g
        params["Le"] = Le
        params["density"] = density
        params["p0"] = p0
        params["p1"] = p1

        return self.cli.request("add-homogeneus-volume", params=params,
                                response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    # pylint: disable=R0913
    def add_homogeneus_volume_to_model(self, model_id, sig_a, sig_s, g=0, scale=1, Le=(1, 1, 1),
                                       density=1, name=""):
        """
        Adds a homegeneus (constant density) volume to the shapes of a given model

        :param int model_id: ID identify the model to add this volume to
        :param list sig_a: Absorption spectrum
        :param list sig_s: Scattering spectrum
        :param float g: Anysotropy factor
        :param float scale: Spectrum scaling
        :param list Le: Volume radiance
        :param float density: constant density value
        :param str name: Scene object name
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params["name"] = name
        params["absorption"] = sig_a
        params["scattering"] = sig_s
        params["scale"] = scale
        params["g"] = g
        params["Le"] = Le
        params["density"] = density
        params["modelId"] = model_id

        return self.cli.request("add-homogeneus-volume-model", params=params,
                                response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    # pylint: disable=R0913
    def add_heterogeneus_volume(self, sig_a, sig_s, min_density, max_density, g=0, scale=1,
                                Le=(1, 1, 1), p0=(0, 0, 0), p1=(1, 1, 1), name=""):
        """
        Adds a heterogeneus (non constant density) volume to the scene enclosed in a box

        :param list sig_a: Absorption spectrum
        :param list sig_s: Scattering spectrum
        :param float min_density: Min density found in the volume
        :param float max_density: Max density found in the volume
        :param float g: Anysotropy factor
        :param float scale: Spectrum scaling
        :param list Le: Volume radiance
        :param list p0: box minimum bound
        :param list p1: box maximum bound
        :param str name: Scene object name
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params["name"] = name
        params["absorption"] = sig_a
        params["scattering"] = sig_s
        params["scale"] = scale
        params["g"] = g
        params["Le"] = Le
        params["minDensity"] = min_density
        params["maxDensity"] = max_density
        params["p0"] = p0
        params["p1"] = p1

        return self.cli.request("add-heterogeneus-volume", params=params,
                                response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    # pylint: disable=R0913,R0914
    def add_grid_volume(self, sig_a, sig_s, density, nx, ny, nz, g=0, scale=1,
                        Le=(1, 1, 1), p0=(0, 0, 0), p1=(1, 1, 1), grid_type="grid",
                        name=""):
        """
        Adds a grid volume to the scene enclosed in a box.

        :param list sig_a: Absorption spectrum
        :param list sig_s: Scattering spectrum
        :param floast density: Density values. Lenth must equal grid size
        :param int nx: Grid width size
        :param int ny: Grid height size
        :param int nz: Grid depth size
        :param float g: Anysotropy factor
        :param float scale: Spectrum scaling
        :param list Le: Volume radiance
        :param list p0: box minimum bound
        :param list p1: box maximum bound
        :param str grid_type: Type of grid to use (grid|vsd)
        :param str name: Scene object name
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params["name"] = name
        params["absorption"] = sig_a
        params["scattering"] = sig_s
        params["scale"] = scale
        params["g"] = g
        params["Le"] = Le
        params["density"] = density
        params["nx"] = nx
        params["ny"] = ny
        params["nz"] = nz
        params["p0"] = p0
        params["p1"] = p1
        params["grid_type"] = grid_type

        return self.cli.request("add-grid-volume", params=params,
                                response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)
