import helper_functions
import os
import shutil
import platform
import subprocess

Import("env")
platformpio = env.PioPlatform()

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

    dfu_suffix = "dfu-suffix"     if platform.system() == 'Linux' and platform.machine() == 'x86_64' else \
                        "dfu-suffix" if platform.system() == 'Linux' and platform.machine() == 'armv7l' else \
                        "dfu-suffix" if platform.system() == 'Darwin'  else \
                        "dfu-suffix.exe" if platform.system() == 'Windows' else \
                        None
    dfu_util = "dfu-util"     if platform.system() == 'Linux' and platform.machine() == 'x86_64' else \
                        "dfu-util" if platform.system() == 'Linux' and platform.machine() == 'armv7l' else \
                        "dfu-util" if platform.system() == 'Darwin'  else \
                        "dfu-util.exe" if platform.system() == 'Windows' else \
                        None
    
    dfusuffix_path = [platformpio.get_package_dir("tool-dfuutil") + "/bin/" + dfu_suffix]
    dfuutil_path = [platformpio.get_package_dir("tool-dfuutil") + "/bin/" + dfu_util]
    
    suffix_flags = ["-v 0x0483", \
                    "-p 0xDF11", \
                    "-d 0xffff", \
                    "-a", \
                    bootloader_path \
                    ]
    upload_flags = [ "-a 0", \
                    "-s 0x08000000", \
                    "-D", 
                    bootloader_path \
                    ]
    # Sign the bootloader binary using dfu-suffix
    suffix_command =  dfusuffix_path + suffix_flags
    upload_command =  dfuutil_path + upload_flags

    try:
        # Execute the command
        subprocess.run(suffix_command, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error executing mcumgr command: {e}")
        os.remove(bootloader_path)
        exit(-1)
    
    try:
        # Execute the command
        subprocess.run(upload_command)
    except subprocess.CalledProcessError as e:
        print(f"Error executing mcumgr command: {e}")
        os.remove(bootloader_path)
        exit(-1)
    
    os.remove(bootloader_path)

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

