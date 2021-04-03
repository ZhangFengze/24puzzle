import ctypes
from http.server import *

Solve = ctypes.cdll.LoadLibrary("./worker.dll").Solve
Solve.restype = ctypes.c_char_p


class Handler(BaseHTTPRequestHandler):
    def do_POST(self):
        self.log_request()

        length = int(self.headers.get("content-length"))
        task = self.rfile.read(length)
        result = Solve(task)

        self.send_response(200)
        self.send_header('Content-type', 'text/plain')
        self.end_headers()
        self.wfile.write(result)

        self.log_message(result.decode("utf-8"))


def run(server_class=HTTPServer, handler_class=Handler):
    server_address = ('', 8000)
    httpd = server_class(server_address, handler_class)
    httpd.serve_forever()


run()
