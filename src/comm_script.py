"""
Copyright (c) 2021-2024 LAAS-CNRS

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 2.1 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

SPDX-License-Identifier: LGLPV2.1
"""

"""
@brief  This is a communcication script that deploys a PV emulator with
        hardware-in-the-loop with a Twitst 1.4.1

@author Luiz Villa <luiz.villa@laas.fr>
@author Thomas Walter <thomas.walter@laas.fr>
@author Guillaume Arthaud <guillaume.arthaud@laas.fr>
@author Amalie Alchami <amalie.alchami@utc.fr>
"""

import serial
import os
import sys
parent_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../"))
sys.path.insert(0, parent_dir)


from owntech.lib.USB.comm_protocol.src import find_devices
from owntech.lib.USB.comm_protocol.src.Shield_Class import Shield_Device

import matplotlib.pyplot as plt
import matplotlib.animation as animation

import xmlrpc.client as xml
import time
import matplotlib.pyplot as plt
import numpy as np

leg_to_test = "LEG1"                               #leg to be tested in this script
reference_names = ["V1","V2","VH","I1","I2","IH"]  #names of the sensors of the board

shield_vid = 0x2fe3
shield_pid = 0x0101

Shield_ports = find_devices.find_shield_device_ports(shield_vid, shield_pid)
print(Shield_ports)

Shield = Shield_Device(shield_port= Shield_ports[0], shield_type='OWNVERTER')


#Reference
#-----------MPPT variables------------------------------------------------------------------------------------------------------
ref_base_value =5
ref_step = 0.5
ref_max_value = 15
reference = ref_base_value # initializes the reference

# ----- Create a figure and axis for an animation window -----------------------------------------------------------------------------------------------------------
fig, ax = plt.subplots()
line1, = ax.plot([], [], lw=2, label='V1')                      # Current I1 Plot
line2, = ax.plot([], [], lw=2, label='V2')                      # Current I2 Plot
frame_limit = 200                                               # Set the frame limit of the animation window
xdata, ydata1, ydata2 = [], [], []      # xdata : time, ydata1 : V1, ydata2 : V2

# Set up the plot parameters
ax.set_xlim(0, frame_limit)
ax.legend()
ax.set_ylim(-100, 100)
ax.set_xlabel('Time')
ax.set_ylabel('Value')
ax.set_title('Real-time Plot')

# Function to initialize the plot
def init():
    line1.set_data([], [])
    line2.set_data([], [])
    return line1, line2

# Function to update the plot
def update(frame):

  global reference

  if frame == frame_limit:                                      # Clear Plot when frame limit is reached
      xdata.clear()
      ydata1.clear()
      ydata2.clear()
      ax.set_xlim(frame, frame + frame_limit)

  else:

    xdata.append(frame)

    reference = reference + ref_step
    if reference == ref_max_value : reference = ref_base_value

    Shield.sendCommand("REFERENCE","LEG1","V1",reference)            # Send Voltage reference to LEG 1
    Shield.sendCommand("REFERENCE","LEG2","V2",reference)            # Send Voltage reference to LEG 2
    time.sleep(10e-3)                                           # Pause to ensure stability

    # Measurement to Plot in Real Time
    ydata1.append(Shield.getMeasurement('V1')  )                  # Append the I1 measure to ydata1
    ydata2.append(Shield.getMeasurement('V2'))                    # Append the I2 measure to ydata2
    # Add to the line
    line1.set_data(xdata, ydata1)
    line2.set_data(xdata, ydata2)

  return line1, line2


# ---------------HARDWARE IN THE LOOP PV EMULATOR CODE ------------------------------------
message1 = Shield.sendCommand("IDLE")
print(message1)

message = Shield.sendCommand( "BUCK", "LEG1", "ON")
print(message)

message = Shield.sendCommand( "BUCK", "LEG2", "ON")
print(message)

message = Shield.sendCommand("LEG","LEG1","ON")
print(message)

message = Shield.sendCommand("LEG","LEG2","ON")
print(message)

message1 = Shield.sendCommand("REFERENCE","LEG1","V1",5)
print(message1)

message = Shield.sendCommand("POWER_ON")
print(message)


try:
  ani = animation.FuncAnimation(fig, update, frames=range(frame_limit), init_func=init, blit=True)
  plt.grid()
  plt.show()
finally:
  message1 = Shield.sendCommand("IDLE")
  print(message1)
