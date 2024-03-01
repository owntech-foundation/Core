/*
 * Copyright (c) 2022-2023 LAAS-CNRS
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 2.1 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGLPV2.1
 */

/**
 * @date   2023
 * @author Clément Foucher <clement.foucher@laas.fr>
 * @author Luiz Villa <luiz.villa@laas.fr>
 */

#ifndef VERSIONHAL_H_
#define VERSIONHAL_H_




/** Hardware version. See
 *  https://gitlab.laas.fr/owntech/1leg/-/wikis/Releases
 *  for the list and specificities of versions.
 */
typedef enum
{
	nucleo_G474RE,
	O2_v_0_9,
	O2_v_1_1_2,
	SPIN_v_0_1,
	SPIN_v_0_9,
	SPIN_v_1_0,
	TWIST_v_1_1_2,
	TWIST_v_1_1_3,
	TWIST_v_1_1_4
} board_version_t;



/**
 * @brief  Handles the versioning for the spin board
 *
 * @note   Use this element to defined your version of the SPIN board
 */
class VersionHAL
{
public:

	/**
	 * @brief Sets the version of the microcontroller board.
	 * 	      It can take the following values:
	 * 	      	nucleo_G474RE, 	O2_v_0_9, O2_v_1_1_2,
	 * 			SPIN_v_0_1, SPIN_v_0_9, SPIN_v_1_0, TWIST_v_1_1_2
	 *          TWIST_v_1_1_3, TWIST_v_1_1_4 
	 *
	 *        Stand alone versions such as SPIN_v_X comprise only the spin board itself.
	 * 		  Legacy versions such as nucleo or O2_X comprise the nucleo board or the old O2 boards.
	 * 		  Twist versions comprise all the available Twist connected versions.
	 * @param board_version Enum representing the microcontroller version.
	 */
	void setBoardVersion(board_version_t board_version);
	

	/**
	 * @brief return board version
	 * 
	 * @return Return one of these values : 
	 * 	      	nucleo_G474RE, 	O2_v_0_9, O2_v_1_1_2,
	 * 			SPIN_v_0_1, SPIN_v_0_9, SPIN_v_1_0, TWIST_v_1_1_2
	 *          TWIST_v_1_1_3, TWIST_v_1_1_4 
	*/
	board_version_t getBoardVersion();


private:

	static board_version_t board_version;


};



#endif // VERSIONHAL_H_
