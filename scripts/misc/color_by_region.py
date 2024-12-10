import random
import brayns

PATH = (
    "/gpfs/bbp.cscs.ch/data/scratch/proj134/fullcircuit/custom-1p/circuit_config.json"
)
MESH = "/home/acfleury/source/test/brain.obj"

URI = "r1i7n0:5000"

CAMERA_ROTATION = brayns.euler(0, 0, 0, degrees=True)
CAMERA_ROTATION = brayns.euler(-90, 0, 0, degrees=True)

BACKGROUND = brayns.Color4.black.transparent

RENDERER = brayns.ProductionRenderer(background_color=BACKGROUND)
RENDERER = brayns.InteractiveRenderer(background_color=BACKGROUND)

RESOLUTION = brayns.Resolution.ultra_hd

MATERIAL = brayns.PrincipledMaterial(
    roughness=0.35,
    specular=0.5,
)
MATERIAL = brayns.PhongMaterial()

connector = brayns.Connector(URI)

with connector.connect() as instance:
    brayns.clear_models(instance)

    brayns.add_light(instance, brayns.AmbientLight(intensity=0.5))
    brayns.add_light(
        instance,
        brayns.DirectionalLight(
            intensity=10,
            direction=brayns.Vector3(1, -1, -1),
        ),
    )

    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="root__neurons",
                nodes=brayns.SonataNodes.from_density(1),
                morphology=brayns.Morphology(
                    radius_multiplier=10,
                    load_soma=False,
                    load_dendrites=False,
                    resampling=1,
                ),
            )
        ]
    )
    model = loader.load_models(instance, PATH)[0]

    with open(MESH, "rb") as file:
        data = file.read()

    loader = brayns.MeshLoader()
    mesh = loader.upload_models(instance, loader.OBJ, data)[0]

    rotation = brayns.euler(180, 0, 0, degrees=True)
    transform = brayns.Transform(rotation=rotation)
    mesh = brayns.update_model(instance, mesh.id, transform)
    model = brayns.update_model(instance, model.id, transform)

    model = next(
        model for model in brayns.get_models(instance) if model.type == "neurons"
    )
    brayns.set_material(instance, model.id, MATERIAL)

    mesh = next(model for model in brayns.get_models(instance) if model.type == "mesh")
    brayns.set_material(instance, mesh.id, brayns.GhostMaterial())

    method = brayns.CircuitColorMethod.REGION
    methods = brayns.get_color_methods(instance, model.id)
    values = brayns.get_color_values(instance, model.id, method)
    colors = {
        value: brayns.Color4(
            random.uniform(0, 1), random.uniform(0, 1), random.uniform(0, 1)
        )
        for value in values
    }

    brayns.color_model(instance, model.id, method, colors)

    resolution = RESOLUTION

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    camera = camera.rotate_around_target(CAMERA_ROTATION)

    renderer = RENDERER

    snapshot = brayns.Snapshot(resolution, camera, renderer)

    snapshot.save(instance, "test.png")
