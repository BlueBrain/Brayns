from __future__ import annotations

import csv
from collections import defaultdict
from dataclasses import dataclass
from datetime import date, datetime

PEOPLE = "ExcelPeople.csv"
COMMITS = "ExcelCommits.csv"
ENCODING = "latin-1"

OUTPUT = "output.csv"

MATRICULE = 0
BEGIN = 1
END = 2
OUT = 6
FUNDER = 8

REPOSITORY = 1
DATE = 2
COMMITTER = 3
PLUS = 4
MINUS = 5

FULL = "%Y-%m-%d"
QUICK = "%d.%m.%y"
MONTH = "%Y-%m"
ABBREVIATED = "%d.%b.%y"

HEADER = ["Month", "Funder", "Person", "Repository", "Commits", "Plus", "Minus"]

TODAY = "1999-12-31"


@dataclass
class Person:

    matricule: str
    begin: date
    end: date
    out: date | None = None
    funder: str = ""

    @property
    def effective_end(self) -> date:
        return self.end if self.out is None else self.out


@dataclass
class Commit:

    repository: str
    timestamp: date
    committer: str
    plus: int
    minus: int


@dataclass(frozen=True)
class CommitEntry:

    month: date
    committer: str
    repository: str


@dataclass
class CommitInfo:

    commits: int = 0
    plus: int = 0
    minus: int = 0


@dataclass
class Record:

    month: date
    funder: str
    person: str
    repository: str
    commits: int
    plus: int
    minus: int


def read_csv(filename: str) -> list[list[str]]:
    with open(filename, newline="") as file:
        reader = csv.reader(file)
        return list(reader)[1:]


def parse_date(value: str, format: str = FULL) -> date:
    return datetime.strptime(value, format).date()


def parse_end(value: str) -> date:
    return date.today() if value == TODAY else parse_date(value, FULL)


def parse_out(value: str) -> date | None:
    return parse_date(value, QUICK) if value else None


def parse_person(row: list[str]) -> Person:
    return Person(
        matricule=row[MATRICULE],
        begin=parse_date(row[BEGIN]),
        end=parse_end(row[END]),
        out=parse_out(row[OUT]),
        funder=row[FUNDER],
    )


def parse_commit(row: list[str]) -> Commit:
    return Commit(
        repository=row[REPOSITORY],
        timestamp=parse_date(row[DATE], ABBREVIATED),
        committer=row[COMMITTER],
        plus=int(row[PLUS]),
        minus=int(row[MINUS]),
    )


def get_entry(commit: Commit) -> CommitEntry:
    return CommitEntry(
        month=commit.timestamp.replace(day=1),
        committer=commit.committer,
        repository=commit.repository,
    )


def group_commits(commits: list[Commit]) -> dict[CommitEntry, CommitInfo]:
    result = defaultdict[CommitEntry, CommitInfo](CommitInfo)
    for commit in commits:
        entry = get_entry(commit)
        info = result[entry]
        info.commits += 1
        info.plus += commit.plus
        info.minus += commit.minus
    return result


def get_funder(committer: str, month: date, people: list[Person]) -> str:
    for person in people:
        if person.matricule != committer:
            continue
        begin, end = person.begin, person.effective_end
        if month < begin or month > end:
            continue
        return person.funder
    return ""


def build_records(
    infos: dict[CommitEntry, CommitInfo], people: list[Person]
) -> list[Record]:
    return [
        Record(
            month=entry.month,
            funder=get_funder(entry.committer, entry.month, people),
            person=entry.committer,
            repository=entry.repository,
            commits=info.commits,
            plus=info.plus,
            minus=info.minus,
        )
        for entry, info in infos.items()
    ]


def serialize_records(infos: list[Record]) -> list[list[str]]:
    return [HEADER] + [
        [
            info.month.strftime(MONTH),
            info.funder,
            info.person,
            info.repository,
            str(info.commits),
            str(info.plus),
            str(info.minus),
        ]
        for info in infos
    ]


def write_csv(rows: list[list[str]], filename: str) -> None:
    with open(filename, "wt", newline="") as file:
        writer = csv.writer(file)
        writer.writerows(rows)


def main() -> None:
    rows = read_csv(PEOPLE)
    people = [parse_person(row) for row in rows]
    rows = read_csv(COMMITS)
    commits = [parse_commit(row) for row in rows]
    infos = group_commits(commits)
    records = build_records(infos, people)
    rows = serialize_records(records)
    write_csv(rows, OUTPUT)


if __name__ == "__main__":
    main()
