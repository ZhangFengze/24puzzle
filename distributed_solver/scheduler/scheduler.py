import os
import sys
import aiohttp
import asyncio
import json
import func


class Queue:
    queue = []
    index = 0
    maxIndex = 0

    def __init__(self, task, preferredCount):
        self.tasks = func.GenerateTasks(json.dumps(task), preferredCount)
        self.maxIndex = len(self.tasks)*(task["maxSteps"]+1)

    def Dequeue(self):
        if not self.queue:
            if self.index >= self.maxIndex:
                return None
            maxSteps = self.index // len(self.tasks)
            index = self.index % len(self.tasks)
            self.index = self.index+1

            return ({
                "board": self.tasks[index]["board"],
                "steps": self.tasks[index]["steps"],
                "maxSteps": maxSteps
            }, )
        else:
            return self.queue.pop(0)

    def Enqueue(self, task: str):
        self.queue.append(task)


async def worker(queue: Queue, url: str, exit: asyncio.Event):
    while not exit.is_set():
        task = queue.Dequeue()
        if not task:
            return
        async with aiohttp.ClientSession() as session:
            async with session.post(url, json=task) as resp:
                if resp.status != 200:
                    queue.Enqueue(task)
                else:
                    result = await resp.text()
                    result = json.loads(result)
                    if result != None:
                        exit.set()
                        return result


async def main(url, concurrency, taskPreferredCount, task):
    exit = asyncio.Event()
    queue = Queue(task, taskPreferredCount)
    workers = [asyncio.create_task(worker(queue, url, exit))
               for _ in range(concurrency)]
    results = await asyncio.gather(*workers)
    results = list(filter(lambda x: x != None, results))
    if not results:
        return json.dumps(None)
    else:
        return json.dumps(min(results, key=lambda x: len(x)))


if __name__ == "__main__":
    config = json.loads(sys.stdin.read())
    if os.name == "nt":
        asyncio.set_event_loop_policy(asyncio.WindowsSelectorEventLoopPolicy())
    result = asyncio.run(main(config["url"], config["concurrency"],
                              config["taskPreferredCount"], config["task"]))
    print(result)
