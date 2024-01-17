Import("env")

import subprocess
import serial.tools.list_ports
import os
import platform
import helper_functions
import platformio
from platformio.device.list.util import list_serial_ports
from time import sleep

################### Make sure Mcumgr exists locally ###################

# Determine executable name
third_party_dir = os.path.join(".", "owntech", "third_party")
mcumgr_executable = "mcumgr"     if platform.system() == 'Linux'   else \
                    "mcumgr-mac" if platform.system() == 'Darwin'  else \
                    "mcumgr.exe" if platform.system() == 'Windows' else \
                    None
mcumgr_url = "https://github.com/owntech-foundation/mcumgr/releases/download/v0.4/" + mcumgr_executable

res = helper_functions.check_file_and_download("Mcumgr", third_party_dir, mcumgr_executable, mcumgr_url, True)

if res != 0:
	import warnings
	warnings.warn("Unable to download Mcumgr! Make sure you are connected to the Internet.", Warning)
	warnings.warn("Without this tool, you may not be able to upload the firmware to the board without a ST Link.", Warning)

mcumgr_path = os.path.join(third_party_dir, mcumgr_executable)


################## Determine port ##################

def find_spin_port(env):
	# List all connected Spin boards
	available_ports = list(serial.tools.list_ports.grep("2FE3"))
	if len(available_ports) == 0:
		return None

	# Determine if there is a preferred board for upload
	try:
		config = env.GetProjectConfig()
		preferred_board_id = config.get("env", "board_id")
		print("Preferred board for upload: " + preferred_board_id)
	except platformio.project.exception.InvalidProjectConfError:
		preferred_board_id = None

	preferred_board_port = None
	port_number = 1
	print("Detecting Spin boards connected to host...")
	for port in available_ports:
		unique_id = port.serial_number
		comport = port.device
		print(f"  Board {port_number}: Found Spin board on port {comport} with unique ID {unique_id}")
		port_number +=1
		if unique_id == preferred_board_id:
			preferred_board_port = comport

	# Preferred board was found: use it for upload
	if preferred_board_port != None:
		print(f"Board with unique ID {preferred_board_id} was found and selected for upload")
		return (preferred_board_port, preferred_board_id)

	# Preferred board was not found (or no preferred board), but only one board connected: use it for upload
	if len(available_ports) == 1:
		if preferred_board_id != None:
			print(f"Board with unique ID {preferred_board_id} not found")
			print(f"WARNING: Board ID does not match preferred ID set in platformio.ini file. Uploading to board ID {available_ports[0].serial_number}")
		else:
			print(f"Uploading to board with ID {available_ports[0].serial_number}")
		return (available_ports[0].device, available_ports[0].serial_number)

	# Multiple boards connected, not including preferred board (or no preferred board): ask user for which one to use
	if preferred_board_id != None:
		print(f"Board with unique ID {preferred_board_id} wasn't found, please select board number manually")
	else:
		print(f"Multiple boards were found, please select board number manually")
	selected_port = None
	while selected_port is None or selected_port < 0 or selected_port >= port_number:
		try:
			selected_port = int(input(f"Enter Board number (between 1 and {port_number}): "))
			if selected_port < 1 or selected_port > port_number:
				print("Please enter a valid board number.")
		except ValueError:
			print("Invalid input. Please enter a valid number.")
	print(f"Board with ID {available_ports[selected_port-1].serial_number} was selected")
	return (available_ports[selected_port-1].device, available_ports[selected_port-1].serial_number)

def get_port_from_id(id_to_find):
	# List all connected Spin boards
	available_ports = list(serial.tools.list_ports.grep("2FE3"))
	if len(available_ports) == 0:
		return None

	# Find port
	for port in available_ports:
		if port.serial_number == id_to_find:
			return port.device

	# Port not found
	return None

################### Pre function ###################

def upload_pre(source, target, env):
	# Get spin port
	spin_port_ret = find_spin_port(env)
	if spin_port_ret == None:
		print("Error! No valid USB port found.")
		print("Make sure board is connected to the host computer.")
		exit(-1)

	(spin_port, spin_id) = spin_port_ret
	# Reboot board
	upload_options = {}
	if "BOARD" in env:
		upload_options = env.BoardConfig().get("upload", {})

	if not bool(upload_options.get("disable_flushing", False)):
		env.FlushSerialBuffer(spin_port)

	env.TouchSerialPort(spin_port, 1200)

	# Wait for board to reboot in bootloader mode
	print("Rebooting board in bootloader mode...")
	elapsed=0
	port_found = False
	while elapsed < 10 and port_found == False:
		spin_port = get_port_from_id(spin_id)
		if spin_port != None:
			port_found = True
		sleep(0.25)
		elapsed += 0.25

	if port_found == False:
		print("Error! Unable to find selected board after reboot.")
		exit(-1)

	# Wait 1 more second so that bootloader finishes loading
	sleep(1)

	print("Board ready.")

	# Configure mcumgr with found port
	init_command = [mcumgr_path, \
	               'conn', \
	               'add', \
	               'serial', \
	               'type=serial', \
	               f'connstring=dev={spin_port},baud=115200,mtu=128' \
	               ]

	try:
		# Execute the command
		subprocess.run(init_command, check=True)
	except subprocess.CalledProcessError as e:
		print(f"Error executing mcumgr command: {e}")
		exit(-1)
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

	print("Please note that first boot will take a few seconds due to partition swapping.")

################### Register pre and post functions ###################

env.AddPreAction("upload", upload_pre)
env.AddPostAction("upload", upload_post)
