import pathlib

import brayns

package = ''

source = getattr(brayns, package) if package else brayns

names = dir(source)

names = [name for name in names if not name.startswith('__')]

PATH = f'/home/acfleury/Source/repos/Brayns/python/brayns'

path = pathlib.Path(PATH) / package

packages = [
    child.stem
    for child in path.glob('*')
    if not child.name.startswith('__')
]

names = [name for name in names if name not in packages]

names = [f'    \'{name}\',' for name in names]

print('__all__ = [\n' + '\n'.join(names) + '\n]')
