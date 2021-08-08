import pathlib
import glob
import subprocess
import json
import itertools
import sys
import timeit
import typing
from common import print_green, print_red


def Cases():
    cases = []
    curDir = str(pathlib.Path(__file__).resolve().parent)
    for casePath in glob.glob(f"{curDir}/cases/unittest/*.json"):
        with open(casePath, "r") as caseFile:
            cases.extend(json.loads(caseFile.read()))
    return cases


def GenerateSolver(cmd):
    def Solver(task):
        p = subprocess.Popen(cmd, shell=True,
                             stdin=subprocess.PIPE, stdout=subprocess.PIPE, encoding="utf8")
        out, err = p.communicate(input=json.dumps(task))
        return json.loads(out)
    return Solver


def GenerateDistributedSolver(binDir, url, concurrency, taskPreferredCount):
    cmd = f"python {binDir}/distributed_solver_scheduler.py"

    def Solver(task):
        config = {
            "url": url,
            "concurrency": concurrency,
            "taskPreferredCount": taskPreferredCount,
            "task": task
        }
        p = subprocess.Popen(cmd, shell=True,
                             stdin=subprocess.PIPE, stdout=subprocess.PIPE, encoding="utf8")
        out, err = p.communicate(input=json.dumps(config))
        return json.loads(out)
    return Solver


class Solver(typing.NamedTuple):
    name: str
    func: typing.Callable


if __name__ == "__main__":
    config = json.loads(sys.stdin.read())
    binDir = str(pathlib.Path(__file__).resolve().parent/".."/"bin"/"release")

    solvers = [
        Solver("normal_solver_cpp", GenerateSolver(f"{binDir}/normal_solver_cpp")),
        Solver("normal_solver_py", GenerateSolver(
            f"python {binDir}/normal_solver.py")),
        Solver("multithreading_solver_cpp", GenerateSolver(
            f"{binDir}/multithreading_solver_cpp")),
        Solver("multiprocessing_solver_py", GenerateSolver(
            f"python {binDir}/multiprocessing_solver.py")),
        Solver("multiprocessing_solver_py_2", GenerateSolver(
            f"python {binDir}/multiprocessing_solver2.py")),
        Solver("multiprocessing_solver_py_3", GenerateSolver(
            f"python {binDir}/multiprocessing_solver3.py")),
        Solver("multiprocessing_solver_py_4", GenerateSolver(
            f"python {binDir}/multiprocessing_solver4.py")),
        Solver("distributed_solver_scheduler", GenerateDistributedSolver(
            binDir, config["url"], config["concurrency"], config["taskPreferredCount"])),
    ]

    for solver, case in itertools.product(solvers, Cases()):

        start = timeit.default_timer()
        result = solver.func(case["task"])
        end = timeit.default_timer()

        steps = len(result) if result != None else -1

        if steps == case["expectSteps"]:
            print_green(solver.name, case, end-start, result, sep=", ")
        else:
            print_red(solver.name, case, end-start, result, sep=", ")
