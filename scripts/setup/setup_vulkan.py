import os
import sys
import subprocess
from pathlib import Path

import utils

from io import BytesIO
from urllib.request import urlopen

class vulkan_config:
    required_version = "1.3.211.0"
    directory = "Mythos/vendor/VulkanSDK" # Installer download location, should probs go into the os's temp dir

    @classmethod
    def validate(cls):
        if (not cls.check_vulkan_sdk()):
            print("Vulkan SDK not installed correctly.")
            return
        
        if (not cls.check_vulkan_sdk_debug_libs):
            print("\nNo Vulkan SDK libs found")
            print("Debug configuration disabled")

    @classmethod
    def check_vulkan_sdk(cls):
        vulkan_sdk = os.environ.get("VULKAN_SDK")
        if (vulkan_sdk is None):
            print("\nVulkan SDK not installed!")
            cls.__install_vulkan_sdk()
            return False
        else:
            print(f"\nLocated Vulkan SDK at {vulkan_sdk}")

        if (cls.required_version not in vulkan_sdk):
            print(f"You don't have the correct Vulkan SDK version! (Mythos requires {cls.required_version})")
            cls.__install_vulkan_sdk()
            return False
    
        print(f"Correct Vulkan SDK located at {vulkan_sdk}")
        return True
        
    @classmethod
    def check_vulkan_sdk_debug_libs(cls):
        vulkan_sdk = os.environ.get("VULKAN_SDK")
        shadercd_lib = Path(f"{vulkan_sdk}/Lib/shaderc_sharedd.lib")
        return shadercd_lib.exists()

    @classmethod
    def __install_vulkan_sdk(cls):
        permission_granted = False
        while not permission_granted:
            reply = str(input("Would you like to install VulkanSDK {0:s}? [Y/N]: ".format(cls.required_version))).lower().strip()[:1]
            if reply == 'n':
                return
            permission_granted = (reply == 'y')

        vulkan_install_url = f"https://sdk.lunarg.com/sdk/download/{cls.required_version}/windows/VulkanSDK-{cls.required_version}-Installer.exe"
        vulkan_path = f"{cls.directory}/VulkanSDK-{cls.required_version}-Installer.exe"
        print("Downloading {0:s} to {1:s}".format(vulkan_install_url, vulkan_path))
        utils.download_file(vulkan_install_url, vulkan_path)
        print("Running Vulkan SDK installer...")
        os.startfile(os.path.abspath(vulkan_path))
        print("Re-run this script after installation!")
        quit()

if __name__ == "__main__":
    vulkan_config.validate()
