/*
 * ============================================================================
 *
 *  Project
 *
 *  File:          project.inc
 *  Type:          Base
 *  Description:   Contains defines, enums, etc available to anywhere in the project.
 *
 *  Copyright (C) 2009-2010  Greyscale
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

// ---------------
//    Reference
// ---------------

/**
 * The amount of registered modules.
 * All integers less than this (and greater than or equal to zero) are valid module IDs.
 */
#define MODULE_COUNT GetArraySize(ModuleMgr_GetList())

/**
 * All the data modules need to register.
 */
enum ModuleData
{
    bool:ModuleData_Disabled = 0,       /** True if the module is disabled. */
    String:ModuleData_FullName[32],     /** Probably going to be used when displaying modules. */
    String:ModuleData_ShortName[16],    /** Probably going to be used for client interaction. */
    // gunna need more than this..
}

// ---------------
//     Private
// ---------------

/**
 * The number of cells used to store all the data in enum ModuleData.
 */
#define MODULE_DATA_CELL_COUNT 49

/**
 * The longest string that can be stored in the module's data. (Cell size of ModuleData_FullName)
 */
#define MODULE_DATA_LONGEST_STRING 32

// ---------------
//    Editable
// ---------------

/**
 * @section Basic project name information.
 */
#define PROJECT_FULLNAME    "Project"   // Used when printing the plugin name anywhere.
#define PROJECT_SHORTNAME   "project"   // Used for filenames, etc.
/**
 * @endsection
 */

/**
 * @section The formatting string for each option in enum MsgFormatting.
 * Note: These are only used if translationsmanager.inc is included. 
 */
#define MSGFORMAT_PLUGIN_COLOR      "\x04[%s] \x01%s"          // 1st %s = Plugin name - 2nd %s = Text
#define MSGFORMAT_PLUGIN_NOCOLOR    "[%s] %s"                  // 1st %s = Plugin name - 2nd %s = Text
#define MSGFORMAT_MODULE_COLOR      "\x03[%s] \x01%s"          // 1st %s = Module name - 2nd %s = Text
#define MSGFORMAT_MODULE_NOCOLOR    "[%s] %s"                  // 1st %s = Module name - 2nd %s = Text
#define MSGFORMAT_BOTH_COLOR        "\x04[%s::\x03%s] \x01%s"    // 1st %s = Plugin name - 2nd %s = Module name - 3rd %s = Text
#define MSGFORMAT_BOTH_NOCOLOR      "[%s::%s] %s"                // 1st %s = Plugin name - 2nd %s = Module name - 3rd %s = Text
/**
 * @endsection
 */
