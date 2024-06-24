# Script used to download third-party Zephyr modules
# Refer to PlatformIO doc: https://docs.platformio.org/en/latest/scripting/index.html


#####
# Prepare environment

import os
Import("env")

# Specify fixed commits for ThingSet libraries to make builds reproducible
thingset_node_rev = "d3338c7623869637bc6ce0b95358e44db2a3443f"
thingset_sdk_rev = "b3382df5b3bd6b7764756c3de42d94e9f42bd376"

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

	def cloneRepo(org_url, repo_name, rev):
		from git import Repo
		path = os.path.join(third_party_dir, repo_name)
		if os.path.isdir(path):
			repo = Repo(path)
		else:
			repo = Repo.clone_from(org_url + "/" + repo_name, path)
		repo.git.checkout(rev)


	#####
	# Make sure the 3rd party folder exists

	if not os.path.isdir(third_party_dir):
		os.mkdir(third_party_dir)


	#####
	# Install thingset

	cloneRepo("https://github.com/ThingSet", "thingset-node-c", thingset_node_rev)
	cloneRepo("https://github.com/ThingSet", "thingset-zephyr-sdk", thingset_sdk_rev)
