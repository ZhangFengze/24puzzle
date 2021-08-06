import os
import sys
import aiohttp
import asyncio
import json
import itertools
from math import ceil
import func


def split(data, parts):
    if len(data)==0:
        return data
    span = ceil(len(data)/parts)
    return [data[x:x+span] for x in range(0, len(data), span)]


async def solve(tasks, url: str):
    async with aiohttp.ClientSession() as session:
        async with session.post(url, json=tasks) as resp:
            if resp.status == 200:
                result = await resp.text()
                return json.loads(result)


async def main(url, concurrency, taskPreferredCount, task):
    tasks = func.GenerateTasks(json.dumps(task), taskPreferredCount)
    tasks = itertools.product(tasks, range(task["maxSteps"]))
    tasks = [{"board": task["board"], "steps":task["steps"], "maxSteps":maxSteps}
             for task, maxSteps in tasks]
    groupedTasks = split(list(tasks), concurrency)
    asyncTasks = [asyncio.create_task(solve(tasks, url))
                  for tasks in groupedTasks]
    for asyncTask in asyncio.as_completed(asyncTasks):
        result = await asyncTask
        if result != None:
            return result


if __name__ == "__main__":
    config = json.loads(sys.stdin.read())
    if os.name == "nt":
        asyncio.set_event_loop_policy(asyncio.WindowsSelectorEventLoopPolicy())
    result = asyncio.run(main(config["url"], config["concurrency"],
                              config["taskPreferredCount"], config["task"]))
    print(json.dumps(result))
