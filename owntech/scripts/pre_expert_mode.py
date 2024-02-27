import os
import json
import click 

#Retrieves list target names from pio command line in CLI.
from SCons.Script import COMMAND_LINE_TARGETS  # pylint: disable=import-error


Import("env")

# Specify the path to your settings.json file
settings_json_path = os.path.join(".", ".vscode", "settings.json")

def toggle_booleans_in_file(file_path):
    # Load the JSON content from the file
    with open(file_path, 'r') as file:
        settings_data = json.load(file)

    # Function to toggle boolean values in the JSON recursively
    def toggle_booleans(data):
        if isinstance(data, dict):
            for key, value in data.items():
                if isinstance(value, bool):
                    data[key] = not value  # Toggle boolean value
                elif isinstance(value, (dict, list)):
                    toggle_booleans(value)

        elif isinstance(data, list):
            for i, item in enumerate(data):
                if isinstance(item, (dict, list)):
                    toggle_booleans(item)

    # Toggle boolean values in the loaded JSON data
    toggle_booleans(settings_data)

    # Save the modified JSON back to the file
    with open(file_path, 'w') as file:
        json.dump(settings_data, file, indent=2)

    print("Boolean values toggled and saved successfully.")

#Dummy function to print a user friendly message using env.VerboseAction() 
#After successfully loading an example.
def PrintSuccess(target, source, env):
    return

env.AddTarget(
    name="expert",
    dependencies=None,
    actions=env.VerboseAction(PrintSuccess,"Toggling Expert Mode !"),
    title="[Advanced] Toggle Expert Mode",
    description="Expert mode allows to see Zephyr RTOS environment",
    group="OwnTech",
    always_build=False,
)

if "expert" in COMMAND_LINE_TARGETS:
    toggle_booleans_in_file(settings_json_path)
    exit(0)