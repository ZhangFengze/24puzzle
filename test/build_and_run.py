import argparse
import subprocess
import itertools
from common import get_root_path, rmdir, cmake, build, install


parser = argparse.ArgumentParser()
parser.add_argument("--clean", help="clean before build", action="store_true")
parser.add_argument("--config", help="configuration", default="debug")
parser.add_argument("--ninja", help="build using ninja", action="store_true")
args = parser.parse_args()

source_dir = get_root_path()
build_dir = get_root_path().joinpath("build")
install_dir = get_root_path().joinpath(f"build/{args.config}")

if args.clean:
    rmdir(build_dir)
    exit()

cmake(source_dir, build_dir, args.ninja, install_dir)
build(build_dir, args.config)
install(build_dir, args.config)

solvers = ("normal_solver", "multithreading_solver")
tests = ("0.json",)
for solver, test in itertools.product(solvers, tests):
    solver = install_dir.joinpath("bin").joinpath(solver)
    test = get_root_path().joinpath("test/cases").joinpath(test)
    with open(test) as input:
        assert(0 == subprocess.call(solver, stdin=input))
