from __future__ import annotations

import json
import sys
from argparse import ArgumentParser
from collections.abc import Callable, Mapping
from configparser import ConfigParser, SectionProxy
from functools import partial
from typing import Any, TypeVar

import brayns

T = TypeVar('T')


class Parser:

    def __init__(self) -> None:
        self._parsers = dict[type, Callable[[str], Any]]()

    def add_parser(self, value_type: type[T], parser: Callable[[str], T]) -> None:
        self._parsers[value_type] = parser

    def parse_value(self, value: str, value_type: type[T]) -> T:
        parser = self._parsers.get(value_type)
        if parser is None:
            return value_type(value)
        return parser(value)

    def parse(self, data: Mapping[str, str], value: Any) -> None:
        for key, default in vars(value).items():
            config = data.get(key)
            if config is None:
                continue
            attribute = self.parse_value(config, type(default))
            setattr(value, key, attribute)


def parse_boolean(value: str) -> bool:
    value = value.lower()
    if value in ('true', 'yes', 'on', '1'):
        return True
    if value in ('false', 'no', 'off', '0'):
        return False
    raise ValueError(f'Invalid boolean value: {value}')


def parse_list(value: str, item_type: type[T]) -> list[T]:
    items = json.loads(value)
    if not isinstance(items, list):
        raise ValueError(f'Expected a list of values for {value}')
    return [item_type(item) for item in items]


def parse_rotation(value: str) -> brayns.Rotation:
    return brayns.euler(*parse_list(value, float), degrees=True)


def parse_resolution(value: str) -> brayns.Resolution:
    return brayns.Resolution(*parse_list(value, int))


def parse_color4(value: str) -> brayns.Color4:
    return brayns.Color4(*parse_list(value, float))


def parse_vector3(value: str) -> brayns.Vector3:
    return brayns.Vector3(*parse_list(value, float))


def default_parser() -> Parser:
    parser = Parser()
    parser.add_parser(bool, parse_boolean)
    parser.add_parser(brayns.Rotation, parse_rotation)
    parser.add_parser(brayns.Resolution, parse_resolution)
    parser.add_parser(brayns.Color4, parse_color4)
    parser.add_parser(brayns.Vector3, parse_vector3)
    return parser


def read_section(filename: str, name: str) -> SectionProxy:
    cfg = ConfigParser()
    cfg.read(filename)
    return cfg[name]


def check_cfg_section(section: SectionProxy, mandatory: list[str]) -> None:
    for key in mandatory:
        if key not in section:
            raise ValueError(f'Missing mandatory parameters: "{key}"')


def create_argv_parser(value: Any, mandatory: set[str], parser: Parser) -> ArgumentParser:
    result = ArgumentParser()
    for key, default in vars(value).items():
        result.add_argument(
            f'--{key}',
            type=partial(parser.parse_value, value_type=type(default)),
            required=key in mandatory,
            default=default,
        )
    return result


def update_from_argv(value: Any, mandatory: set[str], parser: Parser) -> None:
    argv = create_argv_parser(value, mandatory, parser)
    argv.parse_args(namespace=value)


def check_argv(mandatory: list[str]) -> None:
    argv = set(sys.argv[1:])
    for key in mandatory:
        if f'--{key}' not in argv:
            raise ValueError(f'Missing mandatory parameters: "{key}"')


def check_cfg(filename: str, section: str, mandatory: list[str]) -> None:
    data = read_section(filename, section)
    check_cfg_section(data, mandatory)


def cfg_to_argv(filename: str, section: str = 'BRAYNS') -> list[str]:
    data = read_section(filename, section)
    return [
        arg
        for key, value in data.items()
        for arg in (f'--{key}', value)
    ]


def parse_argv(
    value_type: type[T],
    mandatory: list[str] | None = None,
    parser: Parser | None = None,
) -> T:
    value = value_type()
    required = set(mandatory if mandatory else [])
    parser = default_parser() if parser is None else parser
    update_from_argv(value, required, parser)
    return value


def parse_cfg(
    filename: str,
    value_type: type[T],
    section: str = 'BRAYNS',
    mandatory: list[str] | None = None,
    parser: Parser | None = None,
) -> T:
    value = value_type()
    data = read_section(filename, section)
    required = [] if mandatory is None else mandatory
    check_cfg_section(data, required)
    parser = default_parser() if parser is None else parser
    parser.parse(data, value)
    return value
