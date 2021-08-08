FROM crazyz/gcc-cmake-python:latest
COPY . /usr/src/app
WORKDIR /usr/src/app
EXPOSE 9000
run python test/build.py
