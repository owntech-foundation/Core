Import("env")

import subprocess
import shlex
import serial.tools.list_ports
import os

mcumgr_executable = "./mcumgr" if os.name == 'posix' else ".\\mcumgr.exe"

############################################################

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
	init_command = f'conn add serial type="serial" connstring="dev={owntech_port},baud=115200,mtu=128"'
	parsed = shlex.split(init_command)
	parsed.insert(0, mcumgr_executable)

	try:
		# Execute the command
		subprocess.run(parsed, check=True)
	except subprocess.CalledProcessError as e:
		print(f"Error executing mcumgr command: {e}")
	except FileNotFoundError:
		print("mcumgr executable not found. Make sure it's in your PATH.")


def upload_post(source, target, env):
	hash = [mcumgr_executable, "-c", "serial", "image", "list"]
	try:
		subprocess.run(hash, stderr=subprocess.STDOUT, universal_newlines=True)
	except subprocess.CalledProcessError as e:
		print("Unable to list board partitions")
		print(f"Error executing list command: {e}")

	reset = [mcumgr_executable, "-c", "serial", "reset"]
	print("Resetting board...")
	try:
		subprocess.run(reset, check=True)
	except subprocess.CalledProcessError as e:
		print("Error! Unable to reset board.")
		print(f"Error executing reset command: {e}")
		print("Please reset board manually")

############################################################

env.AddPreAction("upload", upload_pre)
env.AddPostAction("upload", upload_post)
