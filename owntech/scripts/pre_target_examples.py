import helper_functions
import os
import json
import click
import shutil
import subprocess

#Retrieves list target names from pio command line in CLI.
from SCons.Script import COMMAND_LINE_TARGETS  # pylint: disable=import-error

Import("env")

#Build path to library.json using the right environement.
lib_path = os.path.join(".", "owntech", "lib", env.Dump("PIOENV").strip("''"), "owntech_examples")
json_dir = os.path.join(lib_path, "library.json")

# Execute this script only if owntech_examples library.json exist
if os.path.exists(json_dir):

    #Extract examples paths from library.json
    with open(json_dir, 'r') as json_file:
        data = json.load(json_file)

    #Define a list of targets based on Examples in library.json.
        targets = []
    # Extract the "base" property and create a path using os.path.join
    for example in data["examples"]:
        target_info = {
            "name": example["name"],
            "title": example["title"],
            "group": example["group"],
            "description": example["description"],
            "base": example["base"]
        }
        targets.append(target_info)


    src_dir = os.path.join(".", "src")
    old_dir = os.path.join(".", "old")
    build_dir = os.path.join(".", ".pio", "build")


    def pullSampleGeneric(example_base):

        if example_base is None:
            print("No target_info found for target")

        # Check and create "old" folders incrementally to save current src/ folder
        old_folder_count = 0
        while os.path.exists(os.path.join(old_dir, f"old{old_folder_count}")):
            old_folder_count += 1

        current_old_dir = os.path.join(old_dir, f"old{old_folder_count}")
        os.makedirs(current_old_dir)

        if os.path.exists(src_dir):
            shutil.move(src_dir, current_old_dir)
        print(f"Previous main saved in {current_old_dir}")

        example_path = os.path.join(lib_path, example_base)
        print(example_path)
        shutil.copytree(example_path, src_dir)

    #Dummy function to print a user friendly message using env.VerboseAction() 
    #After successfully loading an example.
    def PrintSuccess(target, source, env):
        return

    #Defines the targets for each example referenced in library.json and create a nice GUI.
    for target_info in targets:
        env.AddTarget(
            name=target_info["name"],
            dependencies=None,
            actions=env.VerboseAction(PrintSuccess,"Example succesfully loaded and built, enjoy !"),
            title=target_info["title"],
            description=target_info["description"],
            group=target_info["group"],
            always_build=False,
        )

    for tgt_info in targets: #Go through all example targets
        if tgt_info["name"] in COMMAND_LINE_TARGETS: #If an example target is launched via GUI or CLI
            pullSampleGeneric(tgt_info["base"]) #Executes the function that retrieves example and place it in src folder
            shutil.rmtree(build_dir) #Removes build folder in order to trigger a clean compilation