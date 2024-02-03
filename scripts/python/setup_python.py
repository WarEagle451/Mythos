import sys
import subprocess
import importlib.util as lib_util

class python_config:
    @classmethod
    def validate(cls):
        if not cls.__validate_python():
            return # cannot validate further

        for package_name in ["requests"]:
            if not cls.__validate_package(package_name):
                return # cannot validate further

    @classmethod
    def __validate_python(cls, version_major = 3, version_minor = 11):
        if sys.version is not None:
            print(f"Python version {sys.version_info.major:d}.{sys.version_info.minor:d}.{sys.version_info.micro:d} detected.")
            if sys.version_info.major < version_major or (sys.version_info.major == version_major and sys.version_info.minor < version_minor):
                print(f"Python version too low, expected version {version_major:d}.{version_minor:d} or higher.")
                return False
            return True

    @classmethod
    def __validate_package(cls, package_name):
        if lib_util.find_spec(package_name) is None:
            return cls.__install_package(package_name)
        return True

    @classmethod
    def __install_package(cls, package_name):
        permission_granted = False
        while not permission_granted:
            reply = str(input("Would you like to install Python package '{0:s}'? [Y/N]: ".format(package_name))).lower().strip()[:1]
            if reply == 'n':
                return False
            permission_granted = (reply == 'y')
        
        print(f"Installing {package_name} module...")
        subprocess.check_call(['python', '-m', 'pip', 'install', package_name])

        return cls.__validate_package(package_name)

if __name__ == "__main__":
    python_config.validate()