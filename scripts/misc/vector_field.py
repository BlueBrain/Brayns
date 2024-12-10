import brayns

PATH = "/gpfs/bbp.cscs.ch/data/project/proj39/nexus/bbp/atlas/e/c/8/4/6/4/8/9/direction_vectors_ccfv3.nrrd"

URI = "localhost:5000"

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

    voxel_type = brayns.VoxelType.VECTOR
    loader = brayns.NrrdLoader(voxel_type)

    model = loader.load_models(instance, PATH)[0]

    usecase = brayns.AtlasVectorField()
    usecase.run(instance, model.id)

    brayns.remove_models(instance, [model.id])

    resolution = brayns.Resolution.ultra_hd

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    renderer = brayns.InteractiveRenderer()

    snapshot = brayns.Snapshot(resolution, camera, renderer)

    snapshot.save(instance, "test.png")
