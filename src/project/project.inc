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
//     General
// ---------------

/**
 * The client index of the server.
 */
#define SERVER_INDEX 0


// --------------------------------
//        Module Reference
// --------------------------------
// Note: Treat this section like a stripper.  Look, don't touch.

/**
 * The amount of registered modules.
 * All integers less than this (and greater than or equal to zero) are valid module identifiers.
 */
#define MODULE_COUNT GetArraySize(ModuleMgr_GetList())

/**
 * @section Takes either a module ID or module identifier and converts it to its respective opposite.
 */
#define MODULE_TO_ID(%1) _:%1 + 1
#define ID_TO_MODULE(%1) Module:(%1 - 1)
/**
 * @endsection
 */

/**
 * Module identifier tag.
 */
enum Module
{
    INVALID_MODULE = -1     /** Used as an invalid module. */
}

/**
 * @section Module data flags.
 */
#define MODULE_DATA_DISABLED    (1 << 0)
#define MODULE_DATA_HIDDEN      (1 << 1)
#define MODULE_DATA_FULLNAME    (1 << 2)
#define MODULE_DATA_SHORTNAME   (1 << 3)
#define MODULE_DATA_DESCRIPTION (1 << 4)
/**
 * @endsection
 */

/**
 * All the data modules need to register.
 */
enum ModuleData
{
    bool:ModuleData_Disabled = 0,       /** True if the module is disabled. */
    bool:ModuleData_Hidden,             /** True to hide the module from console. (base cmd can't touch it) */
    String:ModuleData_FullName[32],     /** Used for displaying modules. */
    String:ModuleData_ShortName[16],    /** Used for client interaction with modules. */
    String:ModuleData_Description[256], /** A description of the module */
    // gunna need more than this..
}

/**
 * The number of cells used to store all the data in enum ModuleData.
 */
#define MODULE_DATA_CELL_COUNT 1 + 1 + 32 + 16 + 256

/**
 * The longest string that can be stored in the module's data. (Size of ModuleData_FullName)
 */
#define MODULE_DATA_LONGEST_STRING 256

// --------------------------------
//     Event Forwarding Stuff
// --------------------------------
// Note: Since event HOOKING is done here, you can touch this one ;) (you can change/add/remove stuff here)
// Note: Ignore this section if eventmanager.inc won't be compiled into the plugin.
/**
 * This is a collection of ALL game and custom events.
 * Add any custom events here to the bottom of the list, and update define EVENT_DATA_CELL_COUNT. (below)
 * Also don't forget to add the display name of your event to g_ProjectEventDisplay. (below)
 */
enum ProjectEvents
{
    Event_OnPluginStart,
    Event_OnAllPluginsLoaded,
    Event_OnPluginEnd,
    Event_OnMapStart,
    Event_OnMapEnd,
    Event_OnAutoConfigsBuffered,
    Event_OnConfigsExecuted,
    Event_OnClientPutInServer,
    Event_OnClientDisconnect,
    Event_RoundStart,
    Event_RoundFreezeEnd,
    Event_RoundEnd,
    Event_PlayerTeam,
    Event_PlayerSpawn,
    Event_PlayerHurt,
    Event_PlayerDeath,
    Event_PlayerJump,
    Event_WeaponFire
    //Event_WeaponEntityFire
}

/**
 * The number of cells used to store all the event enable data in enum ProjectEvents. (above)
 * To calculate this number, simply add up the number of items in enum ProjectEvents.
 * Keep this updated as ProjectEvents is modified. 
 */
#define EVENT_DATA_CELL_COUNT 18

/**
 * Datatypes that can be passed with each event to modules.
 * Don't touch this!
 */
enum EventDataTypes
{
    DataType_Cell,
    DataType_Float,
    DataType_Array,
    DataType_String
}

/**
 * @section Common data type info.
 */

// When forwarding custom events with no data, use this.
stock any:g_CommonEventData1[][] = {{0}};

// When forwarding custom events whose data thats being passed matches these, you can use these.
stock EventDataTypes:g_CommonDataType1[] = {};
stock EventDataTypes:g_CommonDataType2[] = {DataType_Cell};
/**
 * @endsection
 */

/**
 * String array of display names for project events. (above)
 * Keep this updated as ProjectEvents is modified.
 */
stock String:g_ProjectEventDisplay[ProjectEvents][] = {
    "OnPluginStart",
    "OnAllPluginsLoaded",
    "OnPluginEnd",
    "OnMapStart",
    "OnMapEnd",
    "OnAutoConfigsBuffered",
    "OnConfigsExecuted",
    "OnClientPutInServer",
    "OnClientDisconnect",
    "RoundStart",
    "RoundFreezeEnd",
    "RoundEnd",
    "PlayerTeam",
    "PlayerSpawn",
    "PlayerHurt",
    "PlayerDeath",
    "PlayerJump",
    "WeaponFire"
    //"WeaponEntityFire"
};

// ---------------
//    Editable
// ---------------

// General

/**
 * If this is defined, then it should be in a mercurial repository.
 */
#define PROJECT_MERCURIAL
#define PROJECT_BRANCH "smprojectbase"  // Name of the Mercurial branch this is being developed in.

/**
 * @section Basic project information.  This will also be displayed in 'sm plugins *' commands.
 */
#define PROJECT_FULLNAME    "Project"   // Used when printing the plugin name anywhere.
#define PROJECT_SHORTNAME   "project"   // Used for filenames, etc.
#define PROJECT_AUTHOR      "Jacque Strap"
#define PROJECT_DESCRIPTION "A base for large plugins to load modules and use its API."
#define PROJECT_VERSION     "1.0.0"     // http://wiki.eclipse.org/Version_Numbering
#define PROJECT_URL ""
/**
 * @endsection
 */

/**
 * @section Additional information displayed in the version info.
 */
#define PROJECT_COPYRIGHT "Copyright (C) 2010  PROJECT_AUTHOR"
#define PROJECT_LICENSE "GNU GPL, Version 3"

/**
 * Your plugin's base command for module management.
 * This command will be very similar to the "sm" command except geared toward module management instead of plugin management.
 * Also handles printing plugin information like credits and version.
 * Comment out to remove command from the plugin.
 */
#define PROJECT_BASE_CMD PROJECT_SHORTNAME

// Translations Manager

/**
 * @section The formatting string for each option in enum MsgFormatting.
 * Note: These are only used if translationsmanager.inc is included.
 * 
 * See TransMgr_PrintText in translationsmanager.inc.
 * 'translated': The translated text being printed.
 * 'modulefullname': The module's full name.
 * 
 * Color chars vary from game to game.
 */
#define TM_MSGFORMAT_PLUGIN_COLOR      "\x04[%s] \x01%s", PROJECT_FULLNAME, translated
#define TM_MSGFORMAT_PLUGIN_NOCOLOR    "[%s] %s", PROJECT_FULLNAME, translated
#define TM_MSGFORMAT_MODULE_COLOR      "\x03[%s] \x01%s", modulefullname, translated
#define TM_MSGFORMAT_MODULE_NOCOLOR    "[%s] %s", modulefullname, translated
#define TM_MSGFORMAT_BOTH_COLOR        "\x04[%s::\x03%s] \x01%s", PROJECT_FULLNAME, modulefullname, translated
#define TM_MSGFORMAT_BOTH_NOCOLOR      "[%s::%s] %s", PROJECT_FULLNAME, modulefullname, translated
/**
 * @endsection
 */

/**
 * Load any extra translation files here.
 *  
 * Automatically loaded translations files for the project are:
 * translations/<projectshortname>/<projectshortname>.phrases.txt
 * translations/<projectshortname>/projectbase.phrases.txt
 *  
 * Translation files for modules should be loaded via TransMgr_Register.
 * Note: This is only used if translationsmanager.inc is included.
 */
stock TransMgr_LoadExtraTranslations()
{
    // Standard phrases provided by SM.
    LoadTranslations("common.phrases");
}

// Log Manager

/**
 * The formatting string for all logs.
 * Note: This is only used if logmanager.inc is included.
 *  
 * See LogMgr_Print in logmanager.inc.
 * 'modulefullname': The module's full name.
 * 'description': The log description.
 * 'logbuffer': The log text.
 */
#define LM_FORMAT "[%s] [%s] %s", modulefullname, description, logbuffer


// Access Manager

/**
 * The formatting string for module access groups/cvars.
 * Note: This is only used if accessmanager.inc is included.
 *  
 * See AccessMgr_ModuleToGroupName and AccessMgr_ModuleToCvarName in accessmanager.inc.
 * 'moduleshortname':  The short name of the module the group/cvar name is being formatted for.
 */
#define AM_FORMAT_GROUP "%s_access_%s", PROJECT_SHORTNAME, moduleshortname
#define AM_FORMAT_CVAR  "%s_access_%s", PROJECT_SHORTNAME, moduleshortname
