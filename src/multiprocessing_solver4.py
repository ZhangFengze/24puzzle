import os
import sys
import json
import itertools
import multiprocessing
import solver


def GenerateTasks(task, preferredCount):
    tasks = json.loads(solver.GenerateTasks(json.dumps(task), preferredCount))
    tasks = itertools.product(range(task["maxSteps"]+1), tasks)
    for maxSteps, task in tasks:
        yield {"board": task["board"], "steps": task["steps"], "maxSteps": maxSteps}


def Worker(tasks, index, lock, done: multiprocessing.Queue):
    while True:
        with lock:
            curIndex = index.value
            index.value += 1
        if curIndex >= len(tasks):
            return
        task = tasks[curIndex]
        result = json.loads(solver.Solve(json.dumps(task)))
        done.put(result)


def Solve(task):
    if not solver.Solvable(json.dumps(task["board"])):
        return

    concurrency = os.cpu_count()
    with multiprocessing.Manager() as manager:
        done = manager.Queue()
        index = manager.Value("i", 0)
        lock = manager.Lock()
        tasks = list(GenerateTasks(task, concurrency*8))

        try:
            workers = [multiprocessing.Process(target=Worker, args=(tasks, index, lock, done))
                       for _ in range(concurrency)]
            for worker in workers:
                worker.start()

            for _ in range(len(tasks)):
                result = done.get()
                if result != None:
                    return result
        finally:
            for worker in workers:
                worker.terminate()


if __name__ == "__main__":
    task = json.loads(sys.stdin.read())
    result = Solve(task)
    print(json.dumps(result))
