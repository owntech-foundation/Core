Import("env")

def upload_custom_file(source, target, env):
	# Dirty trick to replace firmware name in upload command...
	upload_command = env.get("UPLOADERFLAGS")
	for i in range(len(upload_command)):
		if "{$SOURCE}" in upload_command[i]:
			upload_command[i] = upload_command[i].replace("{$SOURCE}", "${BUILD_DIR}/${PROGNAME}.mcuboot.bin")

env.AddPreAction("upload", upload_custom_file)
