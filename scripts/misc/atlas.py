import brayns

URI = "localhost:5000"
PATH = "/gpfs/bbp.cscs.ch/project/proj3/TestData/install/share/BBPTestData/nrrd/density.nrrd"
PATH = "/gpfs/bbp.cscs.ch/project/proj142/entities/atlas/20231101_DG_right/[PH]y.nrrd"
PATH = "/home/acfleury/source/test/test.nrrd"
LOWER = brayns.Vector3(243, 80, 229) * 25
UPPER = brayns.Vector3(369, 256, 369) * 25
BOUNDS = brayns.Bounds(LOWER, UPPER)

connector = brayns.Connector(URI)

with connector.connect() as instance:
    brayns.clear_models(instance)

    voxel_type = brayns.VoxelType.SCALAR
    loader = brayns.NrrdLoader(voxel_type)

    model = loader.load_models(instance, PATH)[0]

    usecase = brayns.AtlasDensity()
    density = usecase.run(instance, model.id)

    brayns.remove_models(instance, [model.id])

    ramp = brayns.get_color_ramp(instance, density.id)

    ramp.value_range = brayns.ValueRange(0, 1000)
    ramp.colors = [brayns.Color4.blue.transparent, brayns.Color4.red]
    brayns.set_color_ramp(instance, density.id, ramp)

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
    snapshot.save(instance, "snapshot.png")
