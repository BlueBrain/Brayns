# Copyright (c) 2015-2022 EPFL/Blue Brain Project
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
import pathlib

import brayns

EXECUTABLE = 'path/to/braynsService'
OSPRAY = 'path/to/OSPRAY_2_10/install_dir/lib'
CIRCUIT = '/gpfs/bbp.cscs.ch/project/proj3/TestData/install/share/BBPTestData/circuitBuilding_1000neurons/BlueConfig'
SNAPSHOT = 'path/to/snapshot.png'
FRAMES = 'path/to/frames'
MOVIE = 'path/to/movie.mp4'

launcher = brayns.Launcher(
    uri='0.0.0.0:5000',
    log_level=brayns.LogLevel.INFO,
    executable=EXECUTABLE,
    env={'LD_LIBRARY_PATH': OSPRAY},
)

connector = brayns.Connector(
    uri='localhost:5000',
    logger=brayns.Logger(logging.INFO),
    max_attempts=None,
)

with launcher.start() as process:

    with connector.connect() as instance:

        loader = brayns.BbpLoader(
            cells=brayns.BbpCells.from_density(1),
            report=brayns.BbpReport.compartment('somas'),
            morphology=brayns.Morphology(
                radius_multiplier=10,
                load_soma=True,
                load_dendrites=False,
                geometry_type=brayns.GeometryType.SMOOTH,
            )
        )

        models = loader.load(instance, CIRCUIT)
        model = models[0]

        camera = brayns.PerspectiveCamera()

        view = camera.fovy.get_full_screen_view(model.bounds)

        renderer = brayns.InteractiveRenderer()

        light = brayns.DirectionalLight(
            intensity=4,
            direction=view.direction,
        )

        brayns.add_light(instance, light)

        snapshot = brayns.Snapshot(
            resolution=brayns.Resolution.full_hd,
            view=view,
            camera=camera,
            renderer=renderer,
        )

        snapshot.save(instance, SNAPSHOT)

        simulation = brayns.get_simulation(instance)

        frames = brayns.MovieFrames(
            fps=25,
            slowing_factor=1000,
            start_frame=0,
            end_frame=-1,
        )

        indices = frames.get_indices(simulation)

        for frame in pathlib.Path(FRAMES).glob('*.*'):
            frame.unlink()

        exporter = brayns.FrameExporter(
            frames=brayns.KeyFrame.from_indices(indices, view),
            format=brayns.ImageFormat.PNG,
            resolution=brayns.Resolution.full_hd,
            camera=camera,
            renderer=renderer,
        )

        exporter.export_frames(instance, FRAMES)

        movie = brayns.Movie(
            frames_folder=FRAMES,
            frames_format=exporter.format,
            fps=frames.fps,
        )

        movie.save(MOVIE)
