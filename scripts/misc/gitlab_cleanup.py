import subprocess

PATH = "/home/acfleury/source/repos/Brayns-1"


def execute(cmd: str) -> str:
    result = subprocess.run(
        cmd.split(),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        cwd=PATH,
    )
    return result.stdout.decode()


output = execute("git branch -r")
branches = [branch.replace("origin/", "") for branch in output.split()]

for branch in branches:
    if branch in ["develop", "master", "HEAD"]:
        continue
    execute(f"git push origin --delete {branch}")
