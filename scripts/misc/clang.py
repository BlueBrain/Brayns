import os
import pathlib

DIRECTORY = "/home/acfleury/source/repos/Brayns"
CHILDREN = [
    "apps",
    "brayns",
    "plugins/AtlasExplorer",
    "plugins/CircuitExplorer",
    "plugins/CylindricCamera",
    "plugins/DTI",
    "plugins/MoleculeExplorer",
    "tests",
]
CLANG_FORMAT = "clang-format-15"

for child in CHILDREN:
    count = 0
    source = pathlib.Path(DIRECTORY) / child
    for path in source.glob("**/*"):
        if path.suffix not in {".h", ".cpp"}:
            continue
        count += 1
        print(path)
        os.system(f"{CLANG_FORMAT} -i --style=file {path}")
    print(f"{count} files updated in {source}")
