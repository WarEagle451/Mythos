import os
from pathlib import Path

import utility

class vulkan_config:
    required_version = "1.3.211.0"
    install_url = f"https://sdk.lunarg.com/sdk/download/{required_version}/windows/VulkanSDK-{required_version}-Installer.exe"
    directory = "Mythos/vendor/VulkanSDK"

    @classmethod
    def validate(cls):
        if (not cls.__check_installed()):
            print("Vulkan SDK not installed correctly")
            return
        
        if (not cls.__check_installed_debug_libs()):
            print("\nNo Vulkan SDK libraries found")
            print("Debug configuration disabled")

    @classmethod
    def __check_installed(cls):
        vulkan_sdk_path = os.environ.get("VULKAN_SDK")
        if (vulkan_sdk_path is None):
            print("Vulkan SDK not installed!")
            cls.__install_vulkan_sdk()
            return False
        else:
            print(f"\nLocated Vulkan SDK at {vulkan_sdk_path}")

        if (cls.required_version not in vulkan_sdk_path):
            print(f"Incorrect Vulkan SDK Version detected! {cls.required_version} is required")
            cls.__install_vulkan_sdk()
            return False
        else:
            print(f"Correct Vulkan SDK located at {vulkan_sdk_path}")
            return True
        
    @classmethod
    def __check_installed_debug_libs(cls):
        vulkan_sdk_path = os.environ.get("VULKAN_SDK")
        shadercd_lib = Path(f"{vulkan_sdk_path}/Lib/shaderc_sharedd.lib")
        return shadercd_lib.exists()


    @classmethod
    def __install_vulkan_sdk(cls):
        permission_granted = False
        while (not permission_granted):
            reply = str(input("Suitable version of the Vulkan SDK not found. Would you like to download the Vulkan SDKs"))
            if (reply.casefold() == "n".casefold()):
                return False
            permission_granted = (reply.casefold() == "y".casefold())

        vulkan_path = f"{cls.directory}/VulkanSDK-{cls.required_version}-Installer.exe"
        print("Downloading {0:s} to {1:s}".format(cls.install_url, vulkan_path))
        utility.download_file(cls.install_url, vulkan_path)
        print("Running Vulkan SDK installer...")
        os.startfile(os.path.abspath(vulkan_path))
        print("Re-run this script after installation!")
        quit()

if __name__ == "__main__":
    vulkan_config.validate()