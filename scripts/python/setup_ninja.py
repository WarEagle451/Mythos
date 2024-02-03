import re
import subprocess
import sys
import utility

from distutils.spawn import find_executable

def get_platform_ninja_name():
    match sys.platform:
        case "darwin":           return "mac"
        case "linux" | "linux2": return "linux"
        case "win32":            return "win"
        case _:
            print("Ninja does not support this platform")
            return ""

class ninja_config:
    pid = get_platform_ninja_name()
    required_version = "1.11.1"
    zip_urls = f"https://github.com/ninja-build/ninja/releases/download/v{required_version}/ninja-{pid}.zip"
    license_url = "https://raw.githubusercontent.com/ninja-build/ninja/master/COPYING"
    directory = f"{utility.project_root()}/vendor/ninja/bin"

    @classmethod
    def validate(cls):
        if not cls.__check_installed():
            print("Ninja is not installed.")
            return False
        return True

    @classmethod
    def __check_installed(cls):
        ninja_path = find_executable("ninja", cls.directory) # Check if the project has already downloaded a local Ninja
        if (ninja_path == None):
            ninja_path = find_executable("ninja") # Check for Ninja in PATH

        if (ninja_path != None):
            canidate_version = re.search("[0-9]+.[0-9]+.[0-9]+", subprocess.check_output([ninja_path, "--version"]).decode('utf-8')).group(0)
            print(f"Ninja version {canidate_version} detected.")
            ninja_version = utility.version.parse(canidate_version)
            if (ninja_version >= utility.version.parse(cls.required_version)):
                return True
            else:
                print(f"Ninja version too low, expected version {ninja_version.major:d}.{ninja_version.minor:d}.{ninja_version.patch:d} or higher.")
        return cls.__install_ninja()

    @classmethod
    def __install_ninja(cls):
        premission_granted = False
        while not premission_granted:
            reply = str(input("Suitable version of Ninja not found. Would you like to download Ninja {0:s}? [Y/N]: ".format(cls.required_version))).lower().strip()[:1]
            if reply.casefold() == "n".casefold():
                return False
            premission_granted = (reply.casefold() == "y".casefold())
        ninja_path = f"{cls.directory}/ninja-{cls.required_version}-windows.zip"
        print("Downloading {0:s} to {1:s}".format(cls.zip_urls, ninja_path))

        utility.download_file(cls.zip_urls, ninja_path)
        print("Extracting", ninja_path)
        utility.unzip_file(ninja_path, overwrite=True, delete_zip_file=True)
        print(f"Ninja {cls.required_version} has been downloaded to '{cls.directory}'.")

        license_path = f"{cls.directory}/LICENSE.txt"
        print("Downloading {0:s} to {1:s}".format(cls.license_url, license_path))
        utility.download_file(cls.license_url, license_path)
        print(f"Ninja License file has been downloaded to '{cls.directory}'.")

        return True
    

if __name__ == "__main__":
    ninja_config.validate()