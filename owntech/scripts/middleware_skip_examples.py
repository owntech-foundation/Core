import os

Import("env")

# This script permits to remove library files to be compiled
# Without removing libdep in platformio.ini
# As such owntech_examples are downloaded without being built

def skip_from_build(node):
    """
    `node.name` - a name of File System Node
    `node.get_path()` - a relative path
    `node.get_abspath()` - an absolute path
     to ignore file from a build process, just return None
    """
    if "owntech_examples" in node.get_path():
        # Return None for exclude
        return None
    return node

# Register callback
env.AddBuildMiddleware(skip_from_build, "*")