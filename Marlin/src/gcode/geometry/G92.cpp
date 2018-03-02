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

#include "../gcode.h"
#include "../../module/motion.h"
#include "../../module/stepper.h"

#if ENABLED(I2C_POSITION_ENCODERS)
  #include "../../feature/I2CPositionEncoder.h"
#endif

/**
 * G92: Set current position to given X Y Z E
 */
void GcodeSuite::G92() {

  stepper.synchronize();

  #if ENABLED(CNC_COORDINATE_SYSTEMS)
    switch (parser.subcode) {
      case 1:
        // Zero the G92 values and restore current position
        #if !IS_SCARA
          LOOP_XYZ(i) {
            const float v = position_shift[i];
            if (v) {
              position_shift[i] = 0;
              update_software_endstops((AxisEnum)i);
            }
          }
        #endif // Not SCARA
        return;
    }
  #endif

  #if ENABLED(CNC_COORDINATE_SYSTEMS)
    #define IS_G92_0 (parser.subcode == 0)
  #else
    #define IS_G92_0 true
  #endif

  bool didE = false;
  #if IS_SCARA || !HAS_POSITION_SHIFT
    bool didXYZ = false;
  #else
    constexpr bool didXYZ = false;
  #endif

  if (IS_G92_0) LOOP_XYZE(i) {
    if (parser.seenval(axis_codes[i])) {
      const float l = parser.value_axis_units((AxisEnum)i),
                  v = i == E_AXIS ? l : LOGICAL_TO_NATIVE(l, i),
                  d = v - current_position[i];
      if (!NEAR_ZERO(d)) {
        #if IS_SCARA || !HAS_POSITION_SHIFT
          if (i == E_AXIS) didE = true; else didXYZ = true;
          current_position[i] = v;        // Without workspaces revert to Marlin 1.0 behavior
        #elif HAS_POSITION_SHIFT
          if (i == E_AXIS) {
            didE = true;
            current_position[E_AXIS] = v; // When using coordinate spaces, only E is set directly
          } else {
            position_shift[i] += d;       // Other axes simply offset the coordinate space
            update_software_endstops((AxisEnum)i);
          }
        #endif
      }
    }
  }

  #if ENABLED(CNC_COORDINATE_SYSTEMS)
    // Apply workspace offset to the active coordinate system
    if (WITHIN(active_coordinate_system, 0, MAX_COORDINATE_SYSTEMS - 1))
      COPY(coordinate_system[active_coordinate_system], position_shift);
  #endif

  bool doZAdjust = false;
  float adjust_z = 0.0;



  if(parser.seenval('A')){
    adjust_z = parser.value_axis_units(Z_AXIS);
    doZAdjust = true;
  }else if(parser.seenval('B')){
    float final_z = parser.value_axis_units(Z_AXIS);


    SERIAL_ECHO_START();
    SERIAL_ECHOLNPAIR("final_z: ", final_z);
    SERIAL_ECHOLNPAIR("primaryZTO: ", primaryZTO);
    SERIAL_ECHOLNPAIR("secondaryZTO: ", secondaryZTO);



	if (active_extruder == 0) {
		adjust_z = final_z - primaryZTO;
	}
	else {
		adjust_z = final_z - secondaryZTO;
	}
    doZAdjust = true;

    SERIAL_ECHOLNPAIR("adjust_z: ", adjust_z);

  }

  if (doZAdjust) {
    float curr_z = current_position[Z_AXIS];

	if (adjust_z != 0) {
        float hop_position = max(0, adjust_z) + curr_z + Z_ADJUST_HOP_DISTANCE;

        do_blocking_move_to_z(hop_position, HOMING_FEEDRATE_Z);
        do_blocking_move_to_z(curr_z + adjust_z, HOMING_FEEDRATE_Z);
	}

    current_position[Z_AXIS] = curr_z;

    if (active_extruder == 0) {
        primaryZTO += adjust_z;
    }
    else {
        secondaryZTO += adjust_z;
    }

    //report
    SERIAL_ECHO_START();
    SERIAL_ECHOLNPAIR("adjust_z(now): ", adjust_z);
    SERIAL_ECHOLNPAIR("primaryZTO: ", primaryZTO);
    SERIAL_ECHOLNPAIR("secondaryZTO: ", secondaryZTO);
    SERIAL_ECHOLNPAIR("PrimaryZTOEprom: ", primaryZTOEprom);
    SERIAL_ECHOLNPAIR("SecondaryZTOEprom: ", secondaryZTOEprom);
  }

  if(parser.seen('I')){
    float curr_z = current_position[Z_AXIS];
    if(active_extruder == 0 && curr_z >= -(primaryZTOEprom - primaryZTO)){
      adjust_z = primaryZTOEprom - primaryZTO;

      if(adjust_z != 0){
        doZAdjust = true;
        float hop_position = max(0, adjust_z) + curr_z + Z_ADJUST_HOP_DISTANCE;

        do_blocking_move_to_z(hop_position, HOMING_FEEDRATE_Z);
        do_blocking_move_to_z(curr_z + adjust_z, HOMING_FEEDRATE_Z);

        current_position[Z_AXIS] = curr_z;
        primaryZTO = primaryZTOEprom;
        secondaryZTO = secondaryZTOEprom;
      }
    }else{
      SERIAL_ECHO_START();
      SERIAL_PROTOCOLLNPGM("Switch to T0 first, and move Z higher than primary ZTO.).\n");
    }
  }

  bool didR = parser.seen('R');
  if (didR){
    primaryZTO = 0.0;
    secondaryZTO = 0.0;
    SERIAL_ECHO_START();
    SERIAL_PROTOCOLLNPGM("Z toolhead offsets reset to 0. ");
  }

  if (didXYZ || doZAdjust)
    SYNC_PLAN_POSITION_KINEMATIC();
  else if (didE)
    sync_plan_position_e();

  report_current_position();
}
