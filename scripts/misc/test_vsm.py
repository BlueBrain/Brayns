import json
from dataclasses import dataclass

import brayns
import requests

VSM = "http://localhost:4444"

START = f"{VSM}/start"
STOP = f"{VSM}/stop"
STATUS = f"{VSM}/status"


@dataclass
class JobStatus:
    ready: bool
    end_time: str
    job_url: str | None


def test_proxy(url: str) -> None:
    connector = brayns.Connector(url)
    with connector.connect() as instance:
        print(brayns.get_version(instance))


def start() -> str:
    response = requests.post(START, json={"test": 1}, headers={"authorization": "abc"})
    print(response.status_code)
    print(response.content)
    body = json.loads(response.content)
    return body["job_id"]


def stop(job_id: str) -> None:
    response = requests.post(f"{STOP}/{job_id}", headers={"authorization": "abc"})
    print(response.status_code)
    print(response.content)


def status(job_id: str) -> JobStatus:
    response = requests.get(f"{STATUS}/{job_id}", headers={"authorization": "abc"})
    print(response.status_code)
    print(response.content)
    body = json.loads(response.content)
    return JobStatus(
        ready=body["ready"],
        end_time=body["end_time"],
        job_url=body.get("job_url"),
    )


if __name__ == "__main__":
    # test_proxy("5c4af9a7e4684ffa8e5d289a845a11ab")
    # job_id = start()
    job_id = "ceabb571d2b04c7ead872103ab1a8a83"
    details = status(job_id)
    if details.job_url is not None:
        test_proxy(details.job_url)
    # stop("d5679a21de2f4072a367045f0e269da9")
