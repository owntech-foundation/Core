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

	owntech_port = None
	if len(available_ports) > 0:
		owntech_port = available_ports[0].device
	else:
		print("Error! No valid USB port found")
		exit(-1)

	print(f"Found OwnTech board on port: {owntech_port}")

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
