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

/**
 * Additional includes.
 * Include files like sdktools.inc here.
 */


/**
 * This is where it all begins.
 * Forward the OnPluginStart event here to all modules so they can register.
 */
stock ForwardOnPluginStart()
{
    // Forward the OnPluginStart event to all modules here.
    TestModule_OnPluginStart();
}

// **********************************************
//                   Reference
// **********************************************

/**
 * The client index of the server.
 */
#define SERVER_INDEX 0


// **********************************************
//                Module Manager
// **********************************************

/**
 * The amount of registered modules.
 * All integers less than this (and greater than or equal to zero) are valid module identifiers.
 */
#define MODULE_COUNT GetArraySize(ModuleMgr_GetList())

/**
 * Takes either a module ID or module identifier and converts it to its respective opposite.
 */
#define MODULE_TO_ID(%1) _:%1 + 1
#define ID_TO_MODULE(%1) Module:(%1 - 1)

/**
 * Module identifier tag.
 */
enum Module
{
    INVALID_MODULE = -1     /** Used as an invalid module. */
}

/**
 * Module data flags.
 */
#define MODULE_DATA_DISABLED    (1 << 0)
#define MODULE_DATA_HIDDEN      (1 << 1)
#define MODULE_DATA_FULLNAME    (1 << 2)
#define MODULE_DATA_SHORTNAME   (1 << 3)
#define MODULE_DATA_DESCRIPTION (1 << 4)

/**
 * The max string lengths of module data variables.
 */
#define CM_DATA_FULLNAME    32
#define CM_DATA_SHORTNAME   16
#define CM_DATA_DESCRIPTION 256

/**
 * All the data modules need to register.
 */
enum ModuleData
{
    bool:ModuleData_Disabled = 0,                       /** True if the module is disabled. */
    bool:ModuleData_Hidden,                             /** True to hide the module from console. (base cmd can't touch it) */
    String:ModuleData_FullName[CM_DATA_FULLNAME],       /** Used for displaying modules. */
    String:ModuleData_ShortName[CM_DATA_SHORTNAME],     /** Used for client interaction with modules. */
    String:ModuleData_Description[CM_DATA_DESCRIPTION], /** A description of the module */
}

/**
 * The number of cells used to store all the data in enum ModuleData.
 */
#define MODULE_DATA_CELL_COUNT 1 + 1 + CM_DATA_FULLNAME + CM_DATA_SHORTNAME + CM_DATA_DESCRIPTION

/**
 * The longest string that can be stored in the module's data.
 */
#define MODULE_DATA_LONGEST_STRING 256

// **********************************************
//                 Event Manager
// **********************************************
// Note: Ignore this section if eventmanager.inc isn't included in the project.

/**
 * This is a collection of ALL game and custom events.
 * Add any custom events here to the bottom of the list, and update define EVENT_DATA_CELL_COUNT. (below)
 * Also don't forget to add the display name of your event to g_ProjectEventDisplay. (below)
 */
enum ProjectEvents
{
    Event_OnAllPluginsLoaded,       /** All plugins have loaded. */
    Event_OnPluginEnd,              /** Plugin is ending. */
    Event_OnAllModulesLoaded,       /** All modules have been registered. */
    Event_OnModuleEnable,           /** A module has been enabled. */
    Event_OnMyModuleEnable,         /** The module that hooked this event callback has been enabled. */
    Event_OnModuleDisable,          /** A module has been disabled. */
    Event_OnMyModuleDisable,        /** The module that hooked this event callback has been disabled. */
    Event_OnMapStart,               /** The map has started. */
    Event_OnMapEnd,                 /** The map has ended. */
    Event_OnAutoConfigsBuffered,    /** This is called before OnConfigsExecuted but any time after OnMapStart. */
    Event_OnConfigsExecuted,        /** All convars are set, cvar-dependent code should use this. */
    Event_OnClientPutInServer,      /** Client has joined the server. */
    Event_OnClientDisconnect,       /** Client is disconnecting from the server. */
    Event_RoundStart,               /** Round has started. */
    Event_RoundFreezeEnd,           /** Pre-round has freezetime has finished. */
    Event_RoundEnd,                 /** Round has ended. */
    Event_PlayerTeam,               /** Client has joined a team. */
    Event_PlayerSpawn,              /** Client has spawned. */
    Event_PlayerHurt,               /** Client has been damaged. */
    Event_PlayerDeath,              /** Client has been killed. */
    Event_PlayerJump,               /** Client has jumped. */
    Event_WeaponFire                /** Client has fired a weapon. */
    //Event_WeaponEntityFire        /** Client has fired a weapon. (returns weapon entity index) */
}

/**
 * String array of display names for project events. (above)
 * Keep this updated as ProjectEvents is modified.
 */
stock String:g_ProjectEventDisplay[ProjectEvents][] = {
    "OnAllPluginsLoaded",
    "OnPluginEnd",
    "OnAllModulesLoaded",
    "OnModuleEnable",
    "OnMyModuleEnable",
    "OnModuleDisable",
    "OnMyModuleDisable",
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

/**
 * The number of cells used to store all the event enable data in enum ProjectEvents. (above)
 * To calculate this number, simply add up the number of items in enum ProjectEvents.
 * Keep this updated as ProjectEvents is modified. 
 */
#define EVENT_DATA_CELL_COUNT 22

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
 * Common data type info.
 */

// When forwarding custom events with no data, use this.
stock any:g_CommonEventData1[][] = {{0}};

// When forwarding custom events whose data thats being passed matches these, you can use these.
stock EventDataTypes:g_CommonDataType1[] = {};
stock EventDataTypes:g_CommonDataType2[] = {DataType_Cell};


// **********************************************
//                    General
// **********************************************

/**
 * Basic project information.  This will also be displayed in 'sm plugins *' commands.
 */

// Used when printing the plugin name anywhere.
#define PROJECT_FULLNAME    "Project"

// Shorter version of the full name, used in file paths, and other things.
#define PROJECT_SHORTNAME   "project"

// Author of the plugin.
#define PROJECT_AUTHOR      "Jacque Strap"

// Description of the plugin.
#define PROJECT_DESCRIPTION "A base for large plugins to load modules and use its API."

// http://wiki.eclipse.org/Version_Numbering
#define PROJECT_VERSION     "1.0.0"

// Any URL associated with the project.
#define PROJECT_URL ""

/**
 * Additional information displayed in the version info.
 */
#define PROJECT_COPYRIGHT "Copyright (C) 2010  PROJECT_AUTHOR"
#define PROJECT_LICENSE "GNU GPL, Version 3"

/**
 * Credits.
 * This is the exact string that's printed to the server/client.
 * Feel free to change it to anything, but the names mentioned here by default, should be credited somewhere.
 */
#define PROJECT_CREDITS " Plugin was developed by Greyscale.\n Development would not have been possible without:\n  AlliedModders, LLC.\n  Andrew \"Greyscale\" Borba, plugin base developer\n  Richard \"rhelgeby\" Helgeby, compilation environment designer and advisor\n http://www.sourcemod.net/"

/**
 * Base file message prefixes.
 * 
 * See Project_PrintToServer in wrappers.inc.
 * 'formatted': The text being printed.
 */
#define PROJECT_SERVER_PREFIX "[%s] %s", PROJECT_FULLNAME, formatted

/**
 * Your plugin's base command for module management.
 * This command will be very similar to the "sm" command except geared toward module management instead of plugin management.
 * Also handles printing plugin information like credits and version.
 * Comment out to remove command from the plugin.
 */
#define PROJECT_BASE_CMD PROJECT_SHORTNAME

/**
 * The prefixes for any base cvars/commands.
 * These are strictly for project base cvars/commands.
 * This will not affect cvars/commands created in your modules.
 */
#define PROJECT_CVAR_PREFIX PROJECT_SHORTNAME
#define PROJECT_CMD_PREFIX  PROJECT_SHORTNAME


// **********************************************
//            Translations Manager
// **********************************************

/**
 * The directory where the project's translation files will be in.
 * All translations files in the format of *.phrases.txt will be automatically loaded.
 * Example path: addons/sourcemod/translations/TM_BASE_DIR/projectbase.phrases.txt.
 * If the resulting path doesn't exist, errors will be logged.
 * 
 * Note: This only used if translationsmanager.inc is included.
 */
#define TM_BASE_DIR PROJECT_SHORTNAME

/**
 * The formatting string for each option in enum MsgFormatting.
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
 * Load any extra translation files here.
 *  
 * All translations files in the format of *.phrases.txt will be automatically loaded
 * from the "translations/TM_BASE_DIR/" dir.
 * 
 * Note: If the translations manager isn't included, you must load all your translations here.
 */
stock Project_LoadExtraTranslations()
{
    // Load translations we may need, if the translations manager isn't loaded.
    #if !defined TRANSLATIONS_MANAGER
        LoadTranslations("projectbase.phrases");
        
        #if defined PROJECT_BASE_CMD
            LoadTranslations("basecmd.phrases");
        #endif
    #endif
    
    // Standard phrases provided by SM.
    LoadTranslations("common.phrases");
}


// **********************************************
//                 Log Manager
// **********************************************

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
#define AM_FORMAT_GROUP "%s_access_%s", PROJECT_CVAR_PREFIX, moduleshortname
#define AM_FORMAT_CVAR  "%s_access_%s", PROJECT_CVAR_PREFIX, moduleshortname

// **********************************************
//                Version Info
// **********************************************

/**
 * If this is defined, then it should be in a mercurial repository.
 */
#define PROJECT_MERCURIAL

// Name of the Mercurial branch this is being developed in.
#define PROJECT_BRANCH "smprojectbase"

/**
 * Name of the public cvar for the plugin.
 * Comment this define out to not create a public cvar.
 */
#define PROJECT_PUBLIC_CVAR "project_version_public"
