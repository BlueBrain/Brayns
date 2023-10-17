# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

import logging

import brayns

URI = "localhost:5000"
EXECUTABLE = "path/to/braynsService"
OSPRAY = "path/to/OSPRAY_2_10/install_dir/lib"
CIRCUIT = "path/to/BlueConfig"
RESOLUTION = brayns.Resolution.full_hd
FRAMES = "path/to/frames/%d.png"
MOVIE = "path/to/movie.mp4"

service = brayns.Service(
    uri=URI,
    log_level=brayns.LogLevel.INFO,
    executable=EXECUTABLE,
    env={"LD_LIBRARY_PATH": OSPRAY},
)

connector = brayns.Connector(
    uri=URI,
    logger=brayns.Logger(logging.INFO),
    max_attempts=None,
)

with brayns.start(service, connector) as (process, instance):
    loader = brayns.BbpLoader(
        cells=brayns.BbpCells.from_density(1),
        report=brayns.BbpReport.compartment("somas"),
        morphology=brayns.Morphology(
            radius_multiplier=10,
            load_soma=True,
            load_dendrites=False,
            geometry_type=brayns.GeometryType.SMOOTH,
        ),
    )
    models = loader.load_models(instance, CIRCUIT)
    model = models[0]

    controller = brayns.CameraController(
        model.bounds,
        aspect_ratio=RESOLUTION.aspect_ratio,
        translation=brayns.Vector3.zero,
        rotation=brayns.CameraRotation.front,
        projection=brayns.PerspectiveProjection,
    )
    camera = controller.camera

    renderer = brayns.InteractiveRenderer()

    light = brayns.DirectionalLight(
        intensity=4,
        direction=camera.direction,
    )
    brayns.add_light(instance, light)

    simulation = brayns.get_simulation(instance)
    frames = brayns.MovieFrames(
        fps=1,
        slowing_factor=1000,
        start_frame=0,
        end_frame=-1,
    )
    indices = frames.get_indices(simulation)

    snapshot = brayns.Snapshot(
        resolution=RESOLUTION,
        camera=camera,
        renderer=renderer,
    )
    for index, frame in enumerate(indices):
        snapshot.save(instance, FRAMES % index)

    movie = brayns.Movie(
        frames_pattern=FRAMES,
        fps=frames.fps,
    )
    movie.save(MOVIE)
