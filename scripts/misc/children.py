from pathlib import Path

PATH = "/home/acfleury/source/test/test.swc"

path = Path(PATH)

with path.open() as file:
    lines = file.readlines()

print("".join(line for line in lines if line.endswith(" 1\n")))
