import brayns

CIRCUIT = "/gpfs/bbp.cscs.ch/project/proj144/scratch/Circuits/atlas_based/2024.12.06/sonata/circuit_config.json"
MESH = "/gpfs/bbp.cscs.ch/project/proj144/scratch/Circuits/atlas_based/2024.12.06/bioname/all_gloms.obj"
NODESET = "Nodes_glom_231"

OUTPUT = "/home/acfleury/source/test/scripts/mopro720/{name}.png"

URI = "r2i2n0:5000"

POPULATION = "mainolfactorybulb_neurons"

DENSITY = 1.0

MODEL_ROTATION = brayns.euler(180, 0, 0, degrees=True)

RESOLUTION = brayns.Resolution.ultra_hd
RESOLUTION = brayns.Resolution(int(16 / 9 * 4000), 4000)
# RESOLUTION = brayns.Resolution.full_hd

RENDERER = brayns.ProductionRenderer(background_color=brayns.Color4.black.transparent)
# RENDERER = brayns.InteractiveRenderer(background_color=brayns.Color4.black.transparent)

MATERIAL = (
    brayns.PrincipledMaterial(
        roughness=0.4,
        ior=1.52,
    )
    if isinstance(RENDERER, brayns.ProductionRenderer)
    else brayns.PhongMaterial()
)

COLORS = {
    "MOBopl_mTC": brayns.Color4.from_color3(brayns.parse_hex_color("#0D99E4")),
    "MOBmi_MC": brayns.Color4.from_color3(brayns.parse_hex_color("#7634DB")),
    "MOBgr_sGC": brayns.Color4.from_color3(brayns.parse_hex_color("#E45B0D")),
    "MOBgr_dGC": brayns.Color4.from_color3(brayns.parse_hex_color("#DA1B0B")),
}

MESH_MATERIAL = (
    brayns.GlassMaterial()
    if isinstance(RENDERER, brayns.ProductionRenderer)
    else brayns.PhongMaterial()
)

MESH_COLOR = brayns.Color4.white


def add_lights(instance: brayns.Instance, camera_direction: brayns.Vector3) -> None:
    rotation = brayns.Rotation.between(-brayns.Axis.z, camera_direction)

    lights = [
        brayns.AmbientLight(intensity=0.5),
        brayns.DirectionalLight(
            intensity=10,
            direction=rotation.apply(brayns.Vector3(-1, -1, -1)),
        ),
    ]

    for light in lights:
        brayns.add_light(instance, light)


def load_circuit(
    instance: brayns.Instance, nodeset: str, morphologies: bool
) -> brayns.Model:
    population = brayns.SonataNodePopulation(
        name=POPULATION,
        nodes=brayns.SonataNodes.from_names([nodeset], DENSITY),
        morphology=brayns.Morphology(
            radius_multiplier=1,
            load_soma=True,
            load_dendrites=morphologies,
            load_axon=False,
        ),
    )

    loader = brayns.SonataLoader([population])
    model = loader.load_models(instance, CIRCUIT)[0]

    transform = brayns.Transform(rotation=MODEL_ROTATION)
    model = brayns.update_model(instance, model.id, transform)

    brayns.set_material(instance, model.id, MATERIAL)
    method = brayns.CircuitColorMethod.MTYPE
    brayns.color_model(instance, model.id, method, COLORS)

    return model


def load_mesh(instance: brayns.Instance) -> brayns.Model:
    loader = brayns.MeshLoader()
    model = loader.load_models(instance, MESH)[0]

    brayns.set_material(instance, model.id, MESH_MATERIAL)
    brayns.set_model_color(instance, model.id, MESH_COLOR)

    transform = brayns.Transform(rotation=MODEL_ROTATION)
    model = brayns.update_model(instance, model.id, transform)

    return model


def add_clipping_front(
    instance: brayns.Instance, translation: brayns.Vector3
) -> brayns.Model:
    bounds = brayns.get_bounds(instance)
    translation = bounds.size * translation
    bounds = bounds.translate(translation)
    box = brayns.Box(bounds.min, bounds.max)
    return brayns.add_clipping_geometries(instance, [box])


def focus_camera(
    target: brayns.Bounds,
    translation: brayns.Vector3,
    rotation: brayns.Rotation,
    distance_factor: float,
) -> brayns.Camera:
    controller = brayns.CameraController(
        target=target,
        aspect_ratio=RESOLUTION.aspect_ratio,
        rotation=rotation,
        translation=translation,
    )
    camera = controller.camera

    camera.distance *= distance_factor

    return camera


def take_snapshot(instance: brayns.Instance, camera: brayns.Camera, name: str) -> None:
    snapshot = brayns.Snapshot(RESOLUTION, camera, RENDERER)
    snapshot.save(instance, OUTPUT.format(name=name))


def render_front(instance: brayns.Instance) -> None:
    brayns.clear_models(instance)

    load_circuit(instance, nodeset="All", morphologies=True)
    load_mesh(instance)

    add_clipping_front(instance, translation=brayns.Vector3(0, 0.5, 0.6))

    camera = focus_camera(
        target=brayns.get_bounds(instance),
        translation=brayns.Vector3(0, 300, 0),
        rotation=brayns.euler(-30, -20, 0, degrees=True),
        distance_factor=0.55,
    )

    add_lights(instance, camera.direction)

    take_snapshot(instance, camera, "front")


def add_clipping_nodeset(
    instance: brayns.Instance, camera_direction: brayns.Vector3
) -> brayns.Model:
    bounds = brayns.get_bounds(instance)
    equation = brayns.PlaneEquation.of(
        camera_direction, -bounds.center + 0.1 * min(bounds.size) * camera_direction
    )
    plane = brayns.Plane(equation)
    return brayns.add_clipping_geometries(instance, [plane], invert_normals=False)


def render_nodeset(instance: brayns.Instance) -> None:
    brayns.clear_models(instance)

    load_circuit(instance, nodeset=NODESET, morphologies=False)

    camera = focus_camera(
        target=brayns.get_bounds(instance),
        translation=brayns.Vector3(0, 0, 0),
        rotation=brayns.euler(0, -60, 0, degrees=True),
        distance_factor=1.0,
    )

    extra_rotation = brayns.Rotation.from_axis_angle(
        camera.direction, -95, degrees=True
    )
    camera.up = extra_rotation.apply(camera.up)

    add_lights(instance, camera.direction)
    take_snapshot(instance, camera, "nodeset_somas")

    brayns.clear_models(instance)

    load_circuit(instance, nodeset=NODESET, morphologies=True)

    add_clipping_nodeset(instance, camera.direction)

    add_lights(instance, camera.direction)
    take_snapshot(instance, camera, "nodeset_dendrites")


def main() -> None:
    connector = brayns.Connector(URI)

    with connector.connect() as instance:
        render_front(instance)
        # render_nodeset(instance)


if __name__ == "__main__":
    main()
