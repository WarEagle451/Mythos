<p align="center">
    <img src="https://github.com/WarEagle451/Mythos/blob/main/mythos/resources/branding/logo_name.png">
</p>

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://github.com/WarEagle451/Blue/blob/main/LICENSE)

# Table of Contents
1. [Features](#features)
2. [Requirements](#requirements)
3. [3rd Party Libraries](#3rd-party-libraries)
4. [Getting Started](#getting-started)
5. [Building Mythos](#building-mythos)
6. [Supported Tooling](#supported-tooling)
7. [Common Issues](#common-issues)

# Features
- <details><summary>Native Controller Support</summary>

    | Controller | USB | Bluetooth | Gyro | Additional Features |
    |:-:|:-:|:-:|:-:|:--|
    | Amazon Luna Controller | :x: | :x: | :x: | |
    | Dualsense | :white_check_mark: | :white_check_mark: | WIP | |
    | Dualsense Edge | :x: | :x: | :x: | |
    | Dualshock 3 | :x: | :x: | :x: | |
    | Dualshock 4 | :white_check_mark: | :white_check_mark: | :x: | |
    | Joy-Con | :x: | :x: | :x: | |
    | Nintendo Switch Pro Controller | :x: | :x: | :x: | |
    | Sixaxis | :x: | :x: | :x: | |
    | Stadia Controller | :x: | :x: | :x: | |
    | Steam Controller | :x: | :x: | :x: | |
    | Xbox 360 Controller | :x: | :x: | :x: | |
    | Xbox Wireless Controller | :x: | :x: | :x: | |
</details>

# Requirements
- C++20
- [A supported compiler](#compilers)
- [Python 3.11](https://www.python.org/downloads)
- [Ninja 1.11.1](https://github.com/ninja-build/ninja/releases/tag/v1.11.1)
- [cmake 3.28.0](https://cmake.org/download/)
- [Vulkan 1.3.211.0](https://vulkan.lunarg.com/sdk/home)

# 3rd Party Libraries
- [myl](https://github.com/WarEagle451/myl)
- [spdlog](https://github.com/gabime/spdlog)

# Getting Started
1. Nativate to `Mythos/scripts` and run setup. If needed the python file can also be ran manually, however submodules should be updated and initialized beforehand.
2. Follow any instructions provided by the setup script. Mythos' scripts will attempt to find any required programs on your system before requesting a download. Download URLs are located in the respective `setup_**.py` file in the `Mythos/scripts/python` directory.
3. Mythos should now be ready to be built and compiled.
4. [Build and compile Mythos](#building-mythos) using a supported compiler and build target.
5. Mythos should be ready to use!

# Building Mythos
1. Run cmake and select your desired [configuration](https://github.com/WarEagle451/Mythos/blob/main/CMakePresets.json). Build Mythos, if the build fails see [Common Issues](#common-issues)
2. The executable will be located in `Mythos/build/bin/{configuration}`.

# Supported Tooling
### Compilers
- MSVC
- Clang

# Common Issues
<details><summary>Incorrect Controller Inputs</summary>

If you have Steam installed, it's possible Steam is hijacking the controller's data and modifying it. This can be resolved 2 ways.
- `Steam` -> `Settings` -> `Controller` -> set `Enable Steam Input for [Xbox/Switch Pro/generic] controllers` to off, for PlayStation controllers set `PlayStation Controller Support` to `Not Enabled`.
- End Steam as a process from task manager.
</details>


<details><summary>CMAKE_MT-NOTFOUND</summary>

If the build failed due to `CMAKE_MT-NOTFOUND` this is because CMake could not find the manifest tool. This is likely to occur when attempting to build for Windows with Clang.

On Windows this can possibly be solved by one of the following methods;
1. Running vcvarsall.bat.
2. Adding the parent directory of the manifest tool (mt.exe) to the `PATH` system enviromental variable.
3. Manually setting `CMAKE_MT` in a CMakeUserPresets.json file (See below). The manifest tool should be located in `"C:/Program Files (x86)/Windows Kits/10/bin/{WINDOWS SDK VERSION}/x64"`
<details><summary>CMakeUserPresets.json Example File</summary>

    {
        "version": 8,
        "cmakeMinimumRequired": {
            "major": 3,
            "minor": 28,
            "patch": 0
        },
        "configurePresets": [
            {
                "name": "user-common",
                "hidden": true,
                "cacheVariables": {
                    "CMAKE_MT": "C:/Program Files (x86)/Windows Kits/10/bin/10.0.19041.0/x64/mt.exe"
                }
            },
            {
                "name": "override-windows-clang-release-x64",
                "displayName": "Override: Windows x64 Release - Clang",
                "description": "Target Windows with Clang for a x64 Release build",
                "inherits": [ "windows-clang-release-x64", "user-common" ]
            },
            {
                "name": "override-windows-clang-debug-x64",
                "displayName": "Override: Windows x64 Debug - Clang",
                "description": "Target Windows with Clang for a x64 debug build",
                "inherits": [ "windows-clang-debug-x64", "user-common" ]
            },
            {
                "name": "override-windows-clang-dev_release-x64",
                "displayName": "Override: Windows x64 Developer Release - Clang",
                "description": "Target Windows with Clang for a x64 developer release build",
                "inherits": [ "windows-clang-dev_release-x64", "user-common" ]
            }
        ]
    }
</details>
</details>