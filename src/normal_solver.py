import sys
import json
import solver


def Solve(task):
    if not solver.Solvable(json.dumps(task["board"])):
        return

    for maxSteps in range(task["maxSteps"]+1):
        curTask = {"board": task["board"],
                   "steps": task["steps"],
                   "maxSteps": maxSteps}
        result = json.loads(solver.Solve(json.dumps(curTask)))
        if result != None:
            return result


if __name__ == "__main__":
    task = json.loads(sys.stdin.read())
    result = Solve(task)
    print(json.dumps(result))
