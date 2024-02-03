from setup_python import python_config as python
from setup_cmake import cmake_config as cmake
from setup_ninja import ninja_config as ninja

if __name__ == '__main__':
    python.validate()
    cmake.validate()
    ninja.validate()