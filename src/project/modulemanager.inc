/*
 * ============================================================================
 *
 *  Project
 *
 *  File:          modulemanager.inc
 *  Type:          Base
 *  Description:   Manages project modules.
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
//     Public
// ---------------

// See project.inc

// ---------------
//     Private
// ---------------

/**
 * This is a macro that takes a module and returns the handle to it's data array.
 */
#define MM_HANDLE_FROM_ID(%1) GetArrayCell(g_adtModuleList, _:%1)

/**
 * Defines the block of data in the module data arrays that contains module-defined data.
 */
#define MODULE_DATA g_iMMAllocatedIndexes[0]

/**
 * The max number of cells needed for the module manager's allocated index.
 */
#define MM_DATA_MAX_CELLS MODULE_DATA_CELL_COUNT
// The number of max cells needed is equal to the number of cells needed to store all data for enum ModuleData.

/**
 * Base array that contains all module data array handles.
 */
new Handle:g_adtModuleList;

/**
 * Global variable that indicates the next module data array index available.
 */
new g_iModuleArrayNextIndex;

/**
 * Array to store the index of the allocated space in the module data arrays for the module manager.
 */
new g_iMMAllocatedIndexes[1];

/**
 * The max number of cells needed per block of data in the module data array.
 */
new g_iMMMaxBlockSize;

// **********************************************
//                 Forwards
// **********************************************

/**
 * Plugin has loaded.
 */
ModuleMgr_OnPluginStart()
{
    // Create the adt array used to store module data array handles.
    if (g_adtModuleList == INVALID_HANDLE)
        g_adtModuleList = CreateArray();
    
    // The next open index is 0, the first one.
    g_iModuleArrayNextIndex = 0;
    
    // Allocate 1 index for the data we want to store for each module.
    ModuleMgr_Allocate(1, g_iMMAllocatedIndexes);
    
    // Initialize the max block size to the number of cells needed for the module manager.
    g_iMMMaxBlockSize = MM_DATA_MAX_CELLS;
    
    // Now check each of the other base project files if they need more cells, and set the max to that if true.
    
    #if defined TRANSLATIONS_MANAGER
        if (TM_DATA_MAX_CELLS > g_iMMMaxBlockSize)
            g_iMMMaxBlockSize = TM_DATA_MAX_CELLS;
    #endif
    
    #if defined EVENT_MANAGER
        if (EM_DATA_MAX_CELLS > g_iMMMaxBlockSize)
            g_iMMMaxBlockSize = EM_DATA_MAX_CELLS;
    #endif
    
    #if defined LOG_MANAGER
        if (LM_DATA_MAX_CELLS > g_iMMMaxBlockSize)
            g_iMMMaxBlockSize = LM_DATA_MAX_CELLS;
    #endif
    
    #if defined PROJECT_BASE_CMD
        // Register the base command of the project module system.
        RegConsoleCmd(PROJECT_BASE_CMD, Command_Base, "Print all module data for a specific module, or all modules. Usage: project_module_info [module partial name]");
    #endif
}

// **********************************************
//                Public API
// **********************************************

/**
 * Registers a new module with the project base.
 * 
 * @param moduledata    Array populated with the module's data.  See enum ModuleData.
 * 
 * @return              A unique module identifier.  INVALID_MODULE if registration failed.
 * Note: The IDs used in the code starts from 0, while client interaction starts from 1.
 * So "module ID" starts from 1, while "module identifier" starts from 0.  Modules shouldn't have to worry about this.
 */
stock Module:ModuleMgr_Register(moduledata[ModuleData])
{
    // This is the array that will hold all the actual module data.
    new Handle:adtModule = CreateArray(g_iMMMaxBlockSize);
    
    // Push all the given module data to the new array.
    // This is being pushed into our allocated space for module data.
    PushArrayArray(adtModule, moduledata[0]);
    
    // Forward event to other base project files.
    
    #if defined EVENT_MANAGER
        EventMgr_OnModuleRegister(adtModule);
    #endif
    
    #if defined TRANSLATIONS_MANAGER
        TransMgr_OnModuleRegister(adtModule);
    #endif
    
    #if defined LOG_MANAGER
        LogMgr_OnModuleRegister(adtModule);
    #endif
    
    // Store the handle in the global module list.
    new module = PushArrayCell(g_adtModuleList, adtModule);
    
    // Return the module identifier.
    return Module:module;
}

/**
 * Returns if a module identifier is valid or not.
 * 
 * @param module    The module identifier to check validity of.
 * 
 * @return          True if the module identifier is valid, false if not.
 */
stock bool:ModuleMgr_IsModuleValid(Module:module)
{
    return (_:module > -1 && _:module < MODULE_COUNT);
}

/**
 * Disables a registered module.
 * If the module is already disabled, nothing will change.
 * 
 * @param module    The module to disable.
 */
stock ModuleMgr_Disable(Module:module)
{
    // Set the ModuleData_Disabled data to 'true'.
    ModuleMgr_WriteCell(module, ModuleData_Disabled, true);
}

/**
 * Enables a registered module.
 * If the module is already enabled, nothing will change.
 * 
 * @param module    The module to enable.
 */
stock ModuleMgr_Enable(Module:module)
{
    // Set the ModuleData_Disabled data to 'false'.
    ModuleMgr_WriteCell(module, ModuleData_Disabled, false);
}

/**
 * Returns if a module is disabled.
 * 
 * @param module    The module to check.
 * 
 * @return          True if the module is disabled, false if enabled.
 */
stock bool:ModuleMgr_IsDisabled(Module:module)
{
    // Read the value in ModuleData_Disabled and return the cell as a bool.
    return bool:ModuleMgr_ReadCell(module, ModuleData_Disabled);
}

/**
 * Finds any modules matching the given information.
 * 
 * @param data      The data to match the value with.
 * @param value     The value of the data that must match.
 * @param modules   Output array containing a list of modules that match. (optional)
 * @param matched   The number of modules that matched. (optional)
 * 
 * @return          The first module whose data value matches the given value.  INVALID_MODULE if no modules matched.
 */
stock Module:ModuleMgr_Find(ModuleData:data, any:value, Module:modules[] = {INVALID_MODULE}, &matched = 0)
{
    new matchcount;
    new Module:module;
    
    // Loop through all the modules.
    for (new moduleindex = 0; moduleindex < MODULE_COUNT; moduleindex++)
    {
        // Read moduleindex as a Module type.
        module = Module:moduleindex;
        
        // If the data matches, then add the module identifier to the output array.
        if (ModuleMgr_ReadCell(module, data) == value)
        {
            modules[matchcount] = module;
            matchcount++;
        }
    }
    
    // Return the first module that matched.
    if (matchcount > 0)
    {
        return modules[0];
    }
    
    // No modules matched.
    return INVALID_MODULE;
}

/**
 * Finds any modules matching the given information.
 * 
 * @param data          The data to match the string with.
 * @param value         The string value of the data that must match.
 * @param casesensitive True if the strings must also match case.
 * @param modules       Output array containing a list of module identifiers that match. (optional)
 * @param matched       The number of modules that matched. (optional)
 * 
 * @return              The first module whose data value matches the given value.  INVALID_MODULE if no modules matched.
 */
stock Module:ModuleMgr_FindByString(ModuleData:data, const String:value[], bool:casesensitive = false, Module:modules[] = {INVALID_MODULE}, &matched = 0)
{
    decl String:datastring[MODULE_DATA_LONGEST_STRING];
    new matchcount;
    new Module:module;
    
    // Loop through all the modules.
    for (new moduleindex = 0; moduleindex < MODULE_COUNT; moduleindex++)
    {
        // Read moduleindex as a Module type.
        module = Module:moduleindex;
        
        // If the string matches, then add the module identifier to the output array.
        ModuleMgr_ReadString(module, data, datastring, sizeof(datastring));
        if (StrEqual(datastring, value, casesensitive))
        {
            modules[matchcount] = module;
            matchcount++;
        }
    }
    
    // Return the first module that matched.
    if (matchcount > 0)
    {
        return modules[0];
    }
    
    // No modules matched.
    return INVALID_MODULE;
}

/**
 * Module data reader that returns all available module data.
 * Modules can use this for communicating with other modules.
 * 
 * @param module        The module whose data to read.
 * @param moduledata    Output array for all module data.  See enum ModuleData.
 */
stock ModuleMgr_ReadAll(Module:module, moduledata[ModuleData])
{
    GetArrayArray(MM_HANDLE_FROM_ID(module), MODULE_DATA, moduledata[0], sizeof(moduledata));
}

/**
 * Module data reader for any data type except strings.
 * Modules can use this for communicating with other modules.
 * 
 * @param module    The module whose cell data to read.
 * @param data      The data to get the value of.  See enum ModuleData.
 * 
 * @return          The integer value of the desired module data.
 */
stock ModuleMgr_ReadCell(Module:module, ModuleData:data)
{
    new moduledata[ModuleData];
    GetArrayArray(MM_HANDLE_FROM_ID(module), MODULE_DATA, moduledata[0], sizeof(moduledata));
    
    // Return the value.
    return _:moduledata[data];
}

/**
 * Module data reader for any string typed values.
 * Modules can use this for communicating with other modules. 
 * 
 * @param module    The module whose string data to read.
 * @param data      The data to get the value of.  See enum ModuleData.
 * @param output    Output variable for the data read.
 * @param maxlen    The max length of the output string.
 */
stock ModuleMgr_ReadString(Module:module, ModuleData:data, String:output[], maxlen)
{
    new moduledata[ModuleData];
    GetArrayArray(MM_HANDLE_FROM_ID(module), MODULE_DATA, moduledata[0], sizeof(moduledata));
    
    // Copy full name to output
    strcopy(output, maxlen, String:moduledata[data]);
}

// **********************************************
//   Private API (For base project files only)
// **********************************************

/**
 * Validates all values in a ModuleData array, and prints errors for each invalid value.
 * This is meant for other base project files to use, modules shouldn't need this. 
 * 
 * @param moduledata    Array of module data to validate.
 * @param moduleflags   0 if all values were valid.  Non-zero means there is a list of flags that failed.
 * 
 * @return              True if validation was successful, false if there were invalid values.
 */
stock ModuleMgr_Validate(moduledata[ModuleData], &moduleflags = 0)
{
    // ModuleData_Disabled can't be invalid.
    // ModuleData_Hidden can't be invalid.
    
    // Validate ModuleData_FullName.
    if (!moduledata[ModuleData_FullName][0])
    {
        moduleflags |= MODULE_DATA_FULLNAME;
        LogError("[Module Manager] [Registration Failed] Module needs a full name.");
    }
    
    // Validate ModuleData_ShortName.
    if (!moduledata[ModuleData_ShortName][0])
    {
        moduleflags |= MODULE_DATA_SHORTNAME;
        LogError("[Module Manager] [Registration Failed] Module \"%s\" needs a (unique) short name.", moduledata[ModuleData_FullName]);
    }
    else
    {
        // Shortname must be unique.
        new Module:clashmodule = ModuleMgr_FindByString(ModuleData_ShortName, moduledata[ModuleData_ShortName]);
        if (clashmodule != INVALID_MODULE)
        {
            decl String:clashmodulefullname[32];
            ModuleMgr_ReadString(clashmodule, ModuleData_FullName, clashmodulefullname, sizeof(clashmodulefullname));
            
            moduleflags |= MODULE_DATA_SHORTNAME;
            LogError("[Module Manager] [Registration Failed] Module \"%s\" short name (%s) clashes with that of module \"%s\"", moduledata[ModuleData_FullName],moduledata[ModuleData_ShortName], clashmodulefullname);
        }
    }
    
    // ModuleData_Description can't be invalid.
}

/**
 * Finds any modules matching the given module ID or shortname.
 * This should only be used if input it coming from a client.  Otherwise use ModuleMgr_FindByString.
 * This is meant for other base project files to use, modules shouldn't need this. 
 * 
 * @param value         The module ID to match to a module. (This will be matched first if both are inputed)
 * @param shortname     The short name to match to a module.
 * 
 * @return              The module whose module index or shortname matched. (in that order)  INVALID_MODULE if no modules matched.
 */
stock Module:ModuleMgr_FindByID(moduleID = 0, const String:moduleshortname[] = "")
{
    decl String:matchshortname[16];
    new Module:module;
    
    // Loop through all the modules.
    for (new moduleindex = 0; moduleindex < MODULE_COUNT; moduleindex++)
    {
        // Read moduleindex as a Module type.
        module = Module:moduleindex;
        
        // If the module is hidden, then ignore it.
        if (bool:ModuleMgr_ReadCell(module, ModuleData_Hidden))
            continue;
        
        // Compare the module ID to each module.
        if (moduleID == MODULE_TO_ID(module))
            return module;
        
        // Compare the short name to each module.
        ModuleMgr_ReadString(module, ModuleData_ShortName, matchshortname, sizeof(matchshortname));
        if (StrEqual(moduleshortname, matchshortname, false))
            return module;
    }
    
    // No modules matched.
    return INVALID_MODULE;
}

/**
 * Reserve space in the module data array.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param count     The number of indexes per module that's needed.
 * @param indexes   The indexes within the array that have been allocated to you.  (number of elements = param count)
 */
stock ModuleMgr_Allocate(count, indexes[])
{
    // While the count is above 0, allocate each index until the count has been reduced to 0.
    new index = 0;
    while (count > 0)
    {
        count--;
        indexes[index] = g_iModuleArrayNextIndex;
        g_iModuleArrayNextIndex++;
        index++;
    }
}

/**
 * Module data writer that overwrites all data for a module with the given data.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param module        The module whose data to write.
 * @param moduledata    New data to replace the old data.  See enum ModuleData.
 */
stock ModuleMgr_WriteAll(Module:module, moduledata[ModuleData])
{
    SetArrayArray(MM_HANDLE_FROM_ID(module), MODULE_DATA, moduledata[0], sizeof(moduledata));
}

/**
 * Module data writer that writes a specified non-string data value.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param module    The module whose cell data to write.
 * @param data      Data to write new value to.  See enum ModuleData.
 * @param value     Any cell value to write as the new data.
 */
stock ModuleMgr_WriteCell(Module:module, ModuleData:data, any:value)
{
    // Read all the module data.
    new moduledata[ModuleData];
    ModuleMgr_ReadAll(module, moduledata);
    
    // Change the value of the specified module data.
    moduledata[data] = value;
    
    // Overwrite the old array with the modified one.
    SetArrayArray(MM_HANDLE_FROM_ID(module), MODULE_DATA, moduledata[0], sizeof(moduledata));
}

/**
 * Module data writer that writes a specified string data value.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param module    The module whose string data to write.
 * @param data      Data to write new string to.  See enum ModuleData.
 * @param maxlen    The max length of the data value.  See enum ModuleData.
 * @param value     A string to write as the new data value.
 */
stock ModuleMgr_WriteString(Module:module, ModuleData:data, maxlen, const String:value[])
{
    // Read all the module data.
    new moduledata[ModuleData];
    ModuleMgr_ReadAll(module, moduledata);
    
    // Change the value of the specified module data.
    strcopy(String:moduledata[data], maxlen, value);
    
    // Overwrite the old array with the modified one.
    SetArrayArray(MM_HANDLE_FROM_ID(module), MODULE_DATA, moduledata[0], sizeof(moduledata));
}

/**
 * Returns a module's handle given it's module identifier.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param module    The module to get the handle for.
 * 
 * @return          The array handle to the module.
 */ 
stock Handle:ModuleMgr_GetModuleArray(Module:module)
{
    // Return the module's array handle.
    return MM_HANDLE_FROM_ID(module);
}

/**
 * Returns a module's ID given it's array handle.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param adtModule The module's array handle to get the module identifier for.
 * 
 * @return          The module's module identifier, INVALID_MODULE if no module matched the handle.
 */
stock Module:ModuleMgr_GetModule(Handle:adtModule)
{
    new Module:module;
    
    // Loop through all the modules.
    for (new moduleindex = 0; moduleindex < MODULE_COUNT; moduleindex++)
    {
        // Read moduleindex as a Module type.
        module = Module:moduleindex;
        
        // If the handle's match, then return the current index, which is the module identifier.
        if (adtModule == MM_HANDLE_FROM_ID(module))
            return module;
    }
    
    return INVALID_MODULE;
}

/**
 * Allows other modules to get the handle of the main array to manipulate data.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @return  The handle to the array with all module data handles.
 */
stock Handle:ModuleMgr_GetList()
{
    return g_adtModuleList;
}

// **********************************************
//                Base Command
// **********************************************

/**
 * Returns true if the project base cmd isn't available, false means it is available.
 */
stock bool:ModuleMgr_CreateBackupCmd()
{
    new bool:create = false;
    
    #if !defined PROJECT_BASE_CMD
        create = true;
    #endif
    
    #if !defined TRANSLATIONS_MANAGER
        create = true;
    #endif
    
    return create;
}

#if defined PROJECT_BASE_CMD
    #if defined TRANSLATIONS_MANAGER

/**
 * Base command sub command types.
 */
enum BaseSubCmdType
{
    SubCmdType_Invalid = -1,    /** Invalid sub-command. */
    SubCmdType_Command,         /** A command with no additional arguments. */
    SubCmdType_CommandWithArg,  /** A command with a required argument. */
    SubCmdType_Accessor         /** Accesses a deeper sub-section. */
}

/**
 * First level of base's commands and section accessors.
 * sm *<level1>* [level2] [arguments]
 */
new const String:g_BaseCmdsLvl1[][] = {
    "credits",      // Command
    "modules",      // Section Accessor
    "version"       // Command
};

/**
 * Second level of the second first level section accessor.
 * Confused?  Rephrased: The sub-commands under the "modules" keyword.
 * sm <level1> *[level2]* [arguments]
 */
//new const String:g_BaseCmdsLvl2[][][] = {   // Should use this when the compiler bug is fixed.
new const String:g_BaseCmdsLvl2[][][32] = {
    {"", "", "", "", "", "", ""},
    {   "info",         // Command with argument
        "list",         // Command
        "enable",       // Command with argument
        "refresh",      // Command
        "reload",       // Command with argument
        "disable",      // Command with argument
        "disable_all"}, // Command
    {"", "", "", "", "", "", ""}
};

/**
 * Macros to make accessing [0, x] indexes prettier by changing the bounds to [1, x+1]
 */
#define BASECMD_LEVEL(%1) %1 - 1
#define BASECMD_ARG(%1) %1 - 1
/**
 * @endsection
 */

/**
 * A list that indicates if any of the above sub-commands are commands or section accessors.
 */
new const BaseSubCmdType:g_BaseSubCmdType[][] = {
    {   SubCmdType_Command,
        SubCmdType_Accessor,
        SubCmdType_Command,
        SubCmdType_Invalid,
        SubCmdType_Invalid,
        SubCmdType_Invalid,
        SubCmdType_Invalid },
    {
        SubCmdType_CommandWithArg,
        SubCmdType_Command,
        SubCmdType_CommandWithArg,
        SubCmdType_Command,
        SubCmdType_CommandWithArg,
        SubCmdType_CommandWithArg,
        SubCmdType_Command}
};

/**
 * Finds the index the command is located at in the arrays above.
 * This is meant for other base project files to use, modules shouldn't need this. 
 * 
 * @param cmd       The command to search for.
 * @param level     The level the command is in.
 * @param cmdindex  If searching level 2, this is the level 1 cmd that the level 2 cmds are under.
 * 
 * @return      The index in the array where this command is at.  -1 if not found.
 */
stock ModuleMgr_BaseCmdFind(const String:cmd[], level, cmdindex = -1)
{
    if (level == 1)
    {
        // Loop through level 1 cmds.
        for (new iCmd = 0; iCmd < sizeof(g_BaseCmdsLvl1); iCmd++)
        {
            if (StrEqual(cmd, g_BaseCmdsLvl1[iCmd], false))
                return iCmd;
        }
    }
    else if (level == 2)
    {
        // Loop through level 2 cmds.
        for (new iCmd = 0; iCmd < sizeof(g_BaseCmdsLvl1); iCmd++)
        {
            if (StrEqual(cmd, g_BaseCmdsLvl2[cmdindex][iCmd], false))
                return iCmd;
        }
    }
    
    return -1;
}

/**
 * Command callback: <basecommand>  See define PROJECT_BASE_CMD
 * Root command for plugin and module management.
 * 
 * @param client    The client index.  Or SERVER_INDEX if coming from the server.
 * @param argc      The number of arguments that the client sent with the command.
 */
public Action:Command_Base(client, argc)
{
    if (client > SERVER_INDEX)
        ModuleMgr_BaseCmdNonAdmin(client);
    else
        ModuleMgr_BaseCmdAdmin(client, argc);
    
    // Say that we handled the command so CS:S doesn't see it and print "Unknown command"
    return Plugin_Handled;
}

/**
 * Handle PROJECT_BASE_CMD command when a non-admin uses it.
 * 
 * @param client    The client index.
 * @param argc      The number of arguments that the client sent with the command.
 */
ModuleMgr_BaseCmdNonAdmin(client)
{
    decl String:arg[sizeof(g_BaseCmdsLvl1[])];
    GetCmdArg(1, arg, sizeof(arg));
    
    // The "credits" command.
    if (StrEqual(arg, g_BaseCmdsLvl1[0], false))
    {
        // Print credits.
        TransMgr_PrintText(client, MsgFormat_None, MsgType_Reply, _, false, "Project credits");
    }
    // The "modules" command.
    else if (StrEqual(arg, g_BaseCmdsLvl1[1], false))
    {
        // List modules.
        ModuleMgr_PrintModules(client);
    }
    else
    {
        TransMgr_PrintText(client, MsgFormat_None, MsgType_Reply, _, false, "ModuleMgr cmd base non-admin syntax", PROJECT_FULLNAME, PROJECT_VERSION, PROJECT_AUTHOR, PROJECT_BASE_CMD, PROJECT_BASE_CMD);
        return;
    }
}

/**
 * Handle PROJECT_BASE_CMD command when an admin or server uses it.
 * 
 * @param client    The client index.
 * @param argc      The number of arguments that the client sent with the command.
 */
ModuleMgr_BaseCmdAdmin(client, argc)
{
    // Determine how to print the message to server/client.
    new MsgTypes:msgtype = (client == SERVER_INDEX) ? MsgType_Server : MsgType_Console;
    
    // Get all arguments.
    if (argc == 0) argc++;
    new String:strArgs[argc][16];
    for (new arg = 1; arg <= argc; arg++)
        GetCmdArg(arg, strArgs[BASECMD_ARG(arg)], 16);
                                                   // ^^^ SHOULD BE sizeof(strArgs[].  But compiler bug.
    decl String:phraseformat[64];
    
    // Level 1 validation.
    
    new bool:valid_exists = false;
    new bool:valid_arg = false;
    new iLvl1Cmd;
    for (new iCmd = 0; iCmd < sizeof(g_BaseCmdsLvl1); iCmd++)
    {
        if (StrEqual(g_BaseCmdsLvl1[iCmd], strArgs[BASECMD_ARG(1)], false))
        {
            valid_exists = true;
            iLvl1Cmd = iCmd;
            
            // Set 'valid_arg' to true if an argument was given or if it doesn't need an argument.
            // False if the command needs an argument and none was given.
            if (g_BaseSubCmdType[BASECMD_LEVEL(1)][iCmd] == SubCmdType_CommandWithArg)
                valid_arg = (argc >= 2) ? (bool:strArgs[BASECMD_ARG(2)][0]) : false;
            else
                valid_arg = true;
            
            break;
        }
    }
    
    // Print level 1 syntax.
    if (!valid_exists)
    {
        TransMgr_PrintText(client, MsgFormat_None, msgtype, _, false, "ModuleMgr cmd base lvl1 syntax", PROJECT_FULLNAME, PROJECT_BASE_CMD, g_BaseCmdsLvl1[0], g_BaseCmdsLvl1[1], g_BaseCmdsLvl1[2]);
        return;
    }
    
    // Print level 1 sub-command syntax.
    if (!valid_arg)
    {
        // Format the phrase name, because it depends on which command is being entered.
        Format(phraseformat, sizeof(phraseformat), "ModuleMgr cmd base lvl1 %s syntax", strArgs[BASECMD_ARG(1)]);
        TransMgr_PrintText(client, MsgFormat_Plugin, msgtype, _, false, phraseformat);
        return;
    }
    
    // Level 2 validation.
    
    // If the level 1 sub-command is an accessor, then validate it's sub-command.
    // Otherwise, validate this command string by leaving 'valid' as 'true'.
    if (g_BaseSubCmdType[BASECMD_LEVEL(1)][iLvl1Cmd] == SubCmdType_Accessor)
    {
        valid_exists = false;
        valid_arg = false;
        for (new iSubCmd = 0; iSubCmd < sizeof(g_BaseCmdsLvl2[]); iSubCmd++)
        {
            if (StrEqual(g_BaseCmdsLvl2[iLvl1Cmd][iSubCmd], strArgs[BASECMD_ARG(2)], false))
            {
                valid_exists = true;
                
                // Set 'valid_arg' to true if an argument was given or if it doesn't need an argument.
                // False if the command needs an argument and none was given.
                if (g_BaseSubCmdType[BASECMD_LEVEL(2)][iSubCmd] == SubCmdType_CommandWithArg)
                    valid_arg = (argc >= 3) ? (bool:strArgs[BASECMD_ARG(3)][0]) : false;
                else
                    valid_arg = true;
                
                break;
            }
        }
        
        // Print level 2 sub-command syntax.
        if (!valid_exists)
        {
            // Format the phrase name, because it depends on which sub-command accessor is being used.
            Format(phraseformat, sizeof(phraseformat), "ModuleMgr cmd base lvl2 accessor %s syntax", strArgs[BASECMD_ARG(1)]);
            TransMgr_PrintText(client, MsgFormat_None, msgtype, _, false, phraseformat, PROJECT_FULLNAME, g_BaseCmdsLvl2[iLvl1Cmd][0], g_BaseCmdsLvl2[iLvl1Cmd][1], g_BaseCmdsLvl2[iLvl1Cmd][2], g_BaseCmdsLvl2[iLvl1Cmd][3], g_BaseCmdsLvl2[iLvl1Cmd][4], g_BaseCmdsLvl2[iLvl1Cmd][5], g_BaseCmdsLvl2[iLvl1Cmd][6]);
            return;
        }
        
        // Print level 2 sub-sub-command syntax.
        if (!valid_arg)
        {
            // Format the phrase name, because it depends on which command is being entered.
            Format(phraseformat, sizeof(phraseformat), "ModuleMgr cmd base lvl2 %s %s syntax", strArgs[BASECMD_ARG(1)], strArgs[BASECMD_ARG(2)]);
            TransMgr_PrintText(client, MsgFormat_Plugin, msgtype, _, false, phraseformat, PROJECT_BASE_CMD, strArgs[BASECMD_ARG(1)], strArgs[BASECMD_ARG(2)]);
            return;
        }
    }
    
    // Pass off the validified arguments to another function.
    ModuleMgr_BaseCmdParse(client, strArgs);
}

/**
 * The "control center" that redirects all commands to their respective functions.
 * This is meant for other base project files to use, modules shouldn't need this. 
 * 
 * @param client    The client calling the command.
 * @param strArgs   The list of arguments that were validated by the base command callback.
 */
ModuleMgr_BaseCmdParse(client, const String:strArgs[][])
{
    // The "credits" sub-command.
    if (StrEqual(strArgs[BASECMD_ARG(1)], g_BaseCmdsLvl1[0], false))
    {
        // Print credits.
        TransMgr_PrintText(client, MsgFormat_None, MsgType_Reply, _, false, "Project credits");
    }
    // The "modules" accessor.
    else if (StrEqual(strArgs[BASECMD_ARG(1)], g_BaseCmdsLvl1[1], false))
    {
        // Find the array index that the cmd is in, to access it's sub-commands for checking.
        new cmdindex = ModuleMgr_BaseCmdFind(strArgs[BASECMD_ARG(1)], 1);
        
        // The "info" sub-command.
        if (StrEqual(strArgs[BASECMD_ARG(2)], g_BaseCmdsLvl2[cmdindex][0], false))
        {
            ModuleMgr_PrintModuleInfo(client, strArgs[BASECMD_ARG(3)]);
        }
        // The "list" sub-command.
        else if (StrEqual(strArgs[BASECMD_ARG(2)], g_BaseCmdsLvl2[cmdindex][1], false))
        {
            ModuleMgr_PrintModules(client);
        }
        // The "enable" sub-command.
        else if (StrEqual(strArgs[BASECMD_ARG(2)], g_BaseCmdsLvl2[cmdindex][2], false))
        {
            // Validate the module ID/shortname.
            new Module:module = ModuleMgr_FindByID(StringToInt(strArgs[BASECMD_ARG(3)]), strArgs[BASECMD_ARG(3)]);
            if (module == INVALID_MODULE)
            {
                TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, false, "ModuleMgr invalid module", strArgs[BASECMD_ARG(3)]);
                return;
            }
            
            ModuleMgr_Enable(module);
            TransMgr_PrintText(client, MsgFormat_Both, MsgType_Reply, module, false, "ModuleMgr cmd base lvl2 modules enable");
        }
        // The "refresh" sub-command.
        else if (StrEqual(strArgs[BASECMD_ARG(2)], g_BaseCmdsLvl2[cmdindex][3], false))
        {
            // Loop through all the modules.
            new Module:module;
            for (new moduleindex = 0; moduleindex < MODULE_COUNT; moduleindex++)
            {
                // Read moduleindex as a Module type.
                module = Module:moduleindex;
                
                ModuleMgr_Disable(module);
                ModuleMgr_Enable(module);
            }
            
            TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, false, "ModuleMgr cmd base lvl2 modules refresh");
        }
        // The "reload" sub-command.
        else if (StrEqual(strArgs[BASECMD_ARG(2)], g_BaseCmdsLvl2[cmdindex][4], false))
        {
            // Validate the module ID/shortname.
            new Module:module = ModuleMgr_FindByID(StringToInt(strArgs[BASECMD_ARG(3)]), strArgs[BASECMD_ARG(3)]);
            if (module == INVALID_MODULE)
            {
                TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, false, "ModuleMgr invalid module", strArgs[BASECMD_ARG(3)]);
                return;
            }
            
            ModuleMgr_Disable(module);
            ModuleMgr_Enable(module);
            TransMgr_PrintText(client, MsgFormat_Both, MsgType_Reply, module, false, "ModuleMgr cmd base lvl2 modules reload");
        }
        // The "disable" sub-command.
        else if (StrEqual(strArgs[BASECMD_ARG(2)], g_BaseCmdsLvl2[cmdindex][5], false))
        {
            // Validate the module ID/shortname.
            new Module:module = ModuleMgr_FindByID(StringToInt(strArgs[BASECMD_ARG(3)]), strArgs[BASECMD_ARG(3)]);
            if (module == INVALID_MODULE)
            {
                TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, false, "ModuleMgr invalid module", strArgs[BASECMD_ARG(3)]);
                return;
            }
            
            ModuleMgr_Disable(module);
            TransMgr_PrintText(client, MsgFormat_Both, MsgType_Reply, module, false, "ModuleMgr cmd base lvl2 modules disable");
        }
        // The "disable_all" sub-command.
        else if (StrEqual(strArgs[BASECMD_ARG(2)], g_BaseCmdsLvl2[cmdindex][6], false))
        {
            // Loop through all the modules.
            new Module:module;
            for (new moduleindex = 0; moduleindex < MODULE_COUNT; moduleindex++)
            {
                // Read moduleindex as a Module type.
                module = Module:moduleindex;
                
                ModuleMgr_Disable(module);
            }
            
            TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, false, "ModuleMgr cmd base lvl2 modules disable_all");
        }
    }
    // The "version" sub-command.
    else if (StrEqual(strArgs[BASECMD_ARG(1)], g_BaseCmdsLvl1[2], false))
    {
        #if defined VERSION_INFO
            VersionPrint(client);
        #else
            TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, false, "Missing component", "versioninfo.inc");
        #endif
    }
}

/**
 * Prints the information stored about a module.
 * This is meant for other base project files to use, modules shouldn't need this. 
 * 
 * @param client        Client to print information to.
 * @param strModuleID   Either the module's shortname or it's module ID.
 */
stock ModuleMgr_PrintModuleInfo(client, const String:strModuleID[])
{
    // Validate the module ID/shortname.
    new Module:module = ModuleMgr_FindByID(StringToInt(strModuleID), strModuleID);
    if (module == INVALID_MODULE)
    {
        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, false, "ModuleMgr invalid module", strModuleID);
        return;
    }
    
    // Determine how to print the message to server/client.
    new MsgTypes:msgtype = (client == SERVER_INDEX) ? MsgType_Server : MsgType_Console;
    
    decl String:modulefullname[32];
    decl String:moduleshortname[16];
    decl String:moduledesc[256];
    new bool:bModuleDisabled;
    decl String:strModuleEnabled[16];
    
    // Get the module's short name to match to the given partial name.
    ModuleMgr_ReadString(module, ModuleData_FullName, modulefullname, sizeof(modulefullname));
    ModuleMgr_ReadString(module, ModuleData_ShortName, moduleshortname, sizeof(moduleshortname));
    ModuleMgr_ReadString(module, ModuleData_Description, moduledesc, sizeof(moduledesc));
    bModuleDisabled = bool:ModuleMgr_ReadCell(module, ModuleData_Disabled);
    TransMgr_TranslateBoolToPhrase(client, !bModuleDisabled, BoolPhrase_YesNo, strModuleEnabled, sizeof(strModuleEnabled));
    
    // Print all the data stored in the module manager itself.
    TransMgr_PrintText(client, MsgFormat_None, msgtype, _, false, "ModuleMgr cmd base lvl2 modules info print", modulefullname, moduleshortname, moduledesc, strModuleEnabled);
    
    // Forward event to other project base components.
    #if defined EVENT_MANAGER
        EventMgr_OnPrintModuleInfo(client, module, msgtype);
    #endif
    
    #if defined TRANSLATIONS_MANAGER
        TransMgr_OnPrintModuleInfo(client, module, msgtype);
    #endif
    
    #if defined LOG_MANAGER
        LogMgr_OnPrintModuleInfo(client, module, msgtype);
    #endif
}

/**
 * Prints all registered modules.
 * This is meant for other base project files to use, modules shouldn't need this. 
 * 
 * @param client        Client to print information to.
 */
stock ModuleMgr_PrintModules(client)
{
    // Determine how to print the message to server/client.
    new MsgTypes:msgtype = (client == SERVER_INDEX) ? MsgType_Server : MsgType_Console;
    
    decl String:modulefullname[32];
    decl String:moduleshortname[16];
    new bool:bModuleDisabled;
    decl String:strModuleEnabled[16];
    decl String:line[256];
    
    new Module:module;
    new moduleID;
    
    // Loop through all the modules.
    for (new moduleindex = 0; moduleindex < MODULE_COUNT; moduleindex++)
    {
        // Read moduleindex as a Module type.
        module = Module:moduleindex;
        
        // If the module is hidden then don't list it.
        if (bool:ModuleMgr_ReadCell(module, ModuleData_Hidden))
            continue;
        
        // Get the module's module ID for client interaction.
        moduleID = MODULE_TO_ID(module);
        ModuleMgr_ReadString(module, ModuleData_FullName, modulefullname, sizeof(modulefullname));
        ModuleMgr_ReadString(module, ModuleData_ShortName, moduleshortname, sizeof(moduleshortname));
        bModuleDisabled = bool:ModuleMgr_ReadCell(module, ModuleData_Disabled);
        TransMgr_TranslateBoolToPhrase(client, !bModuleDisabled, BoolPhrase_EnabledDisabled, strModuleEnabled, sizeof(strModuleEnabled), true);
        
        SetGlobalTransTarget(client);
        Format(line, sizeof(line), "%t", "ModuleMgr cmd base lvl2 modules list print", modulefullname, strModuleEnabled, moduleshortname);
        
        // Format on the moduleID?
        (client == SERVER_INDEX) ?  Format(line, sizeof(line), "  %02d %s", moduleID, line) :
                                    Format(line, sizeof(line), "  %s", line);
        
        TransMgr_PrintText(client, MsgFormat_None, msgtype, _, true, line);
    }
}
#else   // If TRANSLATIONS_MANAGER isn't defined.
// How can we print the module information without translations to read from?
public Action:Command_Base(client, argc)
{
    ReplyToCommand(client, "The base command requires that the translations manager be compiled into the plugin.");
}
#endif

#endif
