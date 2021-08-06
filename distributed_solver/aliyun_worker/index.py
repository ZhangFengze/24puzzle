# -*- coding: utf-8 -*-
import json
import func


def Solve(tasks):
    for task in tasks:
        result = json.loads(func.Solve(json.dumps(task)))
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
