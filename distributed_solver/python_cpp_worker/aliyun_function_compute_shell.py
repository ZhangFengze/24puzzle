# -*- coding: utf-8 -*-
import os
import logging
import ctypes


dir = os.path.dirname(os.path.realpath(__file__))
libPath = os.path.join(dir, "libworker.so")
lib = ctypes.cdll.LoadLibrary(libPath)


def Solve(task):
    bufferLength = 1024
    buffer = ctypes.create_string_buffer(bufferLength)
    lib.Solve(task, buffer, bufferLength)
    return buffer.value


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
