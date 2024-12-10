import brayns

PATH = "hippo.obj"
CAMERA_POSITION = brayns.Vector3(-910.239, 1621.451, -5344.393)
CAMERA_ROTATION = brayns.euler(171.6, 42, 0, degrees=True)
FOVY = brayns.Fovy(29, degrees=True)

URI = "r1i7n0:5000"

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

    loader = brayns.MeshLoader()

    with open(PATH, "rb") as file:
        data = file.read()

    model = loader.upload_models(instance, loader.OBJ, data)[0]

    resolution = brayns.Resolution.ultra_hd

    position = CAMERA_POSITION
    direction = CAMERA_ROTATION.apply(-brayns.Axis.z)
    up = CAMERA_ROTATION.apply(brayns.Axis.y)

    view = brayns.View(position, position + direction, up)

    projection = brayns.PerspectiveProjection(FOVY)

    camera = brayns.Camera(view, projection)

    renderer = brayns.InteractiveRenderer()

    snapshot = brayns.Snapshot(resolution, camera, renderer)

    snapshot.save(instance, "test.png")
