import subprocess
import itertools
from common import print_red

if __name__ == "__main__":
    projects = (
        "normal_solver",
        "multithreading_solver",
        "distributed_solver/aliyun_worker",
        "distributed_solver/scheduler",
    )

    configs = (
        "debug",
        "release"
    )

    for project, config in itertools.product(projects, configs):
        result = subprocess.call(
            f"python test/build.py {project} --config {config}", shell=True)
        if result != 0:
            print_red(f"build {project} {config} failed!")
