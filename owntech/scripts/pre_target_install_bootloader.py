import helper_functions
import os
import subprocess

Import("env")


def installBootloader(target, source, env):
	# Make sure file is available locally
	third_party_dir = os.path.join(".", "owntech", "bootloader")
	bootloader_binary = "zephyr.bin"
	booloader_url = "https://github.com/owntech-foundation/bootloader/releases/download/v1.1.0/zephyr.bin"

	res = helper_functions.check_file_and_download("Bootloader firmware", third_party_dir, bootloader_binary, booloader_url)

	if res != 0:
		exit(-1)

	# Build uploader command line
	platform = env.PioPlatform()
	uploader_path = [platform.get_package_dir("tool-openocd") + "/bin/openocd"]

	debug_level = ["-d1"]

	board = env.BoardConfig()
	debug_tools = board.get("debug.tools", {})
	upload_protocol = env.subst("$UPLOAD_PROTOCOL")
	server_arguments = debug_tools.get(upload_protocol).get("server").get("arguments", [])

	bootloader_path = os.path.join(third_party_dir, bootloader_binary)
	program_arguments = ["-c", f"program {bootloader_path} 0x8000000 verify reset; shutdown;"]

	command_line = uploader_path + debug_level + server_arguments + program_arguments

	# Replace $PACKAGE_DIR with its path
	command_line = [
		f.replace("$PACKAGE_DIR",
			platform.get_package_dir("tool-openocd") or "")
		for f in command_line
	]

	# Display command and run it
	print(command_line)
	subprocess.run(command_line)

env.AddPlatformTarget(
    name="owntech-install-bootloader",
    dependencies=None,
    actions=[env.VerboseAction(installBootloader, "Installing bootloader...")],
    title="[STLink required] Install OwnTech bootloader",
    description="Install the OwnTech bootloader to the Spin board. NOTE: The board must be connected to an external STLink for this to work."
)
