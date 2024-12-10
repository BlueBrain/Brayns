"""
USAGE:

Create a personal Gitlab access token as described here https://docs.gitlab.com/ee/user/profile/personal_access_tokens.html

Put this script in the folder you want to generate the CSVs.

Run: python gitlab.py <TOKEN>
"""

from __future__ import annotations

import csv
import sys
from dataclasses import dataclass
from typing import Any

import requests

TOKEN = sys.argv[1]
AUTH = ("Bearer", TOKEN)
HEADERS = {"Authorization": "Bearer " + TOKEN}
REPOS_URL = "https://bbpgitlab.epfl.ch/api/v4/projects"
REPOS_PARAMS = [
    ("order_by", "name"),
    ("sort", "asc"),
    ("per_page", "100"),
]
COMMITS_URL = "https://bbpgitlab.epfl.ch/api/v4/projects/{repo}/repository/commits"
COMMITS_PARAMS = [
    ("per_page", "100"),
]
COMMIT_URL = "https://bbpgitlab.epfl.ch/api/v4/projects/{repo}/repository/commits/{id}"
HEADER = ["Author", "Date", "Additions", "Deletions"]
UNKNOWN = "???"


@dataclass
class Repo:

    id: int
    name: str


@dataclass(order=True)
class Commit:

    author: str
    date: str
    additions: int
    deletions: int


def deserialize_repo(repo: dict[str, Any]) -> Repo:
    return Repo(
        id=repo["id"],
        name=repo["name"],
    )


def get_repos() -> list[Repo]:
    repos = list[Repo]()
    i = 1
    while True:
        params = REPOS_PARAMS + [("page", str(i))]
        reply = requests.get(REPOS_URL, params, headers=HEADERS)
        page = reply.json()
        if not page:
            return repos
        repos.extend(deserialize_repo(repo) for repo in page)
        i += 1


def get_commit_ids(repo: int) -> list[str]:
    ids = list[str]()
    i = 1
    while True:
        url = COMMITS_URL.format(repo=repo)
        params = COMMITS_PARAMS + [("page", str(i))]
        reply = requests.get(url, params, headers=HEADERS)
        page = reply.json()
        if not page:
            return ids
        ids.extend(commit["id"] for commit in page)
        i += 1


def deserialize_commit(data: dict[str, Any]) -> Commit:
    date: str = data["authored_date"]
    stats: dict[str, Any] = data["stats"]
    return Commit(
        author=data["author_name"],
        date=date.split("T")[0],
        additions=stats["additions"],
        deletions=stats["deletions"],
    )


def get_commit(repo: int, id: str) -> Commit:
    url = COMMIT_URL.format(repo=repo, id=id)
    reply = requests.get(url, headers=HEADERS)
    commit = reply.json()
    return deserialize_commit(commit)


def get_commits(repo: int) -> list[Commit]:
    return sorted(get_commit(repo, id) for id in get_commit_ids(repo))


def serialize_commit(commit: Commit) -> list[str]:
    return [
        commit.author,
        commit.date,
        str(commit.additions),
        str(commit.deletions),
    ]


def save_csv(commits: list[Commit], filename: str) -> None:
    with open(filename, "wt") as file:
        writer = csv.writer(file)
        writer.writerow(HEADER)
        writer.writerows(serialize_commit(commit) for commit in commits)


def run(repo: Repo) -> None:
    commits = get_commits(repo.id)
    save_csv(commits, f"{repo.name}.csv")


def main() -> None:
    for repo in get_repos():
        run(repo)


if __name__ == "__main__":
    main()
