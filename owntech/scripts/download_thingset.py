# Script used to download third-party Zephyr modules
# Refer to PlatformIO doc: https://docs.platformio.org/en/latest/scripting/index.html


#####
# Prepare environment

import os
Import("env")

# Specify fixed commits for ThingSet libraries to make builds reproducible
thingset_node_rev = "68c7544830df2ba23f67e31bad91e124377827a3"
thingset_sdk_rev = "48740769cea0d9666fc9132cd8923f9ce2c088f9"

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
		from git import Repo, GitCommandError
		path = os.path.join(third_party_dir, repo_name)
		if os.path.isdir(path):
			repo = Repo(path)
		else:
			try:
				repo = Repo.clone_from(org_url + "/" + repo_name, path)
			except GitCommandError:
				print(f"Can not clone Thingset repository.")

		try:
			repo.git.fetch('origin', rev)  # Ensure the commit is available
			repo.git.checkout(rev)
		except GitCommandError:
			print(f"Error: Unable to checkout on commit {rev}.")


	#####
	# Make sure the 3rd party folder exists

	if not os.path.isdir(third_party_dir):
		os.mkdir(third_party_dir)


	#####
	# Install thingset

	cloneRepo("https://github.com/ThingSet", "thingset-node-c", thingset_node_rev)
	cloneRepo("https://github.com/ThingSet", "thingset-zephyr-sdk", thingset_sdk_rev)
