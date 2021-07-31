import os
import ctypes
from http.server import *

dir = os.path.dirname(os.path.realpath(__file__))
libPath = os.path.join(dir, "libworker.so")
lib = ctypes.cdll.LoadLibrary(libPath)


def Solve(task):
    bufferLength = 1024
    buffer = ctypes.create_string_buffer(bufferLength)
    lib.Solve(task, buffer, bufferLength)
    return buffer.value


class Handler(BaseHTTPRequestHandler):
    def do_POST(self):
        self.log_request()

        length = int(self.headers.get("content-length"))
        task = self.rfile.read(length)
        result = Solve(task)

        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()
        self.wfile.write(result)

        self.log_message(result.decode("utf-8"))


def run(server_class=HTTPServer, handler_class=Handler):
    server_address = ('', 8000)
    httpd = server_class(server_address, handler_class)
    httpd.serve_forever()


run()
