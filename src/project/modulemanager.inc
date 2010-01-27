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
 * This is a macro that turns a module ID into the handle to it's data array.
 */
#define MM_HANDLE_FROM_ID(%1) GetArrayCell(g_adtModuleList, %1)

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
    
    
    // Create a module information print command.
    RegConsoleCmd("project_module_info", Command_ModuleInfo, "Print all module data for a specific module, or all modules. Usage: project_module_info [module partial name]");
}

// **********************************************
//                Public API
// **********************************************

/**
 * Registers a new module with the project base.
 * 
 * @param moduledata    Array populated with the module's data.  See enum ModuleData.
 * 
 * @return              The unique module ID.
 */
stock ModuleMgr_Register(moduledata[ModuleData])
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
    new moduleID = PushArrayCell(g_adtModuleList, adtModule);
    
    // Return the module ID.
    return moduleID;
}

/**
 * Disables a registered module.
 * If the module is already disabled, nothing will change.
 * 
 * @param moduleID  The module ID of the module to disable.
 */
stock ModuleMgr_Disable(moduleID)
{
    // Set the ModuleData_Disabled data to 'true'.
    ModuleMgr_WriteCell(moduleID, ModuleData_Disabled, true);
}

/**
 * Enables a registered module.
 * If the module is already enabled, nothing will change.
 * 
 * @param moduleID  The module ID of the module to enable.
 */
stock ModuleMgr_Enable(moduleID)
{
    // Set the ModuleData_Disabled data to 'false'.
    ModuleMgr_WriteCell(moduleID, ModuleData_Disabled, false);
}

/**
 * Returns if a module is disabled.
 * 
 * @param moduleID  The module ID of the module to check.
 * 
 * @return          True if the module is disabled, false if enabled.
 */
stock bool:ModuleMgr_IsDisabled(moduleID)
{
    // Read the value in ModuleData_Disabled and return the cell as a bool.
    return bool:ModuleMgr_ReadCell(moduleID, ModuleData_Disabled);
}

/**
 * Returns if a module ID is valid or not.
 * 
 * @param moduleID  The module ID to check validity of.
 * 
 * @return          True if the module ID is valid, false if not.
 */
stock bool:ModuleMgr_IsModuleValid(moduleID)
{
    return (moduleID > -1 && moduleID < MODULE_COUNT);
}

/**
 * Finds any modules matching the given information.
 * 
 * @param data      The data to match the value with.
 * @param value     The value of the data that must match.
 * @param modules   Output array containing a list of module IDs that match. (optional)
 * @param matched   The number of modules that matched. (optional)
 * 
 * @return          The first module whose data value matches the given value.  -1 if no modules matched.
 */
stock ModuleMgr_Find(ModuleData:data, any:value, modules[] = {0}, &matched = 0)
{
    new matchcount;
    
    // Loop through all the modules.
    for (new moduleID = 0; moduleID < MODULE_COUNT; moduleID++)
    {
        // If the data matches, then add the module ID to the output array.
        if (ModuleMgr_ReadCell(moduleID, data) == value)
        {
            modules[matchcount] = moduleID;
            matchcount++;
        }
    }
    
    // Return the first module that matched.
    if (matchcount > 0)
    {
        return modules[0];
    }
    
    // No modules matched.
    return -1;
}

/**
 * Finds any modules matching the given information.
 * 
 * @param data          The data to match the string with.
 * @param value         The string value of the data that must match.
 * @param casesensitive True if the strings must also match case.
 * @param modules       Output array containing a list of module IDs that match. (optional)
 * @param matched       The number of modules that matched. (optional)
 * 
 * @return              The first module whose data value matches the given value.  -1 if no modules matched.
 */
stock ModuleMgr_FindByString(ModuleData:data, const String:value[], bool:casesensitive = false, modules[] = {0}, &matched = 0)
{
    decl String:datastring[MODULE_DATA_LONGEST_STRING];
    new matchcount;
    
    // Loop through all the modules.
    for (new moduleID = 0; moduleID < MODULE_COUNT; moduleID++)
    {
        // If the string matches, then add the module ID to the output array.
        ModuleMgr_ReadString(moduleID, data, datastring, sizeof(datastring));
        if (StrEqual(datastring, value, casesensitive))
        {
            modules[matchcount] = moduleID;
            matchcount++;
        }
    }
    
    // Return the first module that matched.
    if (matchcount > 0)
    {
        return modules[0];
    }
    
    // No modules matched.
    return -1;
}

/**
 * Module data reader that returns all available module data.
 * Modules can use this for communicating with other modules.
 * 
 * @param moduleID      The unique ID given to the registered module.
 * @param moduledata    Output array for all module data.  See enum ModuleData.
 */
stock ModuleMgr_ReadAll(moduleID, moduledata[ModuleData])
{
    GetArrayArray(MM_HANDLE_FROM_ID(moduleID), MODULE_DATA, moduledata[0], sizeof(moduledata));
}

/**
 * Module data reader for any data type except strings.
 * Modules can use this for communicating with other modules.
 * 
 * @param moduleID  The unique ID given to the registered module.
 * @param data      The data to get the value of.  See enum ModuleData.
 * 
 * @return          The integer value of the desired module data.
 */
stock ModuleMgr_ReadCell(moduleID, ModuleData:data)
{
    new moduledata[ModuleData];
    GetArrayArray(MM_HANDLE_FROM_ID(moduleID), MODULE_DATA, moduledata[0], sizeof(moduledata));
    
    // Return the value.
    return _:moduledata[data];
}

/**
 * Module data reader for any string typed values.
 * Modules can use this for communicating with other modules. 
 * 
 * @param moduleID  The unique ID given to the registered module.
 * @param data      The data to get the value of.  See enum ModuleData.
 * @param output    Output variable for the data read.
 * @param maxlen    The max length of the output string.
 */
stock ModuleMgr_ReadString(moduleID, ModuleData:data, String:output[], maxlen)
{
    new moduledata[ModuleData];
    GetArrayArray(MM_HANDLE_FROM_ID(moduleID), MODULE_DATA, moduledata[0], sizeof(moduledata));
    
    // Copy full name to output
    strcopy(output, maxlen, String:moduledata[data]);
}

// **********************************************
//   Private API (For base project files only)
// **********************************************

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
 * @param moduleID      The unique ID given to the registered module.
 * @param moduledata    New data to replace the old data.  See enum ModuleData.
 */
stock ModuleMgr_WriteAll(moduleID, moduledata[ModuleData])
{
    SetArrayArray(MM_HANDLE_FROM_ID(moduleID), MODULE_DATA, moduledata[0], sizeof(moduledata));
}

/**
 * Module data writer that writes a specified non-string data value.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param moduleID  The unique ID given to the registered module.
 * @param data      Data to write new value to.  See enum ModuleData.
 * @param value     Any cell value to write as the new data.
 */
stock ModuleMgr_WriteCell(moduleID, ModuleData:data, any:value)
{
    // Read all the module data.
    new moduledata[ModuleData];
    ModuleMgr_ReadAll(moduleID, moduledata);
    
    // Change the value of the specified module data.
    moduledata[data] = value;
    
    // Overwrite the old array with the modified one.
    SetArrayArray(MM_HANDLE_FROM_ID(moduleID), MODULE_DATA, moduledata[0], sizeof(moduledata));
}

/**
 * Module data writer that writes a specified string data value.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param moduleID  The unique ID given to the registered module.
 * @param data      Data to write new string to.  See enum ModuleData.
 * @param maxlen    The max length of the data value.  See enum ModuleData.
 * @param value     A string to write as the new data value.
 */
stock ModuleMgr_WriteString(moduleID, ModuleData:data, maxlen, const String:value[])
{
    // Read all the module data.
    new moduledata[ModuleData];
    ModuleMgr_ReadAll(moduleID, moduledata);
    
    // Change the value of the specified module data.
    strcopy(String:moduledata[data], maxlen, value);
    
    // Overwrite the old array with the modified one.
    SetArrayArray(MM_HANDLE_FROM_ID(moduleID), MODULE_DATA, moduledata[0], sizeof(moduledata));
}

/**
 * Returns a module's handle given it's module ID.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param moduleID  The module ID to get the handle for.
 * 
 * @return          The array handle to the module.
 */ 
stock Handle:ModuleMgr_GetModuleArray(moduleID)
{
    // Return the module's array handle.
    return MM_HANDLE_FROM_ID(moduleID);
}

/**
 * Returns a module's ID given it's array handle.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param adtModule The module's array handle to get he module ID for.
 * 
 * @return          The module's module ID, -1 if no module matched the handle.
 */
stock ModuleMgr_GetModuleID(Handle:adtModule)
{
    // Loop through all the modules.
    // x = module ID
    for (new moduleID = 0; moduleID < MODULE_COUNT; moduleID++)
    {
        // If the handle's match, then return the current index, which is the module ID.
        if (adtModule == MM_HANDLE_FROM_ID(moduleID))
            return moduleID;
    }
    
    return -1;
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

/**
 * Command callback: project_module_info
 * Print all module data for a specific module, or all modules.
 * 
 * @param client    The client index.  Or SERVER_INDEX if coming from the server.
 * @param argc      The number of arguments that the client sent with the command.
 */
#if defined TRANSLATIONS_MANAGER
public Action:Command_ModuleInfo(client, argc)
{
    // This will send a message to a client's console.
    new MsgTypes:msgtype = MsgType_Console;
    
    // If the caller is the server, then change some translation vars.
    if (client == SERVER_INDEX)
        msgtype = MsgType_Server;
    
    decl String:partial[16];
    GetCmdArg(1, partial, sizeof(partial));
    
    decl String:divider[48] =       "***********************************************";
    decl String:subdivider[48] =    "-----------------------------------------------";
    
    decl String:modulefullname[32];
    decl String:moduleshortname[16];
    decl String:moduledesc[256];
    new bool:bModuleDisabled;
    decl String:strModuleEnabled[8];
    
    TransMgr_PrintText(client, MsgFormat_None, msgtype, _, true, divider);
    TransMgr_PrintText(client, MsgFormat_None, msgtype, _, false, "ModuleMgr cmd moduleinfo plugin name", PROJECT_FULLNAME);
    
    // Loop through all modules.
    for (new moduleID = 0; moduleID < MODULE_COUNT; moduleID++)
    {
        // Get the module's short name to match to the given partial name.
        ModuleMgr_ReadString(moduleID, ModuleData_ShortName, moduleshortname, sizeof(moduleshortname));
        
        // If the module didn't fit the given partial name, then skip to the next module.
        if (StrContains(moduleshortname, partial, false) == -1)
            continue;
        
        TransMgr_PrintText(client, MsgFormat_None, msgtype, _, true, subdivider);
        
        ModuleMgr_ReadString(moduleID, ModuleData_FullName, modulefullname, sizeof(modulefullname));
        ModuleMgr_ReadString(moduleID, ModuleData_Description, moduledesc, sizeof(moduledesc));
        bModuleDisabled = bool:ModuleMgr_ReadCell(moduleID, ModuleData_Disabled);
        
        // Translate the opposite value of bModuleDisabled into a language "yes/no" string.
        TransMgr_TranslateBool(client, !bModuleDisabled, BoolPhrase_YesNo, strModuleEnabled, sizeof(strModuleEnabled));
        
        TransMgr_PrintText(client, MsgFormat_None, msgtype, _, false, "ModuleMgr cmd moduleinfo module fullname", modulefullname);
        TransMgr_PrintText(client, MsgFormat_None, msgtype, _, false, "ModuleMgr cmd moduleinfo module shortname", moduleshortname);
        TransMgr_PrintText(client, MsgFormat_None, msgtype, _, false, "ModuleMgr cmd moduleinfo module desc", moduledesc);
        TransMgr_PrintText(client, MsgFormat_None, msgtype, _, false, "ModuleMgr cmd moduleinfo module enabled", strModuleEnabled);
        
        // Forward event to other project base files.
        #if defined EVENT_MANAGER
            EventMgr_OnCommandModuleInfo(client, moduleID, msgtype);
        #endif
        
        #if defined TRANSLATIONS_MANAGER
            TransMgr_OnCommandModuleInfo(client, moduleID, msgtype);
        #endif
        
        #if defined LOG_MANAGER
            LogMgr_OnCommandModuleInfo(client, moduleID, msgtype);
        #endif
    }
    
    TransMgr_PrintText(client, MsgFormat_None, msgtype, _, true, divider);
}
#else
// How can we print the module information without translations to read from?
public Action:Command_ModuleInfo(client, argc)
{
    ReplyToCommand(client, "Translations manager must be compiled into this project to use this command.");
}
#endif
