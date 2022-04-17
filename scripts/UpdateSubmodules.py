import subprocess

print("\nUpdating submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])
subprocess.call(["git", "submodule", "update", "--remote", "--merge"])