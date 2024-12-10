import brayns

AWS_URI = "sbo-brayns.shapes-registry.org:8200"

NODE = "r1i7n34"
DIRECT_URI = f"{NODE}.bbp.epfl.ch:5000"

JOB_ID = "5a394b40-e574-4c41-b645-7678944ba3e6"
TOKEN = "eyJhbGciOiJSUzI1NiIsInR5cCIgOiAiSldUIiwia2lkIiA6ICI5T0R3Z1JSTFVsTTJHbFphVDZjVklnenJsb0lzUWJmbTBDck1icXNjNHQ4In0.eyJleHAiOjE2OTMyMjQzNDEsImlhdCI6MTY5MzIyMDc0MSwiYXV0aF90aW1lIjoxNjkzMjA3NDM4LCJqdGkiOiI0MDM3ODczZi1kNWNkLTRlZTMtOTU5MC1lMGZlZGIxZjZkZDkiLCJpc3MiOiJodHRwczovL2JicGF1dGguZXBmbC5jaC9hdXRoL3JlYWxtcy9CQlAiLCJhdWQiOlsiaHR0cHM6Ly9zbGFjay5jb20iLCJjb3Jlc2VydmljZXMtZ2l0bGFiIiwiYWNjb3VudCJdLCJzdWIiOiJmOjBmZGFkZWY3LWIyYjktNDkyYi1hZjQ2LWM2NTQ5MmQ0NTljMjphY2ZsZXVyeSIsInR5cCI6IkJlYXJlciIsImF6cCI6ImJicC1icmF5bnNjaXJjdWl0c3R1ZGlvIiwic2Vzc2lvbl9zdGF0ZSI6IjFlNjY5M2JkLWJiMmUtNGMyZC1iM2Q4LTc0YmZmNGZjOWM3NSIsImFsbG93ZWQtb3JpZ2lucyI6WyJodHRwOi8vYnJheW5zY2lyY3VpdHN0dWRpby5rY3AuYmJwLmVwZmwuY2giLCJodHRwczovL2Rldi5icmF5bnNjaXJjdWl0c3R1ZGlvLmtjcC5iYnAuZXBmbC5jaCIsImh0dHA6Ly9sb2NhbGhvc3Q6ODA4MCIsImh0dHBzOi8vczIuYnJheW5zY2lyY3VpdHN0dWRpby5rY3AuYmJwLmVwZmwuY2giLCJodHRwOi8vczIuYnJheW5zY2lyY3VpdHN0dWRpby5rY3AuYmJwLmVwZmwuY2giLCJodHRwOi8vbG9jYWxob3N0OjgwODEiLCJodHRwOi8vZGV2LmJyYXluc2NpcmN1aXRzdHVkaW8ua2NwLmJicC5lcGZsLmNoIiwiaHR0cHM6Ly9icmF5bnNjaXJjdWl0c3R1ZGlvLmtjcC5iYnAuZXBmbC5jaCIsImh0dHA6Ly9zMS5icmF5bnNjaXJjdWl0c3R1ZGlvLmtjcC5iYnAuZXBmbC5jaCIsImh0dHBzOi8vczMuYnJheW5zY2lyY3VpdHN0dWRpby5rY3AuYmJwLmVwZmwuY2giLCJodHRwOi8vczMuYnJheW5zY2lyY3VpdHN0dWRpby5rY3AuYmJwLmVwZmwuY2giLCJodHRwczovL3MxLmJyYXluc2NpcmN1aXRzdHVkaW8ua2NwLmJicC5lcGZsLmNoIl0sInJlYWxtX2FjY2VzcyI6eyJyb2xlcyI6WyJiYnAtcGFtLWF1dGhlbnRpY2F0aW9uIiwib2ZmbGluZV9hY2Nlc3MiLCJ1bWFfYXV0aG9yaXphdGlvbiIsImRlZmF1bHQtcm9sZXMtYmJwIl19LCJyZXNvdXJjZV9hY2Nlc3MiOnsiaHR0cHM6Ly9zbGFjay5jb20iOnsicm9sZXMiOlsicmVzdHJpY3RlZC1hY2Nlc3MiXX0sImNvcmVzZXJ2aWNlcy1naXRsYWIiOnsicm9sZXMiOlsicmVzdHJpY3RlZC1hY2Nlc3MiXX0sImFjY291bnQiOnsicm9sZXMiOlsibWFuYWdlLWFjY291bnQiLCJtYW5hZ2UtYWNjb3VudC1saW5rcyIsInZpZXctcHJvZmlsZSJdfX0sInNjb3BlIjoib3BlbmlkIG5leHVzIHByb2ZpbGUgbG9jYXRpb24gZ3JvdXBzIGVtYWlsIiwic2lkIjoiMWU2NjkzYmQtYmIyZS00YzJkLWIzZDgtNzRiZmY0ZmM5Yzc1IiwiZW1haWxfdmVyaWZpZWQiOnRydWUsIm5hbWUiOiJBZHJpZW4gQ2hyaXN0aWFuIEZsZXVyeSIsImxvY2F0aW9uIjoiQjEgNCAyNjEuMDM4IiwicHJlZmVycmVkX3VzZXJuYW1lIjoiYWNmbGV1cnkiLCJnaXZlbl9uYW1lIjoiQWRyaWVuIENocmlzdGlhbiIsImZhbWlseV9uYW1lIjoiRmxldXJ5IiwiZW1haWwiOiJhZHJpZW4uZmxldXJ5QGVwZmwuY2gifQ.jH5pnSrTq1x__YCgI9Y_KEUQv5t8lnV_zUnR9tEZQ74mimxC9sqv2yON1bcBd33Mbx_GTrCeeM-vC24EkdUAlHTT_4jjfaPqj7q3u7VXE8e2yq-5uIfRLZ2r4o61QMisVCtbZKIT_A7Xh8CGUsrAWE1OCva0f2N-p8CCv-IU12qSSefG00vmJhJHqrHbwCeDXO02h7l-WGUcNFbToXTn838mGUyw5U1zeSCHZVMVqXKzVCMQzq45e6yOzou1KOM_1ZnBbDpDyRuppesJqYGFRcBYVGjUMdmKB5KBkdS04ZZmpwh2WLXa_OfF33EvO90soyQxY0rMwoTT3kQn4h2O9Q"
PROXY_URI = f"vsm-proxy.kcp.bbp.epfl.ch/{JOB_ID}/renderer?token={TOKEN}"

URI = AWS_URI
SSL = brayns.SslClientContext()

LOADER = brayns.MeshLoader()
PATH = "/gpfs/bbp.cscs.ch/project/proj3/tolokoban/brain.obj"

IMAGE_FORMAT = brayns.ImageFormat.JPEG
RESOLUTION = brayns.Resolution.full_hd
SAVE_AS = "snapshot.jpg"

RENDERER = brayns.InteractiveRenderer(
    samples_per_pixel=8,
    max_ray_bounces=3,
    enable_shadows=False,
    ambient_occlusion_samples=0,
)

LIGHTS = [
    brayns.AmbientLight(intensity=0.4),
    brayns.DirectionalLight(intensity=1, direction=brayns.Vector3.one),
    brayns.DirectionalLight(intensity=2, direction=-brayns.Vector3.one),
]


def prepare_scene(instance: brayns.Instance) -> None:
    brayns.clear_models(instance)
    with open(PATH, "rb") as file:
        data = file.read()
    LOADER.upload_models(instance, LOADER.OBJ, data)
    brayns.clear_lights(instance)
    for light in LIGHTS:
        brayns.add_light(instance, light)


def snapshot(instance: brayns.Instance) -> None:
    target = brayns.get_bounds(instance)
    controller = brayns.CameraController(target, RESOLUTION.aspect_ratio)
    brayns.set_camera(instance, controller.camera)
    snapshot = brayns.Snapshot(RESOLUTION, controller.camera, RENDERER)
    snapshot.save(instance, SAVE_AS)


def cleanup(instance: brayns.Instance) -> None:
    brayns.clear_models(instance)


def main() -> None:
    connector = brayns.Connector(URI, SSL)
    with connector.connect() as instance:
        prepare_scene(instance)
        snapshot(instance)
        cleanup(instance)


if __name__ == "__main__":
    main()
