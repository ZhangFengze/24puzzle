FROM crazyz/gcc-cmake-python:latest
COPY . /usr/src/app
WORKDIR /usr/src/app
EXPOSE 9000
RUN python src/build.py
CMD python bin/release/distributed_solver_worker.py
