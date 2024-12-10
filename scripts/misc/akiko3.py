from typing import cast

import pypi_xmlrpc
import requests

BBP = "bbp.opensource"
PEPY = "https://api.pepy.tech/api/v2/projects"
OUTPUT = "output.csv"


def get_total_downloads(url: str, package: str) -> int:
    url = f"{url}/{package}"
    reply = requests.get(url)
    data = reply.json()
    return data["total_downloads"]


def get_user_packages(user: str) -> list[str]:
    raw = pypi_xmlrpc.user_packages(user)
    result = cast(list[list[str]], raw)
    return [package for _, package in result]


def get_downloads_per_package(url: str, packages: list[str]) -> dict[str, int]:
    return {package: get_total_downloads(url, package) for package in packages}


def save_as_csv(downloads: dict[str, int], filename: str) -> None:
    with open(filename, "w") as file:
        file.writelines(f"{package},{count}\n" for package, count in downloads.items())


def main() -> None:
    packages = get_user_packages(BBP)
    downloads = get_downloads_per_package(PEPY, packages)
    save_as_csv(downloads, OUTPUT)


if __name__ == "__main__":
    main()
