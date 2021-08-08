import argparse
from common import get_root_path, rmdir, ensure_dir_exists, cmake, build, install


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--clean", help="clean before build", action="store_true")
    parser.add_argument("--config", help="configuration", default="release")
    args = parser.parse_args()

    source_dir = get_root_path().joinpath("src")
    build_dir = get_root_path().joinpath(f"build")
    install_dir = get_root_path().joinpath(f"bin/{args.config}")

    if args.clean:
        rmdir(build_dir)
        rmdir(install_dir)
        exit()

    ensure_dir_exists(install_dir)
    cmake(source_dir, build_dir, False, install_dir)
    build(build_dir, args.config)
    install(build_dir, args.config)
