Import("env")

import subprocess
import serial.tools.list_ports
import os
import platform
from stat import *

################### Make sure Mcumgr exists locally ###################

# Make sure "third_party" dir exists
third_party_dir = os.path.join(".", "owntech", "third_party")
if not os.path.isdir(third_party_dir):
	os.mkdir(third_party_dir)

# Determine executable name
mcumgr_executable = "mcumgr"     if platform.system() == 'Linux'   else \
                    "mcumgr.exe" if platform.system() == 'Windows' else \
                    None

mcumgr_path = os.path.join(third_party_dir, mcumgr_executable)

# Download mcumgr if not available
if not os.path.isfile(mcumgr_path):
	print("Mcumgr executable not available: downloading.")
	import urllib.request
	# Download file
	mcumgr_url = "https://github.com/owntech-foundation/mcumgr/releases/download/v0.4/" + mcumgr_executable
	urllib.request.urlretrieve(mcumgr_url, mcumgr_path)
	if platform.system() == 'Linux':
		# Make file executable
		st = os.stat(mcumgr_path)
		os.chmod(mcumgr_path, st.st_mode | S_IEXEC)
	print("Download complete.")

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
		print("Error! No valid USB port found")
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
		print("mcumgr executable not found. Make sure it's in your PATH.")

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
