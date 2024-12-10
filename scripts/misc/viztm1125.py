import logging

import brayns

URI = 'localhost:5000'
CIRCUIT = '/gpfs/bbp.cscs.ch/project/proj112/home/giacalon/signal_propagation_full_20211110/Dep_scan_full/campaigns/dep_all_90/002/BlueConfig'
OUTPUT = '/gpfs/bbp.cscs.ch/home/acfleury/src/Test/viztm1125/frames'

with brayns.connect(URI, log_level=logging.DEBUG) as instance:

    brayns.Model.clear(instance)

    loader = brayns.BbpLoader(
        cells=brayns.BbpCells.from_density(0.1),
        report=brayns.BbpReport.spikes(),
        radius_multiplier=20
    )
    model = loader.load(instance, CIRCUIT)[0]

    euler = brayns.Vector3(28.02, 50.447, 171.166)
    orientation = brayns.Quaternion.from_euler(euler, degrees=True)
    model.transform.rotation = orientation
    model = brayns.Model.update(instance, model.id, transform=model.transform)

    brayns.Simulation.enable(instance, model.id, False)

    camera = brayns.PerspectiveCamera()

    view = camera.get_full_screen_view(model.bounds)

    renderer = brayns.InteractiveRenderer()

    brayns.Light.clear(instance)

    light = brayns.DirectionalLight(
        intensity=5,
        direction=view.direction
    )

    light.add(instance)

    snapshot = brayns.Snapshot(
        resolution=2 * brayns.Resolution.full_hd,
        view=view,
        camera=camera,
        renderer=renderer
    )
    snapshot.save(instance, 'snapshot.png')

    brayns.Simulation.enable(instance, model.id, True)

    transfer_function = brayns.TransferFunction.from_model(instance, model.id)

    curve = brayns.OpacityCurve([
        brayns.ControlPoint(0.0, 0.5),
        brayns.ControlPoint(0.5, 0.5),
        brayns.ControlPoint(1.0, 1.0)
    ])

    transfer_function.colors = curve.apply(transfer_function.colors)

    transfer_function.apply(instance, model.id)

    simulation = brayns.Simulation.from_instance(instance)
    step = int(simulation.fps / 25 / 10)

    frames = [
        brayns.KeyFrame(i, view)
        for i in range(simulation.start_frame, simulation.end_frame, step)
    ]

    exporter = brayns.FrameExporter(
        frames=frames,
        resolution=2 * brayns.Resolution.full_hd,
        camera=camera,
        renderer=renderer
    )

    exporter.export_frames(instance, OUTPUT)
