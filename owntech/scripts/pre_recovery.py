import helper_functions
import os
import shutil
import subprocess

Import("env")
platform = env.PioPlatform()


#Dummy function to print a user friendly message using env.VerboseAction() 
#After successfully loading an example.
def PrintSuccess(target, source, env):
    build_dir = os.path.join(".", ".pio", "build")
    shutil.rmtree(build_dir) #Removes build folder in order to trigger a clean compilation
    #Necessary to make sure we go through the post action triggered by the build of firmware.bin...
    return

def installBootloader(target, source, env):
    # Make sure file is available locally
    third_party_dir = os.path.join(".", "owntech", "bootloader")
    bootloader_binary = "zephyr.bin"
    bootloader_path = os.path.join(third_party_dir, bootloader_binary)
    booloader_url = "https://github.com/owntech-foundation/bootloader/releases/download/v1.1.0/zephyr.bin"

    res = helper_functions.check_file_and_download("Bootloader firmware", third_party_dir, bootloader_binary, booloader_url)

    if res != 0:
        exit(-1)
    # build_path = os.path.join(env.Dump("PROJECT_BUILD_DIR").replace("\'", ""))
    # core_path = os.path.dirname(os.path.dirname(build_path))
	# # Sign the bootloader binary using dfu-suffix
    # subprocess.run(
    #                 " ".join([
    #                     '%s' % os.path.join(platform.get_package_dir("tool-dfuutil") or "",
    #                          "bin", "dfu-suffix"),
    #                     "-v 0x0483",
    #                     "-p 0xDF11",
    #                     "-d 0xffff", "-a", os.path.join(core_path,"owntech", "bootloader", bootloader_binary)
    #                 ]), check=True)
    # subprocess.run(
    #                 " ".join([
    #                     '"%s"' % os.path.join(platform.get_package_dir("tool-dfuutil") or "",
    #                          "bin", "dfu-util"),
    #                     "-d [0x0483,0xDF11]",
    #                     "-a 0",
    #                     "-s 0x08000000"
    #                     "-D", os.path.join(core_path,"owntech", "bootloader", bootloader_binary)
    #                 ]), check=True)


#Defines the targets for each example referenced in library.json and create a nice GUI.
env.AddTarget(
	name="recovery-bootloader",
	dependencies=None,
	actions=env.VerboseAction(installBootloader,"Installing bootloader..."),
	title="[Advanced] Bootloader recovery",
	description="Reinstall the OwnTech bootloader. NOTE: PB8-BOOT0 must be jumped at 3.3V using a wire and board must be reset with USB cable connected to computer.",
	group="OwnTech",
	always_build=True,
)

