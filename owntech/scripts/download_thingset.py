# Script used to download third-party Zephyr modules
# Refer to PlatformIO doc: https://docs.platformio.org/en/latest/scripting/index.html


#####
# Prepare environment

import os
Import("env")

# Specify fixed commits for ThingSet libraries to make builds reproducible
thingset_node_rev = "68c7544830df2ba23f67e31bad91e124377827a3"
thingset_sdk_rev = "e57447bbeb7c165e14a273242e8495343c1c6f54"

####
# Determine if thingset must be downloaded

do_download=False

if os.path.exists("src/app.conf"):
	with open("src/app.conf", 'r') as file:
		conf_content = file.readlines()
		for line in conf_content:
			if line.startswith("#"):
				None
			else:
				if "CONFIG_OWNTECH_COMMUNICATION_ENABLE_CAN=y" in line:
					do_download=True
					break

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
		try:
			from git import Repo
		except:
			print("Can not find git in PlatformIO Python\n")

		path = os.path.join(third_party_dir, repo_name)
		if os.path.isdir(path):
			repo = Repo(path)
		else:
			try:
				repo = Repo.clone_from(org_url + "/" + repo_name, path)
			except:
				print(f"Can not clone Thingset repository.")

		try:
			repo.git.fetch('origin', rev)  # Ensure the commit is available
			repo.git.checkout(rev)
		except:
			print(f"Error: Unable to checkout on commit {rev}.\n")


	#####
	# Make sure the 3rd party folder exists

	if not os.path.isdir(third_party_dir):
		os.mkdir(third_party_dir)


	#####
	# Install thingset

	cloneRepo("https://github.com/ThingSet", "thingset-node-c", thingset_node_rev)
	cloneRepo("https://github.com/ThingSet", "thingset-zephyr-sdk", thingset_sdk_rev)
