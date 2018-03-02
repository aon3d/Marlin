/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// AON3D M2 Chamber heating thermistor
const short temptable_86[][2] PROGMEM = {
	{ 2621 ,  180 },
	{ 2864 ,  175 },
	{ 3154 ,  170 },
	{ 3458 ,  165 },
	{ 3785 ,  160 },
	{ 4136 ,  155 },
	{ 4478 ,  150 },
	{ 4886 ,  145 },
	{ 5332 ,  140 },
	{ 5801 ,  135 },
	{ 6299 ,  130 },
	{ 6800 ,  125 },
	{ 7370 ,  120 },
	{ 7923 ,  115 },
	{ 8494 ,  110 },
	{ 9076 ,  105 },
	{ 9626 ,  100 },
	{ 10206 ,  95 },
	{ 10793 ,  90 },
	{ 11360 ,  85 },
	{ 11906 ,  80 },
	{ 12415 ,  75 },
	{ 12910 ,  70 },
	{ 13363 ,  65 },
	{ 13778 ,  60 },
	{ 14150 ,  55 },
	{ 14473 ,  50 },
	{ 14781 ,  45 },
	{ 15046 ,  40 },
	{ 15274 ,  35 },
	{ 15470 ,  30 },
	{ 15633 ,  25 },
	{ 15776 ,  20 },
	{ 15894 ,  15 },
	{ 15992 ,  10 },
	{ 16072 ,  5 },
	{ 16136 ,  0 },
};
