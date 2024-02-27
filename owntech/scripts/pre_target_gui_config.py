import os
import subprocess
import sys
import platformio

Import("env")

def RunGuiconfig(target, source, env):
    '''
    This function is based on PlatformIO's STSTM32 builder/main.py file
    (c) PlatformIO, Apache Licence, version 2.0
    https://www.apache.org/licenses/LICENSE-2.0
    '''
    zephyr_env = os.environ.copy()
    platform = env.PioPlatform()

    zephyr_env["GNUARMEMB_TOOLCHAIN_PATH"] = platform.get_package_dir("toolchain-gccarmnoneeabi")
    zephyr_env["ZEPHYR_TOOLCHAIN_VARIANT"] = "gnuarmemb"
    zephyr_env["ZEPHYR_BASE"] = platform.get_package_dir("framework-zephyr")

    additional_packages = [
        platform.get_package_dir("tool-dtc"),
        platform.get_package_dir("tool-ninja"),
    ]
    if not platformio.compat.IS_WINDOWS:
        additional_packages.append(platform.get_package_dir("tool-gperf"))
    zephyr_env["PATH"] = os.pathsep.join(additional_packages)

    rc = subprocess.call(
        [
            os.path.join(platform.get_package_dir("tool-cmake"), "bin", "cmake"),
            "--build",
            env.subst("$BUILD_DIR"),
            "--target",
            "guiconfig",
        ],
        env=zephyr_env,
    )

    if rc != 0:
        sys.stderr.write("Error: Couldn't execute 'Guiconfig' target.\n")
        env.Exit(1)

env.AddTarget(
    name="guiconfig",
    dependencies=None,
    actions=[env.VerboseAction(RunGuiconfig, "Running guiconfig")],
    title="[Advanced] Run GUIconfig",
    description="Launch GUIconfig to access Zephyr RTOS configuration",
    group="OwnTech",
	always_build=False
)