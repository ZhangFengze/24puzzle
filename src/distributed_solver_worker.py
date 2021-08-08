from http.server import BaseHTTPRequestHandler, HTTPServer
import json
import solver


hostName = ""
serverPort = 9000


def Solve(tasks):
    for task in tasks:
        if solver.Solvable(json.dumps(task["board"])):
            result = json.loads(solver.Solve(json.dumps(task)))
            if result != None:
                return result


class MyServer(BaseHTTPRequestHandler):
    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        tasks = json.loads(self.rfile.read(content_length))

        result = Solve(tasks)

        self.send_response(200)
        self.send_header("Content-type", "application/json")
        self.end_headers()

        self.wfile.write(json.dumps(result).encode("utf8"))


if __name__ == "__main__":
    server = HTTPServer((hostName, serverPort), MyServer)
    server.serve_forever()
