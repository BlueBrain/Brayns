from dataclasses import dataclass, field
from typing import List

from . import properties

from .properties import Property


@dataclass
class Params:

    properties: List[Property] = field(default_factory=list)


def from_schema(schemas: List[dict]) -> Params:
    return _from_schema(schemas[0]) if schemas else Params()


def _from_schema(schema: dict) -> Params:
    return Params(properties.from_schema(schema))
