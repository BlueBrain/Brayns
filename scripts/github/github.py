"""
USAGE:

Create a personal Github access token as described here https://docs.github.com/en/enterprise-server@3.4/authentication/keeping-your-account-and-data-secure/creating-a-personal-access-token

Put this script in the folder you want to generate the CSVs.

Run: python github.py <TOKEN>
"""

from __future__ import annotations

import csv
import sys
from dataclasses import dataclass
from typing import Any

import requests

TOKEN = sys.argv[1]
AUTH = ("Bearer", TOKEN)
REPOS_URL = "https://api.github.com/orgs/BlueBrain/repos"
REPOS_PARAMS = [("type", "all"), ("sort", "full_name"), ("per_page", 100)]
COMMITS_URL = "https://api.github.com/repos/BlueBrain/{repo}/commits"
COMMITS_PARAMS = [("per_page", 100)]
HEADER = ["Author ID", "Author Login", "Author Name", "Date", "Additions", "Deletions"]
UNKNOWN = "???"


@dataclass(order=True)
class Commit:

    author_id: str
    author_login: str
    author_name: str
    date: str
    additions: int
    deletions: int


def get_repos() -> list[str]:
    repos = list[str]()
    i = 1
    while True:
        params = REPOS_PARAMS + [("page", str(i))]
        reply = requests.get(REPOS_URL, params, auth=AUTH)
        page = reply.json()
        if not page:
            return repos
        repos.extend(item["name"] for item in page)
        i += 1


def get_commit_urls(repo: str) -> list[str]:
    urls = list[str]()
    i = 1
    while True:
        params = COMMITS_PARAMS + [("page", str(i))]
        url = COMMITS_URL.format(repo=repo)
        reply = requests.get(url, params, auth=AUTH)
        page = reply.json()
        if not page:
            return urls
        urls.extend(commit["url"] for commit in page)
        i += 1


def deserialize_commit(data: dict[str, Any]) -> Commit:
    author: dict[str, Any] | None = data["author"]
    commit: dict[str, Any] = data["commit"]
    commit_author: dict[str, Any] | None = commit.get("author")
    stats: dict[str, Any] = data["stats"]
    author_id = UNKNOWN
    author_login = UNKNOWN
    author_name = UNKNOWN
    date = UNKNOWN
    if author is not None:
        author_id = str(author["id"])
        author_login = author["login"]
    if commit_author is not None:
        author_name = commit_author.get("name", UNKNOWN)
        date = commit_author.get("date", UNKNOWN)
    return Commit(
        author_id=author_id,
        author_login=author_login,
        author_name=author_name,
        date=date.split("T")[0],
        additions=stats["additions"],
        deletions=stats["deletions"],
    )


def get_commit(url: str) -> Commit:
    reply = requests.get(url, auth=AUTH)
    commit = reply.json()
    return deserialize_commit(commit)


def get_commits(repo: str) -> list[Commit]:
    return sorted(get_commit(url) for url in get_commit_urls(repo))


def serialize_commit(commit: Commit) -> list[str]:
    return [
        commit.author_id,
        commit.author_login,
        commit.author_name,
        commit.date,
        str(commit.additions),
        str(commit.deletions),
    ]


def save_csv(commits: list[Commit], filename: str) -> None:
    with open(filename, "wt") as file:
        writer = csv.writer(file)
        writer.writerow(HEADER)
        writer.writerows(serialize_commit(commit) for commit in commits)


def run(repo: str) -> None:
    commits = get_commits(repo)
    save_csv(commits, f"{repo}.csv")


def main() -> None:
    for repo in get_repos():
        run(repo)


if __name__ == "__main__":
    main()
