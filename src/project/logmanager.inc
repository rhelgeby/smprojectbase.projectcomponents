/*
 * ============================================================================
 *
 *  Project
 *
 *  File:          logmanager.inc
 *  Type:          Base
 *  Description:   Manages project logging.
 *
 *  Copyright (C) 2009-2010  Greyscale & Richard Helgeby
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
 * Provides the plugin a way to know if the log manager is included in the project.
 */
#define LOG_MANAGER

/**
 * The max number of cells needed for the log manager's allocated index.
 */
#define LM_DATA_MAX_CELLS 1
// Log manager only needs 1 cell for a boolean value.

/**
 * Defines the block of data in the module data arrays that contains module filter data.
 */
#define LOG_DATA_FILTER g_iLMAllocatedIndexes[0]

/**
 * @section Log message max lengths.
 */
#define LOG_MAX_LENGTH_FILE 2048
#define LOG_MAX_LENGTH_CHAT 192
/**
 * @endsection
 */

/**
 * @section Log flags.
 */
#define LOG_CORE_EVENTS             (1 << 0)    /** Log events from the plugin core like config validation and other messages. */
#define LOG_GAME_EVENTS             (1 << 1)    /** Log admin commands, console commands, and game related events from modules like suicide attempts and weapon restrictions. */
#define LOG_PLAYER_COMMANDS         (1 << 2)    /** Log events that are triggered by players, like chat triggers, teleporting and class change. */
#define LOG_DEBUG                   (1 << 3)    /** Log debug messages, if any. Usually only developers enable this log flag. */
#define LOG_DEBUG_DETAIL            (1 << 4)    /** Log additional debug messages with more detail. May cause spam depending on filter settings. Usually only developers enable this log flag. */
/**
 * @endsection
 */

/**
 * Log format types.
 */
enum LogTypes
{
    LogType_Normal = 0,     // Normal log message.  Printed in SourceMod logs.
    LogType_Error,          // Error message.  Printed in SourceMod error logs.
    LogType_Fatal_Module,   // Error message.  Disables the module that prints it.
    LogType_Fatal_Plugin    // Error message.  Kills the entire plugin.
}

/**
 * Array to store the index of the allocated space in the module data arrays for the log manager.
 */
new g_iLMAllocatedIndexes[1];

/**
 * @section Log cvars.
 */
new Handle:cvarLog;
new Handle:cvarLogModuleFilter;
new Handle:cvarLogPrintAdmins;
new Handle:cvarLogPrintPublic;
/**
 * @endsection
 */

// **********************************************
//                 Forwards
// **********************************************

/**
 * Plugin has loaded.
 */
LogManager_OnPluginStart()
{
    // Allocate 1 index for the data we want to store for each module.
    ModuleManager_Allocate(1, g_iLMAllocatedIndexes);
    
    // Create the log filter.
    if (g_hLogModuleFilter == INVALID_HANDLE)
        g_hLogModuleFilter = CreateArray(16); // 16 because thats the max length of the module's shortname.
    
    // Create log cvars.
    cvarLog =               CreateConVar("project_log", "1", "Enable logging in the plugin.  Error messages will always be logged.");
    cvarLogModuleFilter =   CreateConVar("project_log_module_filter", "0", "Only modules added to the filter (project_log_add_module) will be logged.");
    cvarLogPrintAdmins =    CreateConVar("project_log_print_admins", "0", "Logs will be printed to admin chat in addition to log files.");
    cvarLogPrintPublic =    CreateConVar("project_log_print_public", "0", "Logs will be printed to public chat in addition to log files.");
    
    // Create log commands.
    RegConsoleCmd("project_log_module_add", Command_LogModuleAdd, "Add one or more modules to the module filter. Usage: zr_log_add_module <module> [module] ...");
    RegConsoleCmd("project_log_module_remove", Command_LogModuleRemove, "Remove one or more modules from the module filter. Usage: zr_log_remove_module <module> [module] ...");
}

/**
 * A module was just registered.  This is being called before the module has been loaded.
 * 
 * @param adtModule The adt array of the module being registered.
 */
LogManager_OnModuleRegister(Handle:adtModule)
{
    // Push the 'false' into our allocated space to signify if this module is in the module filter or not.
    PushArrayCell(adtModule, false);
}

// **********************************************
//                Public API
// **********************************************

/**
 * Print a formatted message to logs depending on log settings.
 * 
 * @param moduleID      The module ID of the module sending the log.
 * @param logtype       The type of the log being processed.
 * @param description   Short description of the log, like a function name.
 * @param text          The log message.
 * @param ...           Formatting parameters.
 */
stock LogManager_Print(moduleID, LogTypes:logtype, const String:description[], const String:text[], any:...)
{
    // If the module is disabled, then don't print the log.
    if (ModuleManager_IsDisabled(moduleID))
        return;
    
    // If the log manager is disabled, and this is a normal log, then don't print the log.
    if (!LogManager_CvarLog() && logtype == LogType_Normal)
        return;
    
    // Check if module filtering is enabled.
    if (LogManager_CvarLogModuleFilter())
    {
        // Check if the specified module is enabled.
        if (!LogCheckModuleFilter(moduleID))
            return;
    }
    
    // Format extra parameters into the log buffer.
    decl String:logbuffer[LOG_MAX_LENGTH_FILE];
    VFormat(logbuffer, sizeof(logbuffer), text, 5);
    
    // Get the module's full name.
    decl String:fullname[32]; // 32 because that's the max length of the module's fullname,
    ModuleManager_ReadString(moduleID, ModuleData_FullName, fullname, fullname);
    
    // Format the text string.
    Format(logbuffer, sizeof(logbuffer), "[%s] [%s] %s", fullname, description, logbuffer);
    
    // Format other parameters onto the log text.
    switch (logtype)
    {
        // Normal log message.
        case LogType_Normal:
        {
            LogMessage(logbuffer);
        }
        // Log an error message.
        case LogType_Error:
        {
            LogError(logbuffer);
        }
        // Log an error message and disable the module.
        case LogType_Fatal_Module:
        {
            LogError(logbuffer);
            ModuleManager_Disable(moduleID);
        }
        // Log an error message and kill the plugin.
        case LogType_Fatal_Plugin:
        {
            SetFailState(logbuffer);
        }
    }
    
    // Note: The phrase "Literal text" is a blank phrase to pass any string we want into it.
    
    // Check if printing log events to admins is enabled.
    //if (GetConVarBool(g_hCvarsList[CVAR_LOG_PRINT_ADMINS]))
    //{
        // Print text to admins.
        //TranslationPrintToChatAll(false, true, "Literal text", logbuffer);
    //}
    
    // Check if printing log events to public chat is enabled.
    //if (GetConVarBool(g_hCvarsList[CVAR_LOG_PRINT_CHAT]))
    //{
        // Print text to public chat.
        //TranslationPrintToChatAll(false, false, "Literal text", logbuffer);
    //}
}

// **********************************************
//   Private API (For base project files only)
// **********************************************

/**
 * Check if the specified module is enabled in the module filter.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param moduleID      The module ID of the module to check.
 * 
 * @return              True if enabled, false otherwise. 
 */
stock bool:LogManager_IsModuleInFilter(moduleID)
{
    return bool:GetArrayCell(ModuleManager_GetModuleArray(moduleID), LOG_DATA_FILTER);
}

/**
 * Change if a module is enabled in the module filter.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param moduleID      The module ID of the module to change.
 * @param enable        True to enable the module in the filter, false to disable.
 */
stock LogManager_ModuleFilterSet(moduleID, bool:enable)
{
    SetArrayCell(ModuleManager_GetModuleArray(moduleID), LOG_DATA_FILTER, _:enable);
}

/**
 * Adds a module to the module filter.
 * This is meant for other base project files to use, modules shouldn't need this.
 *
 * @param moduleID  The module ID of the module being add to the filter.
 * 
 * @return          True if added, false if it's already in the filter.
 */
stock bool:LogManager_ModuleFilterAdd(moduleID)
{
    // Check if the module isn't already is listed.
    if (!LogManager_IsModuleInFilter(moduleID))
    {
        // Add module to filter.
        LogManager_ModuleFilterSet(moduleID, true);
        return true;
    }
    
    return false;
}

/**
 * Removes a module from the module filter.
 * This is meant for other base project files to use, modules shouldn't need this.
 *
 * @param moduleID  The module ID of the module being removed from the filter.
 * 
 * @return          True if removed, false if it's not in the filter.
 */
stock bool:LogManager_ModuleFilterRemove(moduleID)
{
    // Check if the module isn't already is listed.
    if (LogManager_IsModuleInFilter(moduleID))
    {
        // Add module to filter.
        LogManager_ModuleFilterSet(moduleID, false);
        return true;
    }
    
    return false;
}

/**
 * Handles the zr_log_list command. Displays flags and module filter cache.
 *
 * @param client    The client that executed the command.
 * @param argc      Number of arguments passed.
 */
/*public Action:Command_LogList(client, argc)
{
    decl String:buffer[2048];
    decl String:linebuffer[96];
    decl String:modulename[64];
    decl String:modulenameshort[64];
    
    new modulecount;
    
    // Strings to store translated phrases. Because formatting width settings
    // doesn't work with "%t", but "%s".
    decl String:phrasegenericflag[32];
    decl String:phrasevalue[32];
    decl String:phrasemodule[32];
    decl String:phraseshortname[32];
    
    // Quick initialize string buffer.
    buffer[0] = 0;
    
    // Set language.
    SetGlobalTransTarget(client);
    
    // Module filtering status:
    Format(linebuffer, sizeof(linebuffer), "%t %t\n\n", "Log Module Filtering", LogManager_LogModuleFilter() ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), "%-23s %-19s %t\n", phrasemodule, phraseshortname, "Log Status");
    StrCat(buffer, sizeof(buffer), linebuffer);
    StrCat(buffer, sizeof(buffer), "--------------------------------------------------------------------------------");
    
    ReplyToCommand(client, buffer);
    buffer[0] = 0;
    
    // Module status:
    modulecount = sizeof(LogModuleFilterCache);
    for (new module = 1; module < modulecount; module++)
    {
        LogGetModuleNameString(modulename, sizeof(modulename), LogModules:module);
        LogGetModuleNameString(modulenameshort, sizeof(modulenameshort), LogModules:module, true);
        Format(linebuffer, sizeof(linebuffer), "%-23s %-19s %t", modulename, modulenameshort, LogModuleFilterCache[LogModules:module] ? "On" : "Off");
        ReplyToCommand(client, linebuffer);
    }
    
    return Plugin_Handled;
}*/

/**
 * Command callback: project_log_add_module
 * Add one or modules to module filter.
 * TODO: Translations
 * 
 * @param client    The client index.
 * @param argc      The number of arguments that the client sent with the command.
 */
public Action:Command_LogAddModule(client, argc)
{
    decl String:shortname[32];
    new moduleID;
    
    // Check if privileged.
    /**
     * Check if the client has access
     */
    
    new LogModules:logmodule;
    
    // Check if no arguments.
    if (argc < 1)
    {
        decl String:buffer[256];
        buffer[0] = 0;
        
        // Display syntax info.
        //StrCat(buffer, sizeof(buffer), "Add one or more modules to the module filter. Usage: zr_log_add_module <module> [module] ...\n");
        //StrCat(buffer, sizeof(buffer), "See zr_log_list to list available module names (short names).");
        //ReplyToCommand(client, buffer);
        
        return;
    }
    
    // Loop through each argument.
    for (new arg = 1; arg <= argc; arg++)
    {
        // Get argument string.
        GetCmdArg(arg, shortname, sizeof(shortname));
        moduleID = ModuleManager_FindByString(ModuleData_ShortName, shortname);
        
        // If -1 was returned, then the module is invalid.
        if (moduleID == -1)
        {
            //ReplyToCommand(client, "Invalid module name: \"%s\"", argument);
            
            // Skip to next argument.
            continue;
        }
        
        LogManager_ModuleFilterAdd(moduleID);
        //ReplyToCommand(client, "Added \"%s\" to module filter.", argument);
    }
    
    // Say that we handled the command so CS:S doesn't see it and print "Unknown command"
    return Plugin_Handled;
}

/**
 * Command callback: project_log_remove_module
 * Remove one or modules from module filter.
 * TODO: Translations
 * 
 * @param client    The client index.
 * @param argc      The number of arguments that the client sent with the command.
 */
public Action:Command_LogRemoveModule(client, argc)
{
    decl String:shortname[32];
    new moduleID;
    
    // Check if privileged.
    /**
     * Check if the client has access
     */
    
    new LogModules:logmodule;
    
    // Check if no arguments.
    if (argc < 1)
    {
        decl String:buffer[256];
        buffer[0] = 0;
        
        // Display syntax info.
        //StrCat(buffer, sizeof(buffer), "Add one or more modules to the module filter. Usage: zr_log_add_module <module> [module] ...\n");
        //StrCat(buffer, sizeof(buffer), "See zr_log_list to list available module names (short names).");
        //ReplyToCommand(client, buffer);
        
        return;
    }
    
    // Loop through each argument.
    for (new arg = 1; arg <= argc; arg++)
    {
        // Get argument string.
        GetCmdArg(arg, shortname, sizeof(shortname));
        moduleID = ModuleManager_FindByString(ModuleData_ShortName, shortname);
        
        // If -1 was returned, then the module is invalid.
        if (moduleID == -1)
        {
            //ReplyToCommand(client, "Invalid module name: \"%s\"", argument);
            
            // Skip to next argument.
            continue;
        }
        
        LogManager_ModuleFilterRemove(moduleID);
        //ReplyToCommand(client, "Added \"%s\" to module filter.", argument);
    }
    
    // Say that we handled the command so CS:S doesn't see it and print "Unknown command"
    return Plugin_Handled;
}

/**
 * These are stock functions to return the value of any of the log manager's cvars.
 * This allows other base files/modules to read log cvars.
 */

stock bool:LogManager_CvarLog()
{
    return GetConVarBool(cvarLog);
}

stock bool:LogManager_CvarModuleFilter()
{
    return GetConVarBool(cvarLogModuleFilter);
}

stock bool:LogManager_CvarPrintAdmins()
{
    return GetConVarBool(cvarLogPrintAdmins);
}

stock bool:LogManager_CvarPrintPublic()
{
    return GetConVarBool(cvarLogPrintPublic);
}