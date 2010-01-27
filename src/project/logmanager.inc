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

// ---------------
//     Public
// ---------------

/**
 * Log format types.
 */
enum LogTypes
{
    LogType_Normal,         // Normal log message.  Printed in SourceMod logs.
    LogType_Error,          // Error message.  Printed in SourceMod error logs.
    LogType_Fatal_Module,   // Error message.  Disables the module that prints it.
    LogType_Fatal_Plugin    // Error message.  Kills the entire plugin.
}

// ---------------
//     Private
// ---------------

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
LogMgr_OnPluginStart()
{
    // Allocate 1 index for the data we want to store for each module.
    ModuleMgr_Allocate(1, g_iLMAllocatedIndexes);
    
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
LogMgr_OnModuleRegister(Handle:adtModule)
{
    // Push the 'false' into our allocated space to signify if this module is in the module filter or not.
    PushArrayCell(adtModule, false);
}

/**
 * Command project_module_info is being called.
 * Print the module data allocated by the log manager.
 * Note: |stock| tag will stop this function from being compiled if the project_module_info command
 * isn't working due to required base files not being included.
 * 
 * @param client    The client index the text is being printed to.
 * @param moduleID  The module to print info for.
 * @param msgtype   The type of message to tell Translations Manager to print.
 */
stock LogMgr_OnCommandModuleInfo(client, moduleID, MsgTypes:msgtype)
{
    // Translate a bool into a text phrase.
    decl String:strWhiteList[8];
    new bool:bWhiteList = LogMgr_IsModuleInFilter(moduleID);
    TransMgr_TranslateBool(client, bWhiteList, BoolPhrase_YesNo, strWhiteList, sizeof(strWhiteList));
    
    // Print the module event info.
    TransMgr_PrintText(client, MsgFormat_None, msgtype, _, false, "LogMgr cmd moduleinfo on whitelist", strWhiteList);
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
stock LogMgr_Print(moduleID, LogTypes:logtype, const String:description[], const String:text[], any:...)
{
    // If the module is disabled, then don't print the log.
    if (ModuleMgr_IsDisabled(moduleID))
        return;
    
    // If the log manager is disabled, and this is a normal log, then don't print the log.
    if (!LogMgr_CvarLog() && logtype == LogType_Normal)
        return;
    
    // Check if module filtering is enabled.
    if (LogMgr_CvarModuleFilter())
    {
        // Check if the module is in the filter.
        if (!LogMgr_IsModuleInFilter(moduleID))
            return;
    }
    
    // Format extra parameters into the log buffer.
    decl String:logbuffer[LOG_MAX_LENGTH_FILE];
    VFormat(logbuffer, sizeof(logbuffer), text, 5);
    
    // Get the module's full name.
    decl String:fullname[32]; // 32 because that's the max length of the module's fullname.
    ModuleMgr_ReadString(moduleID, ModuleData_FullName, fullname, sizeof(fullname));
    
    // Format the text string.
    Format(logbuffer, sizeof(logbuffer), LM_FORMAT, fullname, description, logbuffer);
    
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
            ModuleMgr_Disable(moduleID);
        }
        // Log an error message and kill the plugin.
        case LogType_Fatal_Plugin:
        {
            SetFailState(logbuffer);
        }
    }
    
    // Print log to all in-game admins if the cvar is enabled.
    if (LogMgr_CvarPrintAdmins())
    {
        // Print text to admins.
        TransMgr_PrintTextAll(false, true, MsgFormat_None, MsgType_Chat, _, true, logbuffer);
    }
    
    // Print log to all in-game clients if the cvar is enabled.
    if (LogMgr_CvarPrintPublic())
    {
        // Print text to public chat.
        TransMgr_PrintTextAll(false, false, MsgFormat_None, MsgType_Chat, _, true, logbuffer);
    }
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
stock bool:LogMgr_IsModuleInFilter(moduleID)
{
    return bool:GetArrayCell(ModuleMgr_GetModuleArray(moduleID), LOG_DATA_FILTER);
}

/**
 * Change if a module is enabled in the module filter.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param moduleID      The module ID of the module to change.
 * @param enable        True to enable the module in the filter, false to disable.
 */
stock LogMgr_ModuleFilterSet(moduleID, bool:enable)
{
    SetArrayCell(ModuleMgr_GetModuleArray(moduleID), LOG_DATA_FILTER, _:enable);
}

/**
 * Adds a module to the module filter.
 * This is meant for other base project files to use, modules shouldn't need this.
 *
 * @param moduleID  The module ID of the module being add to the filter.
 * 
 * @return          True if added, false if it's already in the filter.
 */
stock bool:LogMgr_ModuleFilterAdd(moduleID)
{
    // Check if the module isn't already is listed.
    if (!LogMgr_IsModuleInFilter(moduleID))
    {
        // Add module to filter.
        LogMgr_ModuleFilterSet(moduleID, true);
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
stock bool:LogMgr_ModuleFilterRemove(moduleID)
{
    // Check if the module isn't already is listed.
    if (LogMgr_IsModuleInFilter(moduleID))
    {
        // Add module to filter.
        LogMgr_ModuleFilterSet(moduleID, false);
        return true;
    }
    
    return false;
}

/**
 * Command callback: project_log_add_module
 * Add one or modules to module filter.
 * TODO: Translations
 * 
 * @param client    The client index.  Or SERVER_INDEX if coming from the server.
 * @param argc      The number of arguments that the client sent with the command.
 */
public Action:Command_LogModuleAdd(client, argc)
{
    // Check if privileged.
    /**
     * Check if the client has access
     */
    
    // Check if no arguments.
    if (argc < 1)
    {
        // Display usage info.
        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, false, "LogMgr cmd moduleadd usage");
        return Plugin_Handled;
    }
    
    decl String:shortname[16];
    new moduleID;
    
    // Loop through each argument.
    for (new arg = 1; arg <= argc; arg++)
    {
        // Get argument string.
        GetCmdArg(arg, shortname, sizeof(shortname));
        moduleID = ModuleMgr_FindByString(ModuleData_ShortName, shortname);
        
        // If -1 was returned, then the module is invalid.
        if (moduleID == -1)
        {
            // Reply with an error and skip to next argument.
            TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, false, "ModuleMgr invalid module", shortname);
            continue;
        }
        
        LogMgr_ModuleFilterAdd(moduleID);
        TransMgr_PrintText(client, MsgFormat_Both, MsgType_Reply, moduleID, false, "LogMgr cmd moduleadd");
    }
    
    // Say that we handled the command so CS:S doesn't see it and print "Unknown command"
    return Plugin_Handled;
}

/**
 * Command callback: project_log_remove_module
 * Remove one or modules from module filter.
 * TODO: Translations
 * 
 * @param client    The client index.  Or SERVER_INDEX if coming from the server.
 * @param argc      The number of arguments that the client sent with the command.
 */
public Action:Command_LogModuleRemove(client, argc)
{
    decl String:shortname[32];
    new moduleID;
    
    // Check if privileged.
    /**
     * Check if the client has access
     */
    
    // Check if no arguments.
    if (argc < 1)
    {
        // Display usage info.
        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, false, "LogMgr cmd moduleremove usage");
        return Plugin_Handled;
    }
    
    // Loop through each argument.
    for (new arg = 1; arg <= argc; arg++)
    {
        // Get argument string.
        GetCmdArg(arg, shortname, sizeof(shortname));
        moduleID = ModuleMgr_FindByString(ModuleData_ShortName, shortname);
        
        // If -1 was returned, then the module is invalid.
        if (moduleID == -1)
        {
            // Reply with an error and skip to next argument.
            TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, false, "ModuleMgr invalid module", shortname);
            continue;
        }
        
        LogMgr_ModuleFilterRemove(moduleID);
        TransMgr_PrintText(client, MsgFormat_Both, MsgType_Reply, moduleID, false, "LogMgr cmd moduleremove");
    }
    
    // Say that we handled the command so CS:S doesn't see it and print "Unknown command"
    return Plugin_Handled;
}

/**
 * These are stock functions to return the value of any of the log manager's cvars.
 * This allows other base files/modules to read log cvars.
 */

stock bool:LogMgr_CvarLog()
{
    return GetConVarBool(cvarLog);
}

stock bool:LogMgr_CvarModuleFilter()
{
    return GetConVarBool(cvarLogModuleFilter);
}

stock bool:LogMgr_CvarPrintAdmins()
{
    return GetConVarBool(cvarLogPrintAdmins);
}

stock bool:LogMgr_CvarPrintPublic()
{
    return GetConVarBool(cvarLogPrintPublic);
}