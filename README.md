# 24puzzle
24-puzzles fast solver, solve 5x5 puzzle in seconds using cloud function compute

## How to build and deploy

### Deploy workers
``` shell
# for aliyun
# edit template.yml first
# login docker with your aliyun container service account
fun build --use-docker
fun deploy --push-registry acr-internet

# or deploy docker container by your self
# Dockerfile is under the top directory

# or deploy workers without docker
# to build and run worker:
python src/build.py
python bin/release/distributed_solver_worker.py
```

### Run solvers
``` shell
# make sure all dependencies (C++20 compiler, cmake, python sdk) are installed
# or use docker
docker pull crazyz/gcc-cmake-python # docker image that has all dependencies installed
docker run -it --rm -v `pwd`:/usr/src/myapp -w /usr/src/myapp crazyz/gcc-cmake-python # inside docker now

# build
python src/build.py

# test
# edit config.json first
cat config.json | python bin/release/test.py

# for details, ref src/test.py
```
