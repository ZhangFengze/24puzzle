import sys
import copy
import json
import solver


def Solve(task):
    for maxSteps in range(task["maxSteps"]+1):
        curTask = copy.deepcopy(task)
        curTask["maxSteps"] = maxSteps
        result = json.loads(solver.Solve(json.dumps(curTask)))
        if result != None:
            return result


if __name__ == "__main__":
    task = json.loads(sys.stdin.read())
    result = Solve(task)
    print(json.dumps(result))
