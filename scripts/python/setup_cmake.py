import importlib.util as lib_util
import re
import subprocess

from utility import version

class cmake_config:
    @classmethod
    def validate(cls):
        if not cls.__validate_cmake():
            return # cannot validate further

        for package_name in ["requests"]:
            if not cls.__validate_package(package_name):
                return # cannot validate further

    @classmethod
    def __validate_cmake(cls, version_major = 3, version_minor = 28):
        try:
            version_info = re.search("[0-9]+.[0-9]+.[0-9]+", subprocess.check_output(["cmake", "--version"]).decode('utf-8')).group(0)
            if version_info:
                print(f"Cmake version {version_info} detected.")
                if (version.parse(version_info) < version(version_major, version_minor)):
                    print(f"CMake version too low, expected version {version_major:d}.{version_minor:d} or higher.")
                    return False
                return True
            return False
        except:
            print("Cmake not found.")
            return False

    @classmethod
    def __validate_package(cls, package_name):
        if lib_util.find_spec(package_name) is None:
            return cls.__install_package(package_name)
        return True

    @classmethod
    def __install_package(cls, package_name):
        permission_granted = False
        while not permission_granted:
            reply = str(input("Would you like to install CMake package '{0:s}'? [Y/N]: ".format(package_name))).lower().strip()[:1]
            if reply == 'n':
                return False
            permission_granted = (reply == 'y')
        
        print(f"Installing {package_name} module...")
        subprocess.check_call(['python', '-m', 'pip', 'install', package_name])

        return cls.__validate_package(package_name)

if __name__ == "__main__":
    cmake_config.validate()