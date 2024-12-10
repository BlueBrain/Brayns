from __future__ import annotations

import csv
from collections import defaultdict
from collections.abc import Iterable
from dataclasses import dataclass
from datetime import date, datetime

from dateutil import rrule

FILE = "Adrien.csv"
ENCODING = "latin-1"

OUTPUT1 = "output1.csv"
OUTPUT2 = "output2.csv"

MATRICULE = 0
BEGIN = 1
END = 2
OUT = 6
FUNDER = 8

FORMAT1 = "%Y-%m-%d"
FORMAT2 = "%d.%m.%y"
FORMAT3 = "%Y-%m"

HEADER1 = ["Matricule", "Begin", "End"]
HEADER2 = ["Funder", "Person", "Date"]

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


def write_csv(rows: list[list[str]], filename: str) -> None:
    with open(filename, "wt", newline="") as file:
        writer = csv.writer(file)
        writer.writerows(rows)


def parse_date(value: str, format: str) -> date:
    return datetime.strptime(value, format).date()


def parse_begin(value: str) -> date:
    return parse_date(value, FORMAT1)


def parse_end(value: str) -> date:
    return date.today() if value == TODAY else parse_date(value, FORMAT1)


def parse_out(value: str) -> date | None:
    return parse_date(value, FORMAT2) if value else None


def parse_person(row: list[str]) -> Person:
    return Person(
        matricule=row[MATRICULE],
        begin=parse_begin(row[BEGIN]),
        end=parse_end(row[END]),
        out=parse_out(row[OUT]),
        funder=row[FUNDER],
    )


def parse_people(reader: Iterable[list[str]]) -> list[Person]:
    i = iter(reader)
    next(i)
    return [parse_person(row) for row in i]


def read_people(filename: str, encoding: str) -> list[Person]:
    with open(filename, newline="", encoding=encoding) as file:
        reader = csv.reader(file)
        return parse_people(reader)


def merge_people(people: list[Person]) -> list[Person]:
    by_matricule = defaultdict[str, list[Person]](list)
    for person in people:
        by_matricule[person.matricule].append(person)
    return [
        Person(
            matricule=matricule,
            begin=min(person.begin for person in rows),
            end=max(person.end for person in rows),
        )
        for matricule, rows in by_matricule.items()
    ]


def serialize_date(value: date) -> str:
    return value.strftime(FORMAT1)


def serialize_end(end: date) -> str:
    return "=today()" if end >= date.today() else serialize_date(end)


def serialize_people(people: list[Person]) -> list[list[str]]:
    return [HEADER1] + [
        [person.matricule, serialize_date(person.begin), serialize_end(person.end)]
        for person in people
    ]


def get_worktime(person: Person) -> list[date]:
    return list(rrule.rrule(rrule.MONTHLY, person.begin, until=person.effective_end))


def split_worktime(people: list[Person]) -> dict[str, dict[str, list[date]]]:
    factory = lambda: defaultdict[str, list[date]](list)
    result = defaultdict[str, dict[str, list[date]]](factory)
    for person in people:
        dates = get_worktime(person)
        result[person.funder][person.matricule].extend(dates)
    return result


def serialize_dates(dates: dict[str, dict[str, list[date]]]) -> list[list[str]]:
    return [HEADER2] + [
        [funder, matricule, month.strftime(FORMAT3)]
        for funder, by_matricule in dates.items()
        for matricule, months in by_matricule.items()
        for month in months
    ]


def main1() -> None:
    people = read_people(FILE, ENCODING)
    people = merge_people(people)
    rows = serialize_people(people)
    write_csv(rows, OUTPUT1)


def main2() -> None:
    people = read_people(FILE, ENCODING)
    dates = split_worktime(people)
    rows = serialize_dates(dates)
    write_csv(rows, OUTPUT2)


if __name__ == "__main__":
    main2()
