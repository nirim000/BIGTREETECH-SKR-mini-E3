/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../gcode.h"
#include "../../inc/MarlinConfig.h"
#include "../queue.h"           // for getting the command port


#if ENABLED(M115_GEOMETRY_REPORT)
  #include "../../module/motion.h"
#endif

#if ENABLED(CASE_LIGHT_ENABLE)
  #include "../../feature/caselight.h"
#endif

#if ENABLED(EXTENDED_CAPABILITIES_REPORT)
  static void cap_line(PGM_P const name, bool ena=false) {
    SERIAL_ECHOPGM("Cap:");
    SERIAL_ECHOPGM_P(name);
    SERIAL_CHAR(':', '0' + ena);
    SERIAL_EOL();
  }
#endif

/**
 * M115: Capabilities string and extended capabilities report
 *       If a capability is not reported, hosts should assume
 *       the capability is not present.
 */
void GcodeSuite::M115() {
  SERIAL_ECHOLNPGM(
    "FIRMWARE_NAME:Marlin " DETAILED_BUILD_VERSION " (" __DATE__ " " __TIME__ ") "
    "SOURCE_CODE_URL:" SOURCE_CODE_URL " "
    "PROTOCOL_VERSION:" PROTOCOL_VERSION " "
    "MACHINE_TYPE:" MACHINE_NAME " "
    "EXTRUDER_COUNT:" STRINGIFY(EXTRUDERS) " "
    #ifdef MACHINE_UUID
      "UUID:" MACHINE_UUID
    #endif
  );

  #if ENABLED(EXTENDED_CAPABILITIES_REPORT)

    // The port that sent M115
    serial_index_t port = queue.ring_buffer.command_port();

    // PAREN_COMMENTS
    TERN_(PAREN_COMMENTS, cap_line(PSTR("PAREN_COMMENTS"), true));

    // QUOTED_STRINGS
    TERN_(GCODE_QUOTED_STRINGS, cap_line(PSTR("QUOTED_STRINGS"), true));

    // SERIAL_XON_XOFF
    cap_line(PSTR("SERIAL_XON_XOFF"), ENABLED(SERIAL_XON_XOFF));

    // BINARY_FILE_TRANSFER (M28 B1)
    cap_line(PSTR("BINARY_FILE_TRANSFER"), ENABLED(BINARY_FILE_TRANSFER)); // TODO: Use SERIAL_IMPL.has_feature(port, SerialFeature::BinaryFileTransfer) once implemented

    // EEPROM (M500, M501)
    cap_line(PSTR("EEPROM"), ENABLED(EEPROM_SETTINGS));

    // Volumetric Extrusion (M200)
    cap_line(PSTR("VOLUMETRIC"), DISABLED(NO_VOLUMETRICS));

    // AUTOREPORT_POS (M154)
    cap_line(PSTR("AUTOREPORT_POS"), ENABLED(AUTO_REPORT_POSITION));

    // AUTOREPORT_TEMP (M155)
    cap_line(PSTR("AUTOREPORT_TEMP"), ENABLED(AUTO_REPORT_TEMPERATURES));

    // PROGRESS (M530 S L, M531 <file>, M532 X L)
    cap_line(PSTR("PROGRESS"));

    // Print Job timer M75, M76, M77
    cap_line(PSTR("PRINT_JOB"), true);

    // AUTOLEVEL (G29)
    cap_line(PSTR("AUTOLEVEL"), ENABLED(HAS_AUTOLEVEL));

    // RUNOUT (M412, M600)
    cap_line(PSTR("RUNOUT"), ENABLED(FILAMENT_RUNOUT_SENSOR));

    // Z_PROBE (G30)
    cap_line(PSTR("Z_PROBE"), ENABLED(HAS_BED_PROBE));

    // MESH_REPORT (M420 V)
    cap_line(PSTR("LEVELING_DATA"), ENABLED(HAS_LEVELING));

    // BUILD_PERCENT (M73)
    cap_line(PSTR("BUILD_PERCENT"), ENABLED(LCD_SET_PROGRESS_MANUALLY));

    // SOFTWARE_POWER (M80, M81)
    cap_line(PSTR("SOFTWARE_POWER"), ENABLED(PSU_CONTROL));

    // TOGGLE_LIGHTS (M355)
    cap_line(PSTR("TOGGLE_LIGHTS"), ENABLED(CASE_LIGHT_ENABLE));
    cap_line(PSTR("CASE_LIGHT_BRIGHTNESS"), TERN0(CASE_LIGHT_ENABLE, caselight.has_brightness()));

    // EMERGENCY_PARSER (M108, M112, M410, M876)
    cap_line(PSTR("EMERGENCY_PARSER"), ENABLED(EMERGENCY_PARSER));

    // HOST ACTION COMMANDS (paused, resume, resumed, cancel, etc.)
    cap_line(PSTR("HOST_ACTION_COMMANDS"), ENABLED(HOST_ACTION_COMMANDS));

    // PROMPT SUPPORT (M876)
    cap_line(PSTR("PROMPT_SUPPORT"), ENABLED(HOST_PROMPT_SUPPORT));

    // SDCARD (M20, M23, M24, etc.)
    cap_line(PSTR("SDCARD"), ENABLED(SDSUPPORT));

    // REPEAT (M808)
    cap_line(PSTR("REPEAT"), ENABLED(GCODE_REPEAT_MARKERS));

    // SD_WRITE (M928, M28, M29)
    cap_line(PSTR("SD_WRITE"), ENABLED(SDSUPPORT) && DISABLED(SDCARD_READONLY));

    // AUTOREPORT_SD_STATUS (M27 extension)
    cap_line(PSTR("AUTOREPORT_SD_STATUS"), ENABLED(AUTO_REPORT_SD_STATUS));

    // LONG_FILENAME_HOST_SUPPORT (M33)
    cap_line(PSTR("LONG_FILENAME"), ENABLED(LONG_FILENAME_HOST_SUPPORT));

    // THERMAL_PROTECTION
    cap_line(PSTR("THERMAL_PROTECTION"), ENABLED(THERMALLY_SAFE));

    // MOTION_MODES (M80-M89)
    cap_line(PSTR("MOTION_MODES"), ENABLED(GCODE_MOTION_MODES));

    // ARC_SUPPORT (G2-G3)
    cap_line(PSTR("ARCS"), ENABLED(ARC_SUPPORT));

    // BABYSTEPPING (M290)
    cap_line(PSTR("BABYSTEPPING"), ENABLED(BABYSTEPPING));

    // CHAMBER_TEMPERATURE (M141, M191)
    cap_line(PSTR("CHAMBER_TEMPERATURE"), ENABLED(HAS_HEATED_CHAMBER));

    // COOLER_TEMPERATURE (M143, M193)
    cap_line(PSTR("COOLER_TEMPERATURE"), ENABLED(HAS_COOLER));

    // MEATPACK Compression
    cap_line(PSTR("MEATPACK"), SERIAL_IMPL.has_feature(port, SerialFeature::MeatPack));

    // Machine Geometry
    #if ENABLED(M115_GEOMETRY_REPORT)
      const xyz_pos_t bmin = { 0, 0, 0 },
                      bmax = { X_BED_SIZE , Y_BED_SIZE, Z_MAX_POS },
                      dmin = { X_MIN_POS, Y_MIN_POS, Z_MIN_POS },
                      dmax = { X_MAX_POS, Y_MAX_POS, Z_MAX_POS };
      xyz_pos_t cmin = bmin, cmax = bmax;
      apply_motion_limits(cmin);
      apply_motion_limits(cmax);
      const xyz_pos_t lmin = dmin.asLogical(), lmax = dmax.asLogical(),
                      wmin = cmin.asLogical(), wmax = cmax.asLogical();
      SERIAL_ECHOLNPAIR(
        "area:{"
          "full:{"
            "min:{x:", lmin.x, ",y:", lmin.y, ",z:", lmin.z, "},"
            "max:{x:", lmax.x, ",y:", lmax.y, ",z:", lmax.z, "}"
          "},"
          "work:{"
            "min:{x:", wmin.x, ",y:", wmin.y, ",z:", wmin.z, "},"
            "max:{x:", wmax.x, ",y:", wmax.y, ",z:", wmax.z, "}",
          "}"
        "}"
      );
    #endif

  #endif // EXTENDED_CAPABILITIES_REPORT
}
