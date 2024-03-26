import re
from pathlib import Path

INCLUDE = re.compile(r'\s*#\s*include\s*[<"](.*?)[>"]')

DYNAMIC_PLUGINS = {
    "AtlasExplorerPlugin",
    "CircuitExplorerPlugin",
}

CURRENT = Path(__file__).parent

ROOT = CURRENT.parent.parent

APPS = ROOT / "apps"

BRAYNS = ROOT / "brayns"

PLUGINS = ROOT / "plugins"

TESTS = ROOT / "tests"


def get_headers(folder: Path) -> list[Path]:
    return list(folder.glob("**/*.h"))


def get_all_headers(folders: list[Path]) -> list[Path]:
    return [header for folder in folders for header in get_headers(folder)]


def get_header_names(folders: list[Path]) -> set[str]:
    return {header.stem for header in get_all_headers(folders)}


def get_directories(folder: Path) -> list[Path]:
    return [path for path in folder.glob("*") if path.is_dir()]


def get_plugin_directories(folder: Path) -> list[Path]:
    return [
        directory for directory in get_directories(folder) if directory.name != "deps"
    ]


def get_sources(folder: Path) -> list[Path]:
    return [path for path in folder.glob("**/*.*") if path.suffix in [".h", ".cpp"]]


def get_all_sources(folders: list[Path]) -> list[Path]:
    return [source for folder in folders for source in get_sources(folder)]


def get_includes(source: Path) -> list[str]:
    with source.open("r") as file:
        code = file.read()
        return [match[1] for match in INCLUDE.finditer(code)]


def get_include_name(include: str) -> str:
    return include.split("/")[-1].replace(".h", "")


def get_dependencies(source: Path) -> list[str]:
    return [
        name
        for name in [get_include_name(include) for include in get_includes(source)]
        if name != source.stem
    ]


def get_usage(folders: list[Path]) -> dict[str, int]:
    header_names = get_header_names(folders)
    usage = {name: 0 for name in header_names}
    for source in get_all_sources(folders):
        dependencies = get_dependencies(source)
        for dependency in dependencies:
            if dependency not in header_names:
                continue
            usage[dependency] += 1
    return usage


def find_unused(usage: dict[str, int]) -> list[str]:
    return sorted(
        name
        for name, count in usage.items()
        if count == 0 and name not in DYNAMIC_PLUGINS
    )


def get_unused_files() -> list[str]:
    folders = [APPS, BRAYNS, *get_plugin_directories(PLUGINS), TESTS]
    usage = get_usage(folders)
    return find_unused(usage)


def main():
    unused = get_unused_files()
    if not unused:
        print("No unused headers have been found")
        return
    print("Unused header files have been found")
    print("\n".join(unused))
    exit(1)


if __name__ == "__main__":
    main()
