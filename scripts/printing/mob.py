from dataclasses import dataclass

import brayns

URI = "r1i5n23:5000"

PATH = "/gpfs/bbp.cscs.ch/home/arnaudon/code/synthdb/synthdb/insitu_synthesis_inputs/mouse_MOB/insitu_full/out/circuit_config.json"
GLOMERULI = "/gpfs/bbp.cscs.ch/home/arnaudon/code/synthdb/synthdb/insitu_synthesis_inputs/mouse_MOB/all_gloms.obj"
POPULATION = "default"
OUTPUT = "/home/acfleury/source/test/scripts/printing/mob_{side}.png"

DENSITY = 1.0
RESAMPLING = 2
SUBSAMPLING = 1

LIGHT_DIRECTION = brayns.Vector3(0, -1, -1)

BACKGOUND = brayns.Color4.bbp_background.transparent
RENDERER = brayns.ProductionRenderer(background_color=BACKGOUND)

GLOMERULI_MATERIAL = brayns.GlassMaterial()
CIRCUIT_MATERIAL = brayns.PrincipledMaterial(
    roughness=0.4,
    ior=1.52,
)

RESOLUTION = brayns.Resolution(4000, 5000)

GLOMERULI_COLOR = brayns.Color4.from_color3(brayns.parse_hex_color("#9AD2BD"))

CIRCUIT_COLORS = {
    "MOBmi": brayns.Color4(1.0, 0.5, 0),
    "MOBopl": brayns.Color4(0, 0.25, 0.5),
    "MOBgr": brayns.Color4(0.5, 0, 0),
}

MODEL_ROTATION = brayns.euler(180, 0, 0, degrees=True)


@dataclass
class View:
    name: str
    camera_translation: brayns.Vector3
    camera_rotation: brayns.Rotation
    camera_distance_factor: float


VIEWS = [
    View(
        name="front",
        camera_translation=brayns.Vector3(10, 0, 0),
        camera_rotation=brayns.euler(0, 0, 0, degrees=True),
        camera_distance_factor=1.0,
    ),
    View(
        name="right",
        camera_translation=brayns.Vector3(0, -50, 0),
        camera_rotation=brayns.euler(0, 90, 0, degrees=True),
        camera_distance_factor=1.0,
    ),
]


def rotate_model(instance: brayns.Instance, model: brayns.Model) -> brayns.Model:
    transform = brayns.Transform(rotation=MODEL_ROTATION)
    return brayns.update_model(instance, model.id, transform)


def load_glomeruli(instance: brayns.Instance) -> brayns.Model:
    with open(GLOMERULI, "rb") as file:
        data = file.read()

    loader = brayns.MeshLoader()

    model = loader.upload_models(instance, loader.OBJ, data)[0]

    brayns.set_model_color(instance, model.id, GLOMERULI_COLOR)
    brayns.set_material(instance, model.id, GLOMERULI_MATERIAL)

    return model


def load_circuit(instance: brayns.Instance) -> brayns.Model:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name=POPULATION,
                nodes=brayns.SonataNodes.from_density(DENSITY),
                morphology=brayns.Morphology(
                    radius_multiplier=1,
                    load_soma=True,
                    load_dendrites=True,
                    resampling=RESAMPLING,
                    subsampling=SUBSAMPLING,
                ),
            )
        ]
    )

    model = loader.load_models(instance, PATH)[0]

    brayns.set_material(instance, model.id, CIRCUIT_MATERIAL)
    method = brayns.CircuitColorMethod.LAYER
    brayns.color_model(instance, model.id, method, CIRCUIT_COLORS)

    return model


def add_lights(instance: brayns.Instance, camera_rotation: brayns.Rotation) -> None:
    lights = [
        brayns.AmbientLight(0.5),
        brayns.DirectionalLight(
            intensity=10, direction=camera_rotation.apply(LIGHT_DIRECTION)
        ),
    ]

    for light in lights:
        brayns.add_light(instance, light)


def focus_camera(instance: brayns.Instance, view: View) -> brayns.Camera:
    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=RESOLUTION.aspect_ratio,
    )
    camera = controller.camera

    camera = camera.translate(view.camera_translation)

    camera = camera.rotate_around_target(view.camera_rotation)

    camera.distance *= view.camera_distance_factor

    return camera


def snapshot(instance: brayns.Instance, camera: brayns.Camera, side: str) -> None:
    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
    snapshot.save(instance, OUTPUT.format(side=side))


def main() -> None:
    connector = brayns.Connector(URI)

    with connector.connect() as instance:
        brayns.clear_models(instance)

        models = [load_circuit(instance), load_glomeruli(instance)]

        for model in models:
            rotate_model(instance, model)

        for view in VIEWS:
            brayns.clear_lights(instance)

            add_lights(instance, view.camera_rotation)

            camera = focus_camera(instance, view)

            snapshot(instance, camera, view.name)


if __name__ == "__main__":
    main()
