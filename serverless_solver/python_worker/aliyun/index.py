# -*- coding: utf-8 -*-

import logging
import ctypes


lib = ctypes.cdll.LoadLibrary(
    "./libworker.so") or ctypes.cdll.LoadLibrary("./worker.dll")
Solve = lib.Solve
Solve.restype = ctypes.c_char_p


def get_body(environ):
    try:
        request_body_size = int(environ.get('CONTENT_LENGTH', 0))
    except (ValueError):
        request_body_size = 0
    return environ['wsgi.input'].read(request_body_size)


def handler(environ, start_response):
    task = get_body(environ)
    result = Solve(task)

    status = '200 OK'
    response_headers = [('Content-type', 'text/plain')]
    start_response(status, response_headers)

    return [result]
