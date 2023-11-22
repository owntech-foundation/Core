# Script used to download third-party Zephyr modules
# Refer to PlatformIO doc: https://docs.platformio.org/en/latest/scripting/index.html


#####
# Prepare environment

import os
Import("env")


####
# Determine if thingset must be downloaded

do_download=False
with open("zephyr/prj.conf", 'r') as file:
	conf_content = file.readlines()
	for line in conf_content:
		if line.startswith("#"):
			None
		else:
			if "CONFIG_OWNTECH_COMMUNICATION_ENABLE_CAN=y" in line:
				do_download=True
				break

if do_download:
	try:
		import git
	except ImportError:
		env.Execute("$PYTHONEXE -m pip install GitPython")
		import git


	#####
	# Define base objects

	third_party_dir = os.path.join(".", "zephyr", "third_party_modules")

	def cloneRepo(folderName, url, branch):
		from git import Repo
		repo = Repo.clone_from(url, folderName)
		if branch is not None:
			repo.git.checkout(branch)


	#####
	# Make sure the 3rd party folder exists

	if not os.path.isdir(third_party_dir):
		os.mkdir(third_party_dir)


	#####
	# Install thingset

	thingset_dir = os.path.join(third_party_dir, "thingset")
	if not os.path.isdir(thingset_dir):
		cloneRepo(thingset_dir, "https://github.com/ThingSet/thingset-device-library", "main")
