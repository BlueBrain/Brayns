from typing import List

from .params import Params, Property


def get_declarations(params: Params) -> List[str]:
    return [
        get_declaration(property)
        for property in _get_properties(params)
    ]


def get_declaration(property: Property) -> str:
    name = property.name
    typename = property.typename
    default = '' if property.required else ' = None'
    return f'{name}: {typename}{default}'


def get_descriptions(params: Params) -> List[str]:
    return [
        get_description(property)
        for property in _get_properties(params)
    ]


def get_description(property: Property) -> str:
    name = property.name
    typename = property.typename
    optional = '' if property.required else ' (optional)'
    description = property.description
    return f'{name}: {typename}{optional} -- {description}.'


def _get_properties(params: Params) -> List[Property]:
    return [
        property
        for property in _get_sorted_properties(params)
        if not property.read_only
    ]


def _get_sorted_properties(params: Params) -> List[Property]:
    return sorted(
        params.properties,
        key=lambda property: (not property.required, property.name)
    )
