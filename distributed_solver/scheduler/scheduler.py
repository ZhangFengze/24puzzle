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
        self.maxIndex = len(self.tasks)*task["depth"]

    def Dequeue(self):
        if not self.queue:
            if self.index >= self.maxIndex:
                return None
            depth = self.index // len(self.tasks)
            index = self.index % len(self.tasks)
            self.index = self.index+1

            return {
                "board": self.tasks[index]["board"],
                "steps": self.tasks[index]["steps"],
                "depth": depth
            }
        else:
            return self.queue.pop(0)

    def Enqueue(self, task: str):
        self.queue.append(task)


async def worker(queue: Queue, url: str):
    while True:
        task = queue.Dequeue()
        if not task:
            return
        async with aiohttp.ClientSession() as session:
            async with session.post(url, json=task) as resp:
                result = await resp.text()
                if result != "null":
                    return result
                # TODO error


async def main(url, concurrency, taskPreferredCount, task):
    queue = Queue(task, taskPreferredCount)
    workers = [asyncio.create_task(worker(queue, url))
               for _ in range(concurrency)]
    for coro in asyncio.as_completed(workers):
        earliest = await coro
        if earliest:
            print(earliest)
            return
    print("null")


if __name__ == "__main__":
    config = json.loads(sys.stdin.read())
    if os.name == "nt":
        asyncio.set_event_loop_policy(asyncio.WindowsSelectorEventLoopPolicy())
    asyncio.run(main(config["url"], config["concurrency"],
                     config["taskPreferredCount"], config["task"]))
