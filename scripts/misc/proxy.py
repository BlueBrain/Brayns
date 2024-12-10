"""
Allocation of BB5 node
----------------------

salloc --account=proj3 -p interactive -t 8:00:00 --exclusive --constraint=cpu -c 72 --mem 0 --comment=certs

BB5 node script
---------------

#!/bin/bash

HOST=$(srun hostname)
PREFIX="${TMPDIR}/${HOST}.bbp.epfl.ch"
CERTIFICATE="${PREFIX}.crt"
KEY="${PREFIX}.key"

module load unstable brayns

srun braynsService \
    --uri 0.0.0.0:5000 \
    --secure true \
    --private-key-file $KEY \
    --certificate-file $CERTIFICATE
"""

import math
import time
from pathlib import Path

import brayns
import requests

NODE = "r1i7n6"
DIRECT_URI = f"{NODE}.bbp.epfl.ch:5000"

START = "https://vsm.kcp.bbp.epfl.ch/start"
STATUS = "https://vsm.kcp.bbp.epfl.ch/status/{job_id}"

TOKEN = "eyJhbGciOiJSUzI1NiIsInR5cCIgOiAiSldUIiwia2lkIiA6ICI5T0R3Z1JSTFVsTTJHbFphVDZjVklnenJsb0lzUWJmbTBDck1icXNjNHQ4In0.eyJleHAiOjE2ODY4MzIzMDQsImlhdCI6MTY4NjgyODcwNCwiYXV0aF90aW1lIjoxNjg2ODI4NTA4LCJqdGkiOiI1M2FhNGFlOS1jNDY1LTRjOTItYTcyYS0zNTIwMjkxN2E2MWMiLCJpc3MiOiJodHRwczovL2JicGF1dGguZXBmbC5jaC9hdXRoL3JlYWxtcy9CQlAiLCJhdWQiOlsiaHR0cHM6Ly9zbGFjay5jb20iLCJjb3Jlc2VydmljZXMtZ2l0bGFiIiwiYWNjb3VudCJdLCJzdWIiOiJmOjBmZGFkZWY3LWIyYjktNDkyYi1hZjQ2LWM2NTQ5MmQ0NTljMjphY2ZsZXVyeSIsInR5cCI6IkJlYXJlciIsImF6cCI6ImJicC1icmF5bnNjaXJjdWl0c3R1ZGlvIiwic2Vzc2lvbl9zdGF0ZSI6IjZkYTRhMjU5LWFlOGItNDViMS1iZjZkLTE3YTMxN2YyNDdjNyIsImFsbG93ZWQtb3JpZ2lucyI6WyJodHRwOi8vYnJheW5zY2lyY3VpdHN0dWRpby5rY3AuYmJwLmVwZmwuY2giLCJodHRwczovL2Rldi5icmF5bnNjaXJjdWl0c3R1ZGlvLmtjcC5iYnAuZXBmbC5jaCIsImh0dHA6Ly9sb2NhbGhvc3Q6ODA4MCIsImh0dHBzOi8vczIuYnJheW5zY2lyY3VpdHN0dWRpby5rY3AuYmJwLmVwZmwuY2giLCJodHRwOi8vczIuYnJheW5zY2lyY3VpdHN0dWRpby5rY3AuYmJwLmVwZmwuY2giLCJodHRwOi8vbG9jYWxob3N0OjgwODEiLCJodHRwOi8vZGV2LmJyYXluc2NpcmN1aXRzdHVkaW8ua2NwLmJicC5lcGZsLmNoIiwiaHR0cHM6Ly9icmF5bnNjaXJjdWl0c3R1ZGlvLmtjcC5iYnAuZXBmbC5jaCIsImh0dHA6Ly9zMS5icmF5bnNjaXJjdWl0c3R1ZGlvLmtjcC5iYnAuZXBmbC5jaCIsImh0dHBzOi8vczMuYnJheW5zY2lyY3VpdHN0dWRpby5rY3AuYmJwLmVwZmwuY2giLCJodHRwOi8vczMuYnJheW5zY2lyY3VpdHN0dWRpby5rY3AuYmJwLmVwZmwuY2giLCJodHRwczovL3MxLmJyYXluc2NpcmN1aXRzdHVkaW8ua2NwLmJicC5lcGZsLmNoIl0sInJlYWxtX2FjY2VzcyI6eyJyb2xlcyI6WyJiYnAtcGFtLWF1dGhlbnRpY2F0aW9uIiwib2ZmbGluZV9hY2Nlc3MiLCJ1bWFfYXV0aG9yaXphdGlvbiIsImRlZmF1bHQtcm9sZXMtYmJwIl19LCJyZXNvdXJjZV9hY2Nlc3MiOnsiaHR0cHM6Ly9zbGFjay5jb20iOnsicm9sZXMiOlsicmVzdHJpY3RlZC1hY2Nlc3MiXX0sImNvcmVzZXJ2aWNlcy1naXRsYWIiOnsicm9sZXMiOlsicmVzdHJpY3RlZC1hY2Nlc3MiXX0sImFjY291bnQiOnsicm9sZXMiOlsibWFuYWdlLWFjY291bnQiLCJtYW5hZ2UtYWNjb3VudC1saW5rcyIsInZpZXctcHJvZmlsZSJdfX0sInNjb3BlIjoib3BlbmlkIG5leHVzIHByb2ZpbGUgbG9jYXRpb24gZ3JvdXBzIGVtYWlsIiwic2lkIjoiNmRhNGEyNTktYWU4Yi00NWIxLWJmNmQtMTdhMzE3ZjI0N2M3IiwiZW1haWxfdmVyaWZpZWQiOnRydWUsIm5hbWUiOiJBZHJpZW4gQ2hyaXN0aWFuIEZsZXVyeSIsImxvY2F0aW9uIjoiQjEgNCAyNjEuMDM4IiwicHJlZmVycmVkX3VzZXJuYW1lIjoiYWNmbGV1cnkiLCJnaXZlbl9uYW1lIjoiQWRyaWVuIENocmlzdGlhbiIsImZhbWlseV9uYW1lIjoiRmxldXJ5IiwiZW1haWwiOiJhZHJpZW4uZmxldXJ5QGVwZmwuY2gifQ.d4bzjMm5eeuSdHNBfJEJU75plDnD03k_Nfd5iiUEUn9LdskzE4h9aG33UbvCYwInEFmEGqIu9GWuNpVb7c6FRE75tT57ZTPsXj3y4_eNKZsuOdXbEfBfhAcvT96rvW_wmRRiWqkOPyKUNoBZeEzWGDgkquBMn9G5ikPVW9NBPqz5LZ7gO3s0SeoUHs3OdwKfSlRj4EbgNlAXU9ib0Bp712e4Ufr9G7AEaxrmbu8dULnCtzc8cZrAup3OQ6YDwOVKmbkv2sFWBxE-fr9tpGeyrRShe3-3cloODF2_f0dYtFR9TvofhhZhOOMqNf_yPYO7tqsGla_xkd04x4k_YxySIA"

JOB_ID = "cb424de2-296b-4626-8010-3062a841c62d"
PROXY_URI = f"vsm-proxy.kcp.bbp.epfl.ch/{JOB_ID}/renderer?token={TOKEN}"

URI = PROXY_URI

CA = None
SSL = brayns.SslClientContext(CA)

LOADER = brayns.MeshLoader()
PATH = "/home/acfleury/source/test/brain.obj"

IMAGE_COUNT = 100
IMAGE_FORMAT = brayns.ImageFormat.JPEG
ACCUMULATE = False
RESOLUTIONS = [brayns.Resolution.full_hd, brayns.Resolution.full_hd // 4]

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


def start() -> str:
    reply = requests.post(
        url=START,
        json={"usecase": "SBO1"},
        headers={"Authorization": f"Bearer {TOKEN}"},
        verify=False,
    )
    job = reply.json()
    return job["job_id"]


def status(job_id: str) -> bool:
    reply = requests.get(
        url=STATUS.format(job_id=job_id),
        headers={"Authorization": f"Bearer {TOKEN}"},
        verify=False,
    )
    job = reply.json()
    return job["job_running"] and job["brayns_started"]


def upload_model(instance: brayns.Instance) -> None:
    path = Path(PATH)
    format = path.suffix[1:]
    with path.open("rb") as file:
        data = file.read()
    LOADER.upload_models(instance, format, data)


def prepare_scene(instance: brayns.Instance) -> None:
    brayns.clear_models(instance)
    upload_model(instance)
    brayns.set_renderer(instance, RENDERER)
    brayns.clear_lights(instance)
    for light in LIGHTS:
        brayns.add_light(instance, light)


def prepare_render(instance: brayns.Instance, resolution: brayns.Resolution) -> None:
    brayns.set_resolution(instance, resolution)
    target = brayns.get_bounds(instance)
    controller = brayns.CameraController(target, resolution.aspect_ratio)
    brayns.set_camera(instance, controller.camera)


def disable_render(instance: brayns.Instance) -> None:
    image = brayns.Image(accumulate=True, force_download=False)
    image.render(instance)


def trigger_render(instance: brayns.Instance) -> None:
    angle = 360 / IMAGE_COUNT
    rotation = brayns.euler(0, angle, 0, degrees=True)
    view = brayns.get_camera_view(instance)
    view = view.rotate_around_target(rotation)
    brayns.set_camera_view(instance, view)


def render_frame(instance: brayns.Instance, download: bool) -> None:
    image = brayns.Image(accumulate=ACCUMULATE, force_download=download)
    if download:
        image.download(instance, IMAGE_FORMAT)
        return
    image.render(instance)


def render_and_measure(
    instance: brayns.Instance,
    resolution: brayns.Resolution,
    render: bool,
    download: bool,
) -> tuple[float, float]:
    prepare_render(instance, resolution)
    durations = list[float]()
    for _ in range(IMAGE_COUNT):
        trigger_render(instance) if render else disable_render(instance)
        start = time.time()
        render_frame(instance, download)
        duration = time.time() - start
        durations.append(duration)
    mean = sum(durations) / len(durations)
    var = sum((duration - mean) ** 2 for duration in durations) / (len(durations) - 1)
    return mean, math.sqrt(var)


def print_duration(title: str, duration: tuple[float, float]) -> None:
    print(
        f"{title} duration: {1000 * duration[0]:.2f}ms (std = {1000 * duration[1]:.2f}ms) ({1 / duration[0]:.2f}fps)"
    )


def measure(instance: brayns.Instance, resolution: brayns.Resolution) -> None:
    render = render_and_measure(instance, resolution, render=True, download=False)
    download = render_and_measure(instance, resolution, render=False, download=True)
    both = render_and_measure(instance, resolution, render=True, download=True)
    print(f"Resolution: {resolution.width}x{resolution.height}")
    print_duration("Render", render)
    print_duration("Download", download)
    print_duration("Render and download", both)
    print()


def measure_all() -> None:
    connector = brayns.Connector(URI, SSL)
    with connector.connect() as instance:
        prepare_scene(instance)
        print(f"Image count: {IMAGE_COUNT}")
        print()
        for resolution in RESOLUTIONS:
            measure(instance, resolution)


if __name__ == "__main__":
    measure_all()
