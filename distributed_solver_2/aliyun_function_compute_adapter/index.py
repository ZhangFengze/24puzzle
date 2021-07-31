# -*- coding: utf-8 -*-
import requests


def get_body(environ):
    try:
        request_body_size = int(environ.get('CONTENT_LENGTH', 0))
    except (ValueError):
        request_body_size = 0
    return environ['wsgi.input'].read(request_body_size)


def handler(environ, start_response):
    task = get_body(environ)
    url = 'http://locahost:8080/'
    result = requests.post(url, data=task)

    status = '200 OK'
    response_headers = [('Content-type', 'application/json')]
    start_response(status, response_headers)

    return [result]
