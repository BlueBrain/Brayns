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

from brayns import Box2, Box3, Vector2, Vector3


def test_vector_type() -> None:
    assert Box2.vector_type() is Vector2
    assert Box3.vector_type() is Vector3


def test_full() -> None:
    assert Box2.full(0) == Box2(Vector2.zeros(), Vector2.zeros())
    assert Box2.full(1) == Box2(-Vector2.ones(), Vector2.ones())

    assert Box3.full(0) == Box3(Vector3.zeros(), Vector3.zeros())
    assert Box3.full(1) == Box3(-Vector3.ones(), Vector3.ones())


def test_at() -> None:
    center = Vector3(1, 1, 1)
    size = Vector3.full(2)
    extent = Vector3.ones()
    assert Box3.at(center, size) == Box3(center - extent, center + extent)


def test_of() -> None:
    size = Vector3.full(2)
    extent = Vector3.ones()
    assert Box3.of(size) == Box3(-extent, extent)


def test_merge() -> None:
    boxes = [
        Box3(Vector3(-1, 2, 3), Vector3(6, 5, 4)),
        Box3(Vector3(1, -2, 3), Vector3(5, 4, 6)),
        Box3(Vector3(1, 2, -3), Vector3(4, 6, 5)),
    ]

    lower = Vector3(-1, -2, -3)
    upper = Vector3(6, 6, 6)

    assert Box3.merge(boxes) == Box3(lower, upper)


def test_contains() -> None:
    test2 = Box2(-Vector2(1, 2), Vector2(1, 2))

    assert Vector2(1, 0) in test2
    assert Vector2(1, 1) in test2
    assert Vector2(2, 2) not in test2
    assert Vector2(-5, -5) not in test2

    test3 = Box3(-Vector3(1, 2, 3), Vector3(1, 2, 3))

    assert Vector3(1, 0, 2) in test3
    assert Vector3(1, 1, 1) in test3
    assert Vector3(2, 2, 3) not in test3
    assert Vector3(-5, -5, -5) not in test3


def test_bool() -> None:
    assert Box3.of(Vector3.full(2))
    assert not Box3.zeros()
    assert not Box3(Vector3.ones(), Vector3.zeros())


def test_corners_around() -> None:
    test = Box2(
        min=Vector2(1, 2),
        max=Vector2(4, 5),
    )
    assert Box2.around(test.corners) == test

    test = Box3(
        min=Vector3(1, 2, 3),
        max=Vector3(4, 5, 6),
    )
    assert Box3.around(test.corners) == test


def test_translate() -> None:
    test = Box3(
        min=Vector3(1, 2, 3),
        max=Vector3(4, 5, 6),
    )

    translation = Vector3(1, 2, 3)

    assert test.translate(translation) == Box3(
        test.min + translation,
        test.max + translation,
    )


def test_scale() -> None:
    test = Box3(
        min=Vector3(1, 2, 3),
        max=Vector3(4, 5, 6),
    )

    scale = Vector3(1, 2, 3)

    assert test.scale(scale) == Box3(
        test.min * scale,
        test.max * scale,
    )


def test_center_and_size() -> None:
    test = Box3(
        min=Vector3(2, 4, 1),
        max=Vector3(4, 5, 6),
    )

    assert test.center == Vector3(3, 4.5, 3.5)
    assert test.size == Vector3(2, 1, 5)
    assert test.width == test.size.x
    assert test.height == test.size.y
    assert test.depth == test.size.z
