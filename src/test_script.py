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

def repeat_get_line(shield, num_times):
    """
    Calls shield.getLine() and prints the result the specified number of times.

    Args:
        shield: The object that has the getLine() method (e.g., Shield).
        num_times (int): The number of times to call getLine() and print the result.
    """
    for _ in range(num_times):
        message = shield.getLine()
        print(message)

# Example usage:
# repeat_get_line(Shield, 5)  # Calls Shield.getLine() 5 times and prints each message



leg_to_test = "LEG1"                               #leg to be tested in this script
reference_names = ["V1","V2","VH","I1","I2","IH"]  #names of the sensors of the board

shield_vid = 0x2fe3
shield_pid = 0x0101

Shield_ports = find_devices.find_shield_device_ports(shield_vid, shield_pid)
print(Shield_ports)

Shield = Shield_Device(shield_port= Shield_ports[0], shield_type='TWIST')

try:

  # ---------------HARDWARE IN THE LOOP PV EMULATOR CODE ------------------------------------
  message1 = Shield.sendCommand("IDLE")
  print(message1)

  message = Shield.sendCommand( "BUCK", "LEG1", "OFF")
  print(message)

  message = Shield.sendCommand( "BUCK", "LEG2", "OFF")
  print(message)

  message = Shield.sendCommand("LEG","LEG1","ON")
  print(message)

  message = Shield.sendCommand("LEG","LEG2","ON") 
  print(message)

  message = Shield.sendCommand("POWER_ON")
  print(message)

  message = Shield.sendCommand("DUTY","LEG1",1.0)
  print(message)

  message = Shield.sendCommand("DUTY","LEG2",1.0)
  print(message)

  # duty_cycle = 0.5  # Initial duty cycle value

  # user_input = input("Press enter to continue")


  # message1 = Shield.sendCommand("PHASE_SHIFT","LEG2",0)
  # print(message1)

  # for phase_shift_value in range(180, 0, -10):  # Example values from 0 to 170 with a step of 10
  #     message1 = Shield.sendCommand("PHASE_SHIFT", "LEG2", phase_shift_value)
  #     print(f"Sent command with phase shift: {phase_shift_value}, message: {message1}")





finally:
  # message1 = Shield.sendCommand("IDLE")
  print(message1)


