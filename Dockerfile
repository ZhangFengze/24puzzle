FROM crazyz/gcc-cmake-python:latest
COPY . /usr/src/app
WORKDIR /usr/src/app
EXPOSE 9000
RUN python test/build.py
