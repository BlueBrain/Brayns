def from_schema(schema: dict) -> str:
    typename = schema.get('type')
    if typename is not None:
        return _TYPES[typename].__name__
    return _from_oneof(schema)


_TYPES = {
    'boolean': bool,
    'integer': int,
    'number': float,
    'string': str,
    'array': list,
    'object': dict
}


def _from_oneof(schema: dict) -> str:
    typenames = ', '.join(
        from_schema(oneof)
        for oneof in schema['oneOf']
        if oneof.get('type') != 'null'
    )
    return f'Union[{typenames}]'
