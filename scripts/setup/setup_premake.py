import sys
import os
from pathlib import Path

import utils

class premake_config:
    version = "5.0.0-beta1"
    zip_urls = f"https://github.com/premake/premake-core/releases/download/v{version}/premake-{version}-windows.zip"
    license_url = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
    directory = "././vendor/premake/bin"

    @classmethod
    def validate(cls):
        if (not cls.check_installed()):
            print("Premake is not installed")
            return False
        print(f"Correct Premake located at {os.path.abspath(cls.directory)}")
        return True

    @classmethod
    def check_installed(cls):
        exe = Path(f"{cls.directory}/premake5.exe")
        if (not exe.exists()):
            return cls.install_premake()
        return True

    @classmethod
    def install_premake(cls):
        premission_granted = False
        while not premission_granted:
            reply = str(input("Premake not found. Would you like to download Premake {0:s}? [Y/N]: ".format(cls.premakeVersion))).lower().strip()[:1]
            if (reply == 'n'):
                return False
            premission_granted = (reply == 'y')
        premake_path =  f"{cls.directory}/premake-{cls.version}-windows.zip"
        print("Downloading {0:s} to {1:s}".format(cls.zip_urls, premake_path))

        utils.download_file(cls.zip_urls, premake_path)
        print("Extracting", premake_path)
        utils.unzip_file(premake_path, delete_zip_file = True)
        print(f"Premake {cls.version} has been downloaded to '{cls.directory}'")

        license_path = f"{cls.directory}/LICENSE.txt"
        print("Downloading {0:s} to {1:s}".format(cls.license_url, license_path))
        utils.download_file(cls.license_url, license_path)
        print(f"Premake License file has been downloaded to '{cls.directory}'")

        return True