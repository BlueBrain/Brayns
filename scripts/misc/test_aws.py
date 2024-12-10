import time

import brayns
import requests

# SBO
HOST = "openbluebrain.com"
REALM = "SBO"
CLIENT_ID = "sbo-brayns"
USERNAME = "sbo-viz"
PASSWORD = "623bxDBbS2MSDJ0OLmGRTb7osn4"
KEYCLOAK_URL = f"https://{HOST}/auth/realms/{REALM}/protocol/openid-connect/token"

VSM = f"https://{HOST}/vsm/master"
VSM_START = f"{VSM}/start"
VSM_STATUS = f"{VSM}/status/" + "{job_id}"

VSM_PROXY_URL = f"{HOST}/vsm/proxy/" + "{job_id}/renderer"

PROJECT = "bbp.cscs.ch"
CIRCUIT = f"/sbo/data/project/{PROJECT}/data/scratch/proj134/workflow-outputs/05072024-atlas-release-v1.0.1-full/cellPositionConfig/root/build/circuit_config.json"

COLOR = brayns.Color4.red


def get_token() -> str:
    response = requests.post(
        KEYCLOAK_URL,
        headers={"content-type": "application/x-www-form-urlencoded"},
        data={
            "grant_type": "password",
            "client_id": CLIENT_ID,
            "username": USERNAME,
            "password": PASSWORD,
        },
    )
    response.raise_for_status()
    data = response.json()
    return data["access_token"]


def start(token: str) -> str:
    response = requests.post(
        url=VSM_START,
        json={"project": PROJECT},
        headers={"Authorization": f"Bearer {token}"},
    )
    response.raise_for_status()
    job = response.json()
    return job["job_id"]


def status(job_id: str, token: str) -> bool:
    response = requests.get(
        url=VSM_STATUS.format(job_id=job_id),
        headers={"Authorization": f"Bearer {token}"},
    )
    if not response.ok:
        return False
    job = response.json()
    return job["ready"]


def load_circuit(instance: brayns.Instance, filename: str) -> None:
    brayns.clear_models(instance)

    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="root__neurons",
                nodes=brayns.SonataNodes.from_density(0.01),
                morphology=brayns.Morphology(
                    radius_multiplier=20,
                    load_soma=False,
                    load_dendrites=False,
                    load_axon=False,
                ),
            )
        ]
    )
    model = loader.load_models(instance, filename)[0]

    brayns.set_model_color(instance, model.id, COLOR)


def render(instance: brayns.Instance, filename: str) -> None:
    resolution = brayns.Resolution.ultra_hd

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    renderer = brayns.InteractiveRenderer()

    light = brayns.DirectionalLight(5, direction=brayns.Vector3(-1, 0, -1))
    brayns.add_light(instance, light)

    snapshot = brayns.Snapshot(resolution, camera, renderer)
    snapshot.save(instance, filename)


def render_circuit(job_id: str) -> None:
    uri = VSM_PROXY_URL.format(job_id=job_id)
    context = brayns.SslClientContext()

    connector = brayns.Connector(uri, context)

    print(f"Connecting to Brayns at {uri}")
    with connector.connect() as instance:
        version = brayns.get_version(instance)
        print(f"Connected to Brayns {version}")

        print("Loading circuit")
        load_circuit(instance, CIRCUIT)
        print("Circuit loaded")

        print("Rendering circuit")
        render(instance, "test.png")
        print("Circuit rendered")


def main() -> None:
    print("Getting token")
    token = get_token()
    print(f"Got token {token}")

    print("Starting job")
    job_id = start(token)
    print(f"Job {job_id} started")

    while not status(job_id, token):
        print(f"Waiting for job {job_id} to be ready")
        time.sleep(1)

    # job_id = "d40ed0f01671421a8a712dffc12d6360"
    print(f"Job {job_id} ready")

    render_circuit(job_id)


if __name__ == "__main__":
    main()
