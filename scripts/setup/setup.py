import os
import subprocess
import platform

from setup_python import python_config as python_requirements

python_requirements.validate() # Confirm everything for the setup is installed

from setup_premake import premake_config as premake_requirements
from setup_vulkan import vulkan_config as vulkan_requirements

os.chdir('./../') # Change to root directory

premake_installed = premake_requirements.validate()
vulkan_requirements.validate()

import submodules

if (premake_installed):
    if platform.system() == "Windows":
        print("\nRunning Premake")
        subprocess.call([os.path.abspath("./scripts/windows_generate_projects.bat"), "nopause"])
    print("\nSetup completed!")
else:
     print("Mythos requires Premake to generate project files.")