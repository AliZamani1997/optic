Import("env")
import os
def run_my_scripts(*args, **kwargs):
    # Example: run creat_server_pages.py before build
    os.system("python scripts/creat_parameters.py")
    os.system("python scripts/creat_server_pages.py")
    # Add more scripts as needed
print("[pre_build.py]")
run_my_scripts()
# env.AddPreAction("buildprog", run_my_scripts)