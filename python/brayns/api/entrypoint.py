from . import params

from .params import Params


class Entrypoint:

    def __init__(
        self,
        name: str,
        description: str,
        params: Params
    ) -> None:
        self.name = name
        self.description = description
        self.params = params


def from_schema(schema: dict) -> Entrypoint:
    return Entrypoint(
        name=schema['title'],
        description=schema.get('description', ''),
        params=params.from_schema(schema.get('params', []))
    )
