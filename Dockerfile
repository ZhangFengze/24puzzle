FROM crazyz/gcc-cmake-python:latest
COPY . /usr/src/app
WORKDIR /usr/src/app
RUN python src/build.py

FROM python:3.9.2-slim
COPY --from=0 /usr/src/app/bin/release /usr/src/app
WORKDIR /usr/src/app
EXPOSE 9000
CMD python distributed_solver_worker.py
