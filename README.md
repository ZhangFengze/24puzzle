# 24puzzle
24-puzzles fast solver, solve 5x5 puzzle in seconds using cloud function compute

## How to build and deploy

``` shell

# prepare repo
git clone https://github.com/ZhangFengze/24puzzle.git
cd 24puzzle
git submodule update --init --recursive

# prepare docker
docker pull crazyz/gcc-cmake-python # docker image that has all dependencies installed
docker run -it --rm -v `pwd`:/usr/src/myapp -w /usr/src/myapp crazyz/gcc-cmake-python # inside docker now

# build
python src/build.py

# test
# edit config.json first
cat config.json | python bin/release/test.py

# deploy aliyun
# edit template.yml first
# login docker
fun build --use-docker
fun deploy --push-registry acr-internet
```

**Note** docker is not necessary if you have dependencies installed locally
