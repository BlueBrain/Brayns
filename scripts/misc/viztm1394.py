from pathlib import Path

import brayns
from morphio import Morphology

FIRST = False
PROD = True
RELOAD_MORPHOLOGIES = False


if FIRST:
    OUTPUT = "/home/acfleury/source/test/test1.png"
    URI = "r2i2n2:5000"
    ROOT = "/gpfs/bbp.cscs.ch/project/proj161/skeletonization/microns_minnie65_public/skeletonization_output"
    SUBFOLDERS = [Path(f"{ROOT}/{i}/morphologies") for i in range(1, 21)]
else:
    OUTPUT = "/home/acfleury/source/test/test2.png"
    URI = "r2i2n1:5000"
    ROOT = (
        "/gpfs/bbp.cscs.ch/project/proj161/skeletonization/h01/skeletonization_output"
    )
    SUBFOLDERS = [path / "morphologies" for path in Path(ROOT).iterdir()]

BACKGOUND = brayns.Color4.black.transparent

if PROD:
    RESOLUTION = 2 * brayns.Resolution.ultra_hd
    RENDERER = brayns.ProductionRenderer(background_color=BACKGOUND)
    MATERIAL = brayns.PrincipledMaterial(
        roughness=0.4,
        ior=1.52,
    )
else:
    RESOLUTION = brayns.Resolution.ultra_hd
    RENDERER = brayns.InteractiveRenderer(background_color=BACKGOUND)
    MATERIAL = brayns.PhongMaterial()

COLOR = brayns.Color4(1.0, 0.5, 0)

LIGHTS = [
    brayns.AmbientLight(0.5),
    brayns.DirectionalLight(10, direction=brayns.Vector3(1, -1, -1)),
]


def is_empty_morphology(path: Path) -> bool:
    morphology = Morphology(str(path))
    return morphology.points.size == 0


def load_morphologies(instance: brayns.Instance) -> list[brayns.Model]:
    morphology = brayns.Morphology(
        radius_multiplier=1,
        load_soma=False,
        load_dendrites=True,
        load_axon=False,
    )

    loader = brayns.MorphologyLoader(morphology)

    return [
        model
        for subfolder in SUBFOLDERS
        for file in list(subfolder.glob("*.swc"))[::4]
        if not is_empty_morphology(file)
        for model in loader.load_models(instance, str(file))
    ]


def load_scene(instance: brayns.Instance) -> None:
    models = brayns.get_models(instance)

    brayns.clear_lights(instance)

    for light in LIGHTS:
        brayns.add_light(instance, light)

    if models and not RELOAD_MORPHOLOGIES:
        return

    brayns.clear_renderables(instance)

    load_morphologies(instance)


def color_models(instance: brayns.Instance) -> None:
    models = [
        model for model in brayns.get_models(instance) if model.type == "morphology"
    ]

    for model in models:
        brayns.set_material(instance, model.id, MATERIAL)
        brayns.set_model_color(instance, model.id, COLOR)


def snapshot(instance: brayns.Instance) -> None:
    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=RESOLUTION.aspect_ratio,
    )
    camera = controller.camera

    snasphot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
    task = snasphot.save_task(instance, OUTPUT)

    for progress in task:
        print(progress)

    task.wait_for_result()


def main() -> None:
    connector = brayns.Connector(URI)

    with connector.connect() as instance:
        print("Loading morphologies")
        load_scene(instance)
        color_models(instance)

        print("Rendering snapshot")
        snapshot(instance)


if __name__ == "__main__":
    main()
