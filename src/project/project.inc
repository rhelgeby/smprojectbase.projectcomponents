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
 * The client index of the server.
 */
#define SERVER_INDEX 0

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
    String:ModuleData_FullName[32],     /** Used for displaying modules. */
    String:ModuleData_ShortName[16],    /** Used for client interaction with modules. */
    String:ModuleData_Description[256], /** A description of the module */
    // gunna need more than this..
}

// ---------------
//     Private
// ---------------

/**
 * The number of cells used to store all the data in enum ModuleData.
 */
#define MODULE_DATA_CELL_COUNT 1 + 32 + 16 + 256

/**
 * The longest string that can be stored in the module's data. (Cell size of ModuleData_FullName)
 */
#define MODULE_DATA_LONGEST_STRING 256

// ---------------
//    Editable
// ---------------

/**
 * @section Basic project information.  This will also be displayed in 'sm plugins *' commands.
 */
#define PROJECT_FULLNAME    "Project"   // Used when printing the plugin name anywhere.
#define PROJECT_SHORTNAME   "project"   // Used for filenames, etc.
#define PROJECT_AUTHOR      "Greyscale"
#define PROJECT_DESCRIPTION "A base for large plugins to load modules and use its API."
#define PROJECT_VERSION     "1.0.0"     // http://wiki.eclipse.org/Version_Numbering
#define PROJECT_URL ""
/**
 * @endsection
 */

/**
 * @section The formatting string for each option in enum MsgFormatting.
 * Note: These are only used if translationsmanager.inc is included.
 */
#define TM_MSGFORMAT_PLUGIN_COLOR      "\x04[%s] \x01%s"          // 1st %s = Plugin name - 2nd %s = Text
#define TM_MSGFORMAT_PLUGIN_NOCOLOR    "[%s] %s"                  // 1st %s = Plugin name - 2nd %s = Text
#define TM_MSGFORMAT_MODULE_COLOR      "\x03[%s] \x01%s"          // 1st %s = Module name - 2nd %s = Text
#define TM_MSGFORMAT_MODULE_NOCOLOR    "[%s] %s"                  // 1st %s = Module name - 2nd %s = Text
#define TM_MSGFORMAT_BOTH_COLOR        "\x04[%s::\x03%s] \x01%s"  // 1st %s = Plugin name - 2nd %s = Module name - 3rd %s = Text
#define TM_MSGFORMAT_BOTH_NOCOLOR      "[%s::%s] %s"              // 1st %s = Plugin name - 2nd %s = Module name - 3rd %s = Text
/**
 * @endsection
 */

/**
 * The formatting string for all logs.
 * Note: This is only used if logmanager.inc is included.
 */
#define LM_FORMAT "[%s] [%s] %s"    // 1st %s = Module name - 2nd %s = Short description - 3rd = Log text

/**
 * Load any extra translation files here.
 * The base project files translation file is <projectname>/<projectname>.phrases.txt.
 * Translation files for modules should be loaded via TransMgr_Register.
 * Note: This is only used if translationsmanager.inc is included.
 */
stock TransMgr_LoadExtraTranslations()
{
    // Standard phrases provided by SM.
    LoadTranslations("common.phrases");
}
