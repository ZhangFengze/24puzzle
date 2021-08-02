import os
import sys
import aiohttp
import asyncio
import json
import func


class Queue:
    queue = []
    index = 0

    def __init__(self, task: str, preferredCount: int):
        self.tasks = func.GenerateTasks(task, preferredCount)

    def Dequeue(self):
        if not self.queue:
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
    done, _ = await asyncio.wait(workers, return_when=asyncio.FIRST_COMPLETED)
    print(done.pop().result())


if __name__ == "__main__":
    config = json.loads(sys.stdin.read())
    if os.name == "nt":
        asyncio.set_event_loop_policy(asyncio.WindowsSelectorEventLoopPolicy())
    asyncio.run(main(config["url"], config["concurrency"],
                     config["taskPreferredCount"], json.dumps(config["task"])))
