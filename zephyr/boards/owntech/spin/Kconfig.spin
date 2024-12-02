# Copyright (c) 2023-2024 OwnTech
# SPDX-License-Identifier: LGPL-2.1

# OwnTech board Kconfig definition
# This file declares the board to Kconfig
# and selects the correct SOC.

config BOARD_SPIN
	select SOC_STM32G474XX
