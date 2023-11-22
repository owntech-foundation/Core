Import("env")

import subprocess
import serial.tools.list_ports
import os
import platform
import helper_functions

################### Make sure Mcumgr exists locally ###################

# Determine executable name
third_party_dir = os.path.join(".", "owntech", "third_party")
mcumgr_executable = "mcumgr"     if platform.system() == 'Linux'   else \
                    "mcumgr.exe" if platform.system() == 'Windows' else \
                    None
mcumgr_url = "https://github.com/owntech-foundation/mcumgr/releases/download/v0.4/" + mcumgr_executable

res = helper_functions.check_file_and_download("Mcumgr", third_party_dir, mcumgr_executable, mcumgr_url, True)

if res != 0:
	import warnings
	warnings.warn("Unable to download Mcumgr! Make sure you are connected to the Internet.", Warning)
	warnings.warn("Without this tool, you may not be able to upload the firmware to the board without a ST Link.", Warning)

mcumgr_path = os.path.join(third_party_dir, mcumgr_executable)

################### Pre function ###################

def upload_pre(source, target, env):
	# List available ports
	available_ports = list(serial.tools.list_ports.grep("2FE3"))
	selected_board = env.GetProjectOption("board_id")

	owntech_port = None
	if len(available_ports) > 0:
		port_number = 0
		for port in available_ports:
			unique_id = port.serial_number
			comport = port.device
			print(f"Found OwnTech board number {port_number} on port: {comport} with unique ID {unique_id}")
			port_number +=1
			if unique_id == selected_board:
				owntech_port = comport

		if owntech_port is None:
			if len(available_ports) > 1:
				print(f"Board with unique ID {selected_board} not found, select board number manually")
				selected_port = None
				while selected_port is None or selected_port < 0 or selected_port >= port_number:
					try:
						selected_port = int(input(f"Enter Board number (between 0 and {port_number - 1}): "))
						if selected_port < 0 or selected_port >= port_number:
							print("Please enter a valid board number.")
					except ValueError:
						print("Invalid input. Please enter a valid number.")
				owntech_port = available_ports[selected_port].device
			else:
				print(f"Board with unique ID {selected_board} not found")
				print(f"WARNING: Board ID does not match platformio.ini file. Uploading to board ID {available_ports[0].serial_number}")
				owntech_port = available_ports[0].device

	else:
		print("Error! No valid USB port found.")
		print("Make sure board is connected to the host computer.")
		exit(-1)

	# Initialize serial port
	init_command = [mcumgr_path, \
	               'conn', \
	               'add', \
	               'serial', \
	               'type=serial', \
	               f'connstring=dev={owntech_port},baud=115200,mtu=128' \
	               ]

	try:
		# Execute the command
		subprocess.run(init_command, check=True)
	except subprocess.CalledProcessError as e:
		print(f"Error executing mcumgr command: {e}")
	except FileNotFoundError:
		print("Error! Mcumgr is not available.")
		print("You must be connected to the Internet the first time you upload a firmware to the board so that Mcumgr can be retrieved.")
		exit(-1)

	# Place executable path in upload command
	env["UPLOADCMD"] = env["UPLOADCMD"].replace("MCUMGRPATH", mcumgr_path)


################### Post function ###################

def upload_post(source, target, env):
	hash = [mcumgr_path, "-c", "serial", "image", "list"]
	try:
		subprocess.run(hash, stderr=subprocess.STDOUT, universal_newlines=True)
	except subprocess.CalledProcessError as e:
		print("Unable to list board partitions")
		print(f"Error executing list command: {e}")

	reset = [mcumgr_path, "-c", "serial", "reset"]
	print("Resetting board...")
	try:
		subprocess.run(reset, check=True)
	except subprocess.CalledProcessError as e:
		print("Error! Unable to reset board.")
		print(f"Error executing reset command: {e}")
		print("Please reset board manually")

################### Register pre and post functions ###################

env.AddPreAction("upload", upload_pre)
env.AddPostAction("upload", upload_post)
