from pathlib import Path
import shutil

FOLDER = Path(
    "/gpfs/bbp.cscs.ch/data/scratch/proj134/fullcircuit/full-synth-more-split-v2"
)

OUTPUT = Path(
    "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FullBrainSynthesized/morphologies"
)


def get_output(group: Path, subgroup: Path) -> Path:
    first = group.name.replace(".h5", "")
    second = subgroup.name
    return OUTPUT / f"{first}_{second}.h5"


def pick_one_morphology(subgroup: Path) -> Path:
    return next(subgroup.glob("*.h5"))


def get_subgroups(group: Path) -> list[Path]:
    root = group / "morphologies" / "hashed"
    return [subgroup for subgroup in root.iterdir() if subgroup.is_dir()]


def get_groups(folder: Path) -> list[Path]:
    return [
        group
        for group in folder.iterdir()
        if group.is_dir() and group.name.endswith(".h5")
    ]


def main() -> None:
    shutil.rmtree(OUTPUT, ignore_errors=True)
    OUTPUT.mkdir(parents=True, exist_ok=True)

    count = 0

    for group in get_groups(FOLDER):
        for subgroup in get_subgroups(group):
            morphology = pick_one_morphology(subgroup)
            output = get_output(group, subgroup)

            shutil.copyfile(morphology, output)

            count += 1

            print(count)


if __name__ == "__main__":
    main()
