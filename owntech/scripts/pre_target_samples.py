import helper_functions
import os
import click
import shutil
import subprocess
import functools

from SCons.Script import COMMAND_LINE_TARGETS  # pylint: disable=import-error

Import("env")

sample_dir = os.path.join(".", "owntech", "samples")
src_dir = os.path.join(".", "src")
old_dir = os.path.join(".", "old")
build_dir = os.path.join(".", ".pio", "build")

targets = [
    {
        "name": "spin-blinky",
        "title": "OwnTech blinky",
        "description": "Blinky is the simplest example, it is a good starting point.",
        "group": "Samples SPIN",
        "file": "RC1.zip",
        "sample_url": "https://github.com/foobar/RC1.zip",
    },
    {
        "name": "example_buck_voltage_mode-RC1",
        "title": "Voltage Mode Buck",
        "description": "Voltage mode buck converter using PID controller for TWIST",
        "group": "Samples TWIST",
        "file": "RC1.zip",
        "sample_url": "https://github.com/owntech-foundation/Buck_Voltage_Mode/archive/refs/tags/RC1.zip",
    },
    {
        "name": "example_twist_boost_voltage_mode-RC1",
        "title": "Voltage Mode Boost",
        "description": "Voltage mode boost converter using PID controller for TWIST",
        "group": "Samples TWIST",
        "file": "RC1.zip",
        "sample_url": "https://github.com/owntech-foundation/example_twist_boost_voltage_mode/archive/refs/tags/RC1.zip",
    },
    {
        "name": "example_twist_interleaved-RC1",
        "title": "Interleaved Mode Buck",
        "description": "Interleaved mode buck converter using PID controller for TWIST",
        "group": "Samples TWIST",
        "file": "RC1.zip",
        "sample_url": "https://github.com/owntech-foundation/example_twist_interleaved/archive/refs/tags/RC1.zip",
    },
    # Add more targets as needed
]

def pullSampleGeneric(extra_arg):
    target_info = extra_arg    
    if target_info is None:
        print("No target_info found for target")
    file = target_info["file"]
    sample_url = target_info["sample_url"]
    res = helper_functions.check_file_and_download(target_info["name"], sample_dir, file, sample_url)
    if res != 0:
        exit(-1)
    file_path = os.path.join(sample_dir, file)
    helper_functions.unzip_file(file_path, sample_dir)
    print("Archive unzipped..")

    
    # Check and create "old" folders incrementally
    old_folder_count = 0
    while os.path.exists(os.path.join(old_dir, f"old{old_folder_count}")):
        old_folder_count += 1

    current_old_dir = os.path.join(old_dir, f"old{old_folder_count}")
    os.makedirs(current_old_dir)

    if os.path.exists(src_dir):
        shutil.move(src_dir, current_old_dir)
    print(f"Previous main saved in {current_old_dir}")

    sample_src_path = os.path.join(sample_dir, target_info["name"], "src")
    shutil.move(sample_src_path, src_dir)
    
    sample_folder_path = os.path.join(sample_dir, target_info["name"])
    os.remove(file_path)
    shutil.rmtree(sample_folder_path)
    print("Cleaning temporary files..")

def create_pull_sample_function(arg_value):
    return functools.partial(pullSampleGeneric, extra_arg=arg_value)

def get_pull_sample_function(target_name):
    for target_info in targets:
        if target_info["name"] == target_name:
            return create_pull_sample_function(target_info)

def PrintSuccess(target, source, env):
    return

# Add the build middleware callback

for target_info in targets:
    env.AddTarget(
        name=target_info["name"],
        dependencies=None,
        actions=env.VerboseAction(PrintSuccess,"Example succesfully loaded and built, enjoy !"),
        title=target_info["title"],
        description=target_info["description"],
        group=target_info["group"],
        always_build=True,
    )

for targets_info in targets: 
    if targets_info["name"] in COMMAND_LINE_TARGETS:
        pullSampleGeneric(target_info)
        shutil.rmtree(build_dir)