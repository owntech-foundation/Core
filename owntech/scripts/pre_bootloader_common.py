Import("env")

# Make sure mcuboot-image is in the target list when uploading,
# because pio scripts will behave incorrectly if it isn't
# (wrong file gets uploaded)
if "mcuboot-image" not in COMMAND_LINE_TARGETS:
	COMMAND_LINE_TARGETS.insert(0, "mcuboot-image")
