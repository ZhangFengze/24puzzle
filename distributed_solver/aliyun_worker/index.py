# -*- coding: utf-8 -*-
import os
import ctypes
import json


dir = os.path.dirname(os.path.realpath(__file__))
libPath = os.path.join(dir, "libworker.so")
lib = ctypes.cdll.LoadLibrary(libPath)


def SolveOne(task):
    task = json.dumps(task).encode("utf8")
    bufferLength = 1024
    buffer = ctypes.create_string_buffer(bufferLength)
    lib.Solve(task, buffer, bufferLength)
    return json.loads(buffer.value)


def Solve(tasks):
    for task in tasks:
        result = SolveOne(task)
        if result != None:
            return result


def get_body(environ):
    try:
        request_body_size = int(environ.get('CONTENT_LENGTH', 0))
    except (ValueError):
        request_body_size = 0
    return environ['wsgi.input'].read(request_body_size)


def handler(environ, start_response):
    tasks = json.loads(get_body(environ))
    result = Solve(tasks)

    status = '200 OK'
    response_headers = [('Content-type', 'application/json')]
    start_response(status, response_headers)

    return [json.dumps(result).encode("utf8")]
