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

## @brief This file is a python script that permits to write recorded data in an output txt file.
# It defines and implements a custom platformio monitor filter.
# Get more info @ https://docs.platformio.org/en/latest/core/userguide/device/cmd_monitor.html#filters
#
# @author Régis Ruelland <regis.ruelland@laas.fr>
# @author Jean Alinei <jean.alinei@owntech.org>

from platformio.public import DeviceMonitorFilterBase
from datetime import datetime
import struct
import os
import io

# Import or install python package in platformio python environnement.
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


IDLE = 0
RECORD = 1

class RecordedDatas(DeviceMonitorFilterBase):
    """
    1. It waits for "begin record" character sequence
    2. then we assume each line as a 8 bytes in hexa
    the 8 bytes are translated to float and recorded in a file.
    3. It waits for "end record" character sequence to finish writing file.
    """
    NAME="recorded_datas"

    # TODO: make more test, because not sure of its robustness... :-/

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.f = None
        self.state = IDLE
        self.buffer = ""
        self.f = io.StringIO()

        print("recorded filter is loaded")

    def rx(self, text):
        self.buffer += text
        if '\n' in self.buffer:
            datas_left = ''
            cr_idx = self.buffer.rfind('\n')
            txt_to_read = self.buffer[:cr_idx]
            if 'begin record' in txt_to_read:
                idx = self.buffer.find('begin record\r\n') # FIXME: no so clear with \n
                txt_to_read = txt_to_read[idx+14:]
                self.state = RECORD

            if 'end record' in txt_to_read:
                idx = txt_to_read.find('end record')
                txt_to_read = txt_to_read[:idx]
                lines = txt_to_read.split("\r\n")
                datas_left = self.save_datas(lines)
                self.state  = IDLE
                path_records = os.path.join(".","src","Data_records")
                if not os.path.exists(path_records):
                    os.makedirs(path_records)
                file_name = f"{datetime.now().strftime('%Y-%m-%d_%H-%M-%S')}-record"
                file_to_record = os.path.join(path_records,file_name)
                with open(file_to_record+".txt", "w+") as ff:
                    ff.write(self.f.getvalue())
                self.f.close()
                self.f = io.StringIO()

                data_frame = self.to_dataFrame(file_to_record+".txt")
                data_frame.to_csv(file_to_record+".csv")
                figure = self.plot_df(data_frame)
                figure.savefig(file_to_record+".png")


            if self.state == RECORD:
                lines = txt_to_read.split("\r\n")
                datas_left = self.save_datas(lines)

            self.buffer = datas_left + self.buffer[cr_idx:]
        return text

    def tx(self, text):
        return text

    def __del__(self):
        if self.f:
            if not self.f.closed:
                self.f.close()


    def save_datas(self, lines):
        datas_left = ''
        # the last data should be ''
        if lines[-1] != '':
            datas_left = lines.pop()
        else:
            lines.pop()
        for k, line in enumerate(lines):
            if "#" in line:
                self.f.write("{}\n".format(line[1:]))
            else:
                try:
                    nombre = struct.unpack('>f', bytes.fromhex(line))[0]
                    self.f.write("{}\n".format(nombre))
                except:
                    nombre = 0.0
                    self.f.write("{}\n".format(line))
        return datas_left

    def to_dataFrame(self, filename):
        """ filename is the name of file got datas dump from a ScopeMimicry object
        the format is : two first line with # at beginning
        in the first line there's a list of column names
        in the second line a number: the index of the final data recorded
        each other lines are 8 bytes in hex format and represent float datas.
        """
        # Get the directory of the current script
        # script_dir = os.path.dirname(os.path.abspath(__file__))

        # # Navigate to the Data_records directory
        # data_records_dir = os.path.join(script_dir, 'Data_records')
        # file_path = os.path.join(data_records_dir, filename)

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


    def plot_df(self, df):
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
