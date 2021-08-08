import os
import pathlib
import subprocess
import shutil
import io


def execute(cmd):
    print_green(f"executing: {cmd}")
    assert(0 == subprocess.call(cmd, shell=True))


def get_root_path():
    cur = pathlib.Path(__file__)
    return cur.absolute().parent.parent


class colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    _END = '\033[0m'


def print_colored(color, *args, **kwargs):
    with io.StringIO() as out:
        print(*args, file=out, **kwargs)
        print(f"{color}{out.getvalue()}{colors._END}")


def print_green(*args, **kwargs):
    print_colored(colors.GREEN, *args, **kwargs)


def print_red(*args, **kwargs):
    print_colored(colors.RED, *args, **kwargs)


def rmdir(dir):
    print_green(f"executing: rmdir {dir}")
    shutil.rmtree(dir, ignore_errors=True)


def ensure_dir_exists(dir):
    os.makedirs(dir, exist_ok=True)


def prefix(install_dir):
    return f"-DCMAKE_INSTALL_PREFIX={install_dir}" if install_dir else ""


def cmake(source_dir, build_dir, ninja, install_dir=None):
    execute(
        f"cmake {prefix(install_dir)} -S {source_dir} -B {build_dir} {' -G Ninja' if ninja else ''}")


def build(build_dir, config):
    execute(f"cmake --build {build_dir} --config {config} -j {os.cpu_count()}")


def install(build_dir, config):
    execute(
        f"cmake --install {build_dir} --config {config}")
