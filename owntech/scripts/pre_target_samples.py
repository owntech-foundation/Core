import helper_functions
import os
import shutil
import subprocess
import functools

Import("env")

sample_dir = os.path.join(".", "owntech", "samples")
src_dir = os.path.join(".", "src")
old_dir = os.path.join(".", "old")

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
        "name": "Buck_Voltage_Mode-RC1",
        "title": "Voltage Mode Buck",
        "description": "Voltage mode buck converter using PID controller for TWIST",
        "group": "Samples TWIST",
        "file": "RC1.zip",
        "sample_url": "https://github.com/owntech-foundation/Buck_Voltage_Mode/archive/refs/tags/RC1.zip",
    },
    # Add more targets as needed
]

def pullSampleGeneric(extra_arg, target, source, env):
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

for target_info in targets:
    pullSampleSpecific=create_pull_sample_function(target_info)
    env.AddTarget(
        name=target_info["name"],
        dependencies=None,
        actions=env.VerboseAction(pullSampleSpecific, "Retrieving desired sample.."),
        title=target_info["title"],
        description=target_info["description"],
        group=target_info["group"],
        always_build=False,
    )
