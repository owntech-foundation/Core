# Script used to download third-party Zephyr modules
# Refer to PlatformIO doc: https://docs.platformio.org/en/latest/scripting/index.html


#####
# Prepare environment

import os
Import("env")

try:
	import GitPython
except ImportError:
	env.Execute("$PYTHONEXE -m pip install GitPython")


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
