import brayns

URI = "localhost:5000"
FOLDER = (
    "/gpfs/bbp.cscs.ch/project/proj142/entities/morphologies/20221112-mouse-trial/ascii"
)
PATH = "/gpfs/bbp.cscs.ch/project/proj142/scratch/circuits/20230414-bluehippo_CA_test/auxiliary/circuit.morphologies.h5"

connector = brayns.Connector(URI)

with connector.connect() as instance:
    brayns.clear_models(instance)

    loader = brayns.CellPlacementLoader(
        morphologies_folder=FOLDER,
        morphology=brayns.Morphology(
            load_soma=True,
            load_dendrites=True,
            radius_multiplier=10,
        ),
    )

    model = loader.load_models(instance, PATH)[0]

    light = brayns.AmbientLight()
    brayns.add_light(instance, light)

    resolution = brayns.Resolution.full_hd

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    renderer = brayns.InteractiveRenderer()

    snapshot = brayns.Snapshot(resolution, camera, renderer)

    snapshot.save(instance, "test.png")
