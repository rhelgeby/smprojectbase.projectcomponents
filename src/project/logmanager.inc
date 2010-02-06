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
 * Defines the block of data in the module data arrays that contains module whitelist data.
 */
#define LOG_DATA_WHITELIST g_iLMAllocatedIndexes[0]

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
new Handle:cvarLogWhitelist;
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
    cvarLog =               Project_CreateConVar("log", "1", "Enable logging in the plugin.  Error messages will always be logged.");
    cvarLogWhitelist =      Project_CreateConVar("log_whitelist", "0", "Only modules added to the whitelist (project_log_whitelist_add) will be logged.");
    cvarLogPrintAdmins =    Project_CreateConVar("log_print_admins", "0", "Logs will be printed to admin chat in addition to log files.");
    cvarLogPrintPublic =    Project_CreateConVar("log_print_public", "0", "Logs will be printed to public chat in addition to log files.");
    
    // Create log commands.
    Project_RegServerCmd("log_whitelist_add", Command_LogWhitelistAdd, "Adds one or more modules to the whitelist.  Usage: project_log_whitelist_add <moduleshortname [module2] ...");
    Project_RegServerCmd("log_whitelist_remove", Command_LogWhitelistRemove, "Removes one or more modules from the whitelist.  Usage: project_log_whitelist_remove <moduleshortname [module2] ...");
}

/**
 * A module was just registered.  This is being called before the module has been assigned a module identifier.
 * 
 * @param adtModule The adt array of the module being registered.
 */
stock LogMgr_OnModuleRegister(Handle:adtModule)
{
    // Push the 'false' into our allocated space to signify if this module is on the whitelist or not.
    PushArrayCell(adtModule, false);
}

/**
 * Command project_module_info is being called.
 * Print the module data allocated by the log manager.
 * Note: |stock| tag will stop this function from being compiled if the project_module_info command
 * isn't working due to required base components not being included.
 * 
 * @param client    The client index the text is being printed to.
 * @param module    The module to print info for.
 * @param msgtype   The type of message to tell Translations Manager to print.
 */
stock LogMgr_OnPrintModuleInfo(client, Module:module, MsgTypes:msgtype)
{
    // Translate a bool into a text phrase.
    decl String:strWhiteList[8];
    new bool:bWhiteList = LogMgr_IsModuleOnWhitelist(module);
    TransMgr_TranslateBoolToPhrase(client, bWhiteList, BoolPhrase_YesNo, strWhiteList, sizeof(strWhiteList));
    
    // Print the module event info.
    TransMgr_PrintText(client, MsgFormat_None, msgtype, _, false, "LogMgr cmd base lvl2 modules info print", strWhiteList);
}

// **********************************************
//                Public API
// **********************************************

/**
 * Print a formatted message to logs depending on log settings.
 * 
 * @param module        The module sending the log.
 * @param logtype       The type of the log being processed.
 * @param description   Short description of the log, like a function name.
 * @param text          The log message.
 * @param ...           Formatting parameters.
 */
stock LogMgr_Print(Module:module, LogTypes:logtype, const String:description[], const String:text[], any:...)
{
    // If the module is disabled, then don't print the log.
    if (ModuleMgr_IsDisabled(module))
        return;
    
    // If the log manager is disabled, and this is a normal log, then don't print the log.
    if (!LogMgr_CvarLog() && logtype == LogType_Normal)
        return;
    
    // Check if whitelist is enabled.
    if (LogMgr_CvarWhitelist())
    {
        // Check if the module is on the whitelist.
        if (!LogMgr_IsModuleOnWhitelist(module))
            return;
    }
    
    // Format extra parameters into the log buffer.
    decl String:logbuffer[LOG_MAX_LENGTH_FILE];
    VFormat(logbuffer, sizeof(logbuffer), text, 5);
    
    // Get the module's full name.
    decl String:modulefullname[32]; // 32 because that's the max length of the module's fullname.
    ModuleMgr_ReadString(module, ModuleData_FullName, modulefullname, sizeof(modulefullname));
    
    // Format the text string.
    Format(logbuffer, sizeof(logbuffer), LM_FORMAT);
    
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
            ModuleMgr_Disable(module);
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
        #if defined TRANSLATIONS_MANAGER
            // Print text to admins.
            TransMgr_PrintTextAll(false, true, MsgFormat_None, MsgType_Chat, _, true, logbuffer);
        #else
            PrintToChatAll("Cvar project_log_print_admins requires the translations manager.");
        #endif
    }
    
    // Print log to all in-game clients if the cvar is enabled.
    if (LogMgr_CvarPrintPublic())
    {
        #if defined TRANSLATIONS_MANAGER
            // Print text to public chat.
            TransMgr_PrintTextAll(false, false, MsgFormat_None, MsgType_Chat, _, true, logbuffer);
        #else
            PrintToChatAll("Cvar project_log_print_public requires the translations manager.");
        #endif
    }
}

// **********************************************
//   Private API (For base project files only)
// **********************************************

/**
 * Check if the specified module is on the whitelist
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param module        The module to check.
 * 
 * @return              True if enabled, false otherwise. 
 */
stock bool:LogMgr_IsModuleOnWhitelist(Module:module)
{
    return bool:GetArrayCell(ModuleMgr_GetModuleArray(module), LOG_DATA_WHITELIST);
}

/**
 * Change if a module is enabled on the whitelist.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param module        The module to change.
 * @param enable        True to add the module to the whitelist, false to remove.
 */
stock LogMgr_WhitelistSet(Module:module, bool:enable)
{
    SetArrayCell(ModuleMgr_GetModuleArray(module), LOG_DATA_WHITELIST, _:enable);
}

/**
 * Adds a module to the whitelist.
 * This is meant for other base project files to use, modules shouldn't need this.
 *
 * @param module    The module being add to whitelist.
 * 
 * @return          True if added, false if it's already in the whitelist.
 */
stock bool:LogMgr_WhitelistAdd(Module:module)
{
    // Check if the module isn't already is listed.
    if (!LogMgr_IsModuleOnWhitelist(module))
    {
        // Add module to the whitelist.
        LogMgr_WhitelistSet(module, true);
        return true;
    }
    
    return false;
}

/**
 * Removes a module from the whitelist.
 * This is meant for other base project files to use, modules shouldn't need this.
 *
 * @param module    The module being removed from the whitelist.
 * 
 * @return          True if removed, false if it's not on the whitelist.
 */
stock bool:LogMgr_WhitelistRemove(Module:module)
{
    // Check if the module isn't already is listed.
    if (LogMgr_IsModuleOnWhitelist(module))
    {
        // Remove module from the whitelist.
        LogMgr_WhitelistSet(module, false);
        return true;
    }
    
    return false;
}

/**
 * Command callback: project_log_whitelist_add
 * Add one or modules to the whitelist.
 * 
 * @param argc      The number of arguments that the server sent with the command.
 */
public Action:Command_LogWhitelistAdd(argc)
{
    /**
     * Check if the client has access
     */
    
    // Check if no arguments.
    if (argc < 1)
    {
        #if defined TRANSLATIONS_MANAGER
            // Display usage info.
            TransMgr_PrintText(SERVER_INDEX, MsgFormat_Plugin, MsgType_Server, _, false, "LogMgr cmd whitelistadd usage");
        #else
            ReplyToCommand(SERVER_INDEX, "Adds one or more modules to the whitelist.  Usage: project_log_whitelist_add <moduleshortname [module2] ...\nRelated command(s): project_log_whitelist_remove");
        #endif
        
        return Plugin_Handled;
    }
    
    decl String:strModuleID[16];
    new Module:module;
    
    // Loop through each argument.
    for (new arg = 1; arg <= argc; arg++)
    {
        // Get argument string.
        GetCmdArg(arg, strModuleID, sizeof(strModuleID));
        module = ModuleMgr_FindByID(StringToInt(strModuleID), strModuleID);
        
        if (module == INVALID_MODULE)
        {
            #if defined TRANSLATIONS_MANAGER
                // Reply with an error and skip to next argument.
                TransMgr_PrintText(SERVER_INDEX, MsgFormat_Plugin, MsgType_Server, _, false, "ModuleMgr invalid module", strModuleID);
            #else
                ReplyToCommand(SERVER_INDEX, "Invalid module: \"%s\"", strModuleID);
            #endif
            
            continue;
        }
        
        LogMgr_WhitelistAdd(module);
        
        #if defined TRANSLATIONS_MANAGER
            TransMgr_PrintText(SERVER_INDEX, MsgFormat_Both, MsgType_Server, module, false, "LogMgr cmd whitelistadd");
        #else
            ReplyToCommand(SERVER_INDEX, "Module has been added to the whitelist.");
        #endif
    }
    
    // Say that we handled the command so CS:S doesn't see it and print "Unknown command"
    return Plugin_Handled;
}

/**
 * Command callback: project_log_whitelist_remove
 * Remove one or modules from the whitelist
 * 
 * @param argc      The number of arguments that the server sent with the command.
 */
public Action:Command_LogWhitelistRemove(argc)
{
    /**
     * Check if the client has access
     */
    
    // Check if no arguments.
    if (argc < 1)
    {
        #if defined TRANSLATIONS_MANAGER
            // Display usage info.
            TransMgr_PrintText(SERVER_INDEX, MsgFormat_Plugin, MsgType_Server, _, false, "LogMgr cmd whitelistremove usage");
        #else
            ReplyToCommand(SERVER_INDEX, "Removes one or more modules from the whitelist.  Usage: project_log_whitelist_remove <moduleshortname [module2] ...\nRelated command(s): project_log_whitelist_add");
        #endif
        
        return Plugin_Handled;
    }
    
    decl String:strModuleID[16];
    new Module:module;
    
    // Loop through each argument.
    for (new arg = 1; arg <= argc; arg++)
    {
        // Get argument string.
        GetCmdArg(arg, strModuleID, sizeof(strModuleID));
        module = ModuleMgr_FindByID(StringToInt(strModuleID), strModuleID);
        
        if (module == INVALID_MODULE)
        {
            #if defined TRANSLATIONS_MANAGER
                // Reply with an error and skip to next argument.
                TransMgr_PrintText(SERVER_INDEX, MsgFormat_Plugin, MsgType_Server, _, false, "ModuleMgr invalid module", strModuleID);
            #else
                ReplyToCommand(SERVER_INDEX, "Invalid module: \"%s\"", strModuleID);
            #endif
            
            continue;
        }
        
        LogMgr_WhitelistRemove(module);
        
        #if defined TRANSLATIONS_MANAGER
            TransMgr_PrintText(SERVER_INDEX, MsgFormat_Both, MsgType_Server, module, false, "LogMgr cmd whitelistremove");
        #else
            ReplyToCommand(SERVER_INDEX, "Module has been added to the whitelist.");
        #endif
    }
    
    // Say that we handled the command so CS:S doesn't see it and print "Unknown command"
    return Plugin_Handled;
}

/**
 * These are stock functions to return the value of any of the log manager's cvars.
 * This allows other base components/modules to read log cvars.
 */

stock bool:LogMgr_CvarLog()
{
    return GetConVarBool(cvarLog);
}

stock bool:LogMgr_CvarWhitelist()
{
    return GetConVarBool(cvarLogWhitelist);
}

stock bool:LogMgr_CvarPrintAdmins()
{
    return GetConVarBool(cvarLogPrintAdmins);
}

stock bool:LogMgr_CvarPrintPublic()
{
    return GetConVarBool(cvarLogPrintPublic);
}
