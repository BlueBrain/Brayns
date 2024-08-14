# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

import math

import pytest

from brayns import (
    Box3,
    CameraId,
    CameraSettings,
    OrthographicCamera,
    OrthographicSettings,
    PerspectiveCamera,
    PerspectiveSettings,
)


def test_look_at() -> None:
    target = Box3.full(2)

    camera = PerspectiveCamera(CameraId(1), CameraSettings(), PerspectiveSettings())
    camera.perspective.fovy = math.radians(90)
    camera.look_at(target)

    assert camera.view.position.x == pytest.approx(0)
    assert camera.view.position.y == pytest.approx(0)
    assert camera.view.position.z == pytest.approx(4)

    camera = OrthographicCamera(CameraId(1), CameraSettings(), OrthographicSettings())
    camera.look_at(target)

    assert camera.view.position.x == pytest.approx(0)
    assert camera.view.position.y == pytest.approx(0)
    assert camera.view.position.z == pytest.approx(2)
    assert camera.orthographic.height == target.height

    camera.near_clip = 10
    camera.look_at(target)

    assert camera.view.position.z == 12


def test_properties() -> None:
    settings = CameraSettings()
    perspective = PerspectiveSettings()
    camera = PerspectiveCamera(CameraId(1), settings, perspective)

    assert camera.id == 1

    camera.settings = settings
    assert camera.settings is settings

    camera.view = settings.view
    assert camera.view is settings.view

    camera.near_clip = settings.near_clip
    assert camera.near_clip is settings.near_clip

    camera.perspective.fovy = perspective.fovy
    assert camera.perspective.fovy is perspective.fovy

    orthographic = OrthographicSettings()
    camera = OrthographicCamera(CameraId(1), settings, orthographic)

    camera.orthographic.height = orthographic.height
    assert camera.orthographic.height is orthographic.height
