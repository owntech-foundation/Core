##  Copyright (c) 2021-2024 LAAS-CNRS
# 
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 2.1 of the License, or
#    (at your option) any later version.
# 
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
# 
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.
# 
#  SPDX-License-Identifier: LGPL-2.1

## @brief This file is a python script that permits to plot recorded data from an output txt file.
#
# @author Régis Ruelland <regis.ruelland@laas.fr>

Import("env")
import os
import re

#Retrieves list target names from pio command line in CLI.
from SCons.Script import COMMAND_LINE_TARGETS  # pylint: disable=import-error

# Import or install python package in platformio python environnement.
try: 
    import argparse
except ImportError:
    env.Execute("$PYTHONEXE -m pip install argparse")
try: 
    import numpy as np
except ImportError:
    env.Execute("$PYTHONEXE -m pip install numpy")
try: 
    import matplotlib.pyplot as plt
except ImportError:
    env.Execute("$PYTHONEXE -m pip install matplotlib")
try: 
    import pandas as pd
except ImportError:
    env.Execute("$PYTHONEXE -m pip install pandas")

def list_records():
    # Define the pattern to match the filenames
    pattern = re.compile(r'^\d{4}-\d{2}-\d{2}_\d{2}-\d{2}-\d{2}-record\.txt$')
    # List all files in the current directory
    data_records_path = os.path.join(".", "src", "Data_records")
    files = os.listdir(data_records_path)
    # Filter files that match the pattern
    record_files = [file for file in files if pattern.match(file)]

    return record_files


def to_dataFrame(filename):
    """ filename is the name of file got datas dump from a ScopeMimicry object 
    the format is : two first line with # at beginning
    in the first line there's a list of column names
    in the second line a number: the index of the final data recorded
    each other lines are 8 bytes in hex format and represent float datas.
    """
    with open(filename, "r") as f:
        file = f.readlines()
    line1 = file.pop(0)
    if "#" in file[0] or file[0].startswith(" "):
        line2 = file.pop(0)
        idx = int(line2.replace("#", "").strip())
    else:
        idx = None
    datas = np.fromiter(file, dtype=float)
    names = line1.replace("#", "").split(",")
    datas = datas.reshape(-1, len(names) - 1)
    if idx:
        datas = np.roll(datas, -(idx + 1), axis=0)
    df = pd.DataFrame(datas, columns=names[:-1])
    return df


def plot_df(df):
    """ plot dataframe in 3 different axes 
    first axes are voltages
    second axe are currents
    third are others...
    by convention we begin column name with 'V' when it is voltage
    we begin column name with 'I' when it is current
    """
    fig, axs = plt.subplots(3, 1)
    tics = np.arange(len(df))
    try:
        df["V_Low_estim"] = df["duty_cycle"] * df["V_high"]
    except:
        pass
    if "k_acquire" in df:
        del df["k_acquire"]
    for s in df:
        if s.startswith("V"):
            axs[0].step(tics, df[s], label=s)
        elif s.startswith("I"):
            axs[1].step(tics, df[s], label=s)
        else:
            axs[2].step(tics, df[s], label=s)
    for ax in axs:
        ax.legend()
        ax.grid()
    return fig

# if __name__ == "__main__":
#     parser = argparse.ArgumentParser("plot_records", "plot_records <filename>")
#     parser.add_argument("filename")
#     args = parser.parse_args()
#     df = to_dataFrame(args.filename)
#     fig = plot_df(df)
#     fig.suptitle(args.filename)
#     plt.show()


#Dummy function to print a user friendly message using env.VerboseAction() 
#After successfully loading an example.
def PrintSuccess(target, source, env):
    return

env.AddTarget(
    name="plot-record",
    dependencies=None,
    actions=env.VerboseAction(PrintSuccess,"Plotting record"),
    title="Plot recording",
    description="Plot one of scopeMimicry recordings",
    group="OwnTech",
    always_build=False,
)

if "plot-record" in COMMAND_LINE_TARGETS:
    records = list_records()
    for i,record in enumerate(records):
        print(f"record n°{i}, name {record}")
    print("Enter record number to plot")
    try:
        record_number = int(input())
        print("The record number is:", record_number)
    except ValueError:
        print("Invalid input. Please enter a valid integer.")
    df = to_dataFrame(os.path.join(".", "src", "Data_records",records[record_number]))
    fig = plot_df(df)
    fig.suptitle(records[record_number])
    plt.show()

    exit(0)