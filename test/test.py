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


def GenerateNormalSolver():
    curDir = str(pathlib.Path(__file__).resolve().parent)
    cmd = f"{curDir}/../bin/normal_solver/release/bin/normal_solver"

    def Solver(task):
        p = subprocess.Popen(cmd,
                             stdin=subprocess.PIPE, stdout=subprocess.PIPE, encoding="utf8")
        out, err = p.communicate(input=json.dumps(task))
        return json.loads(out)
    return Solver


def GenerateMultithreadingSolver():
    curDir = str(pathlib.Path(__file__).resolve().parent)
    cmd = f"{curDir}/../bin/multithreading_solver/release/bin/multithreading_solver"

    def Solver(task):
        p = subprocess.Popen(cmd,
                             stdin=subprocess.PIPE, stdout=subprocess.PIPE, encoding="utf8")
        out, err = p.communicate(input=json.dumps(task))
        return json.loads(out)
    return Solver


def GenerateDistributedSolver(url, concurrency, taskPreferredCount):
    curDir = str(pathlib.Path(__file__).resolve().parent)
    cmd = f"python {curDir}/../bin/distributed_solver/scheduler/release/bin/scheduler.py"

    def Solver(task):
        config = {
            "url": url,
            "concurrency": concurrency,
            "taskPreferredCount": taskPreferredCount,
            "task": task
        }
        p = subprocess.Popen(cmd,
                             stdin=subprocess.PIPE, stdout=subprocess.PIPE, encoding="utf8")
        out, err = p.communicate(input=json.dumps(config))
        return json.loads(out)
    return Solver


class Solver(typing.NamedTuple):
    name: str
    func: typing.Callable


if __name__ == "__main__":
    config = json.loads(sys.stdin.read())

    solvers = (Solver("normal", GenerateNormalSolver()),
               Solver("mt", GenerateMultithreadingSolver()),
               Solver("fc", GenerateDistributedSolver(
                   config["url"], config["concurrency"], config["taskPreferredCount"])))

    for solver, case in itertools.product(solvers, Cases()):

        start = timeit.default_timer()
        result = solver.func(case["task"])
        end = timeit.default_timer()

        steps = len(result) if result != None else -1

        if steps == case["expectSteps"]:
            print_green(solver.name, case, end-start, result, sep=",")
        else:
            print_red(solver.name, case, end-start, result, sep=",")
