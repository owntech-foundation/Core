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
try: 
    import tkinter as tk
    from tkinter import filedialog
except ImportError:
    env.Execute("$PYTHONEXE -m pip install tkinter")

# Function to open file dialog and plot data
def open_record():
    # Create a root window and hide it
    root = tk.Tk()
    root.withdraw()
    if os.path.exists("./src/Data_records"):
        default_path = os.path.join('.', 'src', 'Data_records')
    else: 
        default_path = os.path.join('.')
    # Open file dialog and get the file path
    file_path = filedialog.askopenfilename(
        title="Select a file",
        initialdir=default_path,
        filetypes=(("txt file", "*.txt"), ("All files", "*.*"))
    )
    if file_path:
        return file_path
    else:
        print("No file selected, aborting")
        return -1


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
    record = open_record()
    if record != -1:
        df = to_dataFrame(record)
        fig = plot_df(df)
        fig.suptitle(record)
        plt.show()
    exit(0)