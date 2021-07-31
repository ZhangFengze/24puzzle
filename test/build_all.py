import subprocess
import itertools
from common import print_red

projects = (
    "normal_solver",
    "multithreading_solver",
    "distributed_solver/aliyun/pure_cpp_worker",
    "distributed_solver/aliyun/python_cpp_worker",
    "distributed_solver/scheduler",
    "distributed_solver/scheduler2",
)

configs = (
    "debug",
    "release"
)

for project, config in itertools.product(projects, configs):
    result = subprocess.call(
        f"python3 test/build.py {project} --config {config}", shell=True)
    if result != 0:
        print_red(f"build {project} {config} failed!")
