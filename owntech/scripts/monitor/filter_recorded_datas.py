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
                file_name = f"{datetime.now().strftime('%Y-%m-%d_%H-%M-%S')}-record.txt"
                with open(os.path.join(path_records,file_name), "w+") as ff:
                    ff.write(self.f.getvalue())
                self.f.close()
                self.f = io.StringIO()

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
