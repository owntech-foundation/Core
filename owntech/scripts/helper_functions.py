import os
import platform
from stat import *

def check_file_and_download(what, folder_path, file_name, file_url, make_exec = False):
	# Make sure folder exists
	if not os.path.isdir(folder_path):
		os.makedirs(folder_path)
	if not os.path.isdir(folder_path):
		print(f"Error! Unable to create directory {folder_path}.")
		print("Make sure you have permission to write in this directory.")
		return -1

	# Determine file path
	file_path = os.path.join(folder_path, file_name)

	# Make sure file is available locally
	if not os.path.isfile(file_path):
		import urllib.request
		from urllib.error import URLError

		print(f"{what} is not available locally, downloading.")
		try:
			# Download file
			urllib.request.urlretrieve(file_url, file_path)
			print("Download complete.")
		except URLError:
			print(f"Error! Unable to download {what}.")
			print("Make sure you are connected to the Internet.")
			return -1

	if make_exec:
		if platform.system() == 'Linux':
			# Make file executable
			st = os.stat(file_path)
			os.chmod(file_path, st.st_mode | S_IXUSR | S_IXGRP | S_IXOTH)

	return 0
