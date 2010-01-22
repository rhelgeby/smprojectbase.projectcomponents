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

/**
 * Defines the block of data in the module data arrays that contains module-defined data.
 */
#define MODULE_DATA 0

/**
 * Define the block size of the modules' data array.  Only 1 block is used, and it's for an array.
 */
#define MM_DATA_MAX_LENGTH 64

/**
 * This is a macro that turns a module ID into the handle to it's data array.
 */
#define MM_HANDLE_FROM_ID(%1) GetArrayCell(g_adtModuleList, %1)

/**
 * Base array that contains all module data array handles.
 */
new Handle:g_adtModuleList;

/**
 * All the data modules need to register.
 * See ModuleManager_Register()
 */
enum ModuleData
{
    bool:ModuleData_Disabled = 0,   /** True if the module is disabled. */
    String:ModuleData_FullName,     /** Probably going to be used when displaying modules. */
    String:ModuleData_ShortName,    /** Probably going to be used for client interaction. */
    // gunna need more than this..
}

// **********************************************
//                 Forwards
// **********************************************

/**
 * Plugin has loaded.
 */
public ModuleManager_OnPluginStart()
{
    // Create the adt array used to store module data array handles.
    if (g_adtModuleList == INVALID_HANDLE)
        g_adtModuleList = CreateArray();
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
stock ModuleManager_Register(moduledata[ModuleData])
{
    // This is the array that will hold all the actual module data.
    new Handle:adtModule = CreateArray(MM_DATA_MAX_LENGTH);
    
    // Push all the given module data to the new array.
    PushArrayArray(adtModule, moduledata);
    
    // Store the handle in the global module list.
    new moduleID = PushArrayCell(g_adtModuleList, adtModule);
    
    // Forward event to other base project files.
    EventManager_OnModuleRegister(adtModule);
    
    // Return the module ID.
    return moduleID;
}

/**
 * Module data reader that returns all available module data.
 * Modules can use this for communicating with other modules.
 * 
 * @param moduleID  The unique ID given to the registered module.
 */
stock ModuleManager_ReadAll(moduleID, moduledata[ModuleData])
{
    GetArrayArray(MM_HANDLE_FROM_ID(moduleID), MODULE_DATA, moduledata);
}

/**
 * Module data reader for any data type except strings.
 * Modules can use this for communicating with other modules.
 * 
 * @param moduleID  The unique ID given to the registered module.
 * 
 * @return          The integer value of the desired module data.
 */
stock ModuleManager_ReadCell(moduleID, ModuleData:data)
{
    new moduledata[ModuleData];
    GetArrayArray(MM_HANDLE_FROM_ID(moduleID), MODULE_DATA, moduledata);
    
    // Return the value.
    return _:moduledata[data];
}

/**
 * Module data reader for any string typed values.
 * Modules can use this for communicating with other modules. 
 * 
 * @param moduleID  The unique ID given to the registered module.
 * @param fullname  Output variable for the full name the module gave upon registration.
 * @param maxlen    The max length of the output string.
 */
stock ModuleManager_ReadString(moduleID, String:fullname, maxlen)
{
    new moduledata[ModuleData];
    GetArrayArray(MM_HANDLE_FROM_ID(moduleID), MODULE_DATA, moduledata);
    
    // Copy full name to output
    strcopy(fullname, maxlen, moduledata[ModuleData_FullName]);
}

// **********************************************
//   Private API (For base project files only)
// **********************************************

/**
 * Returns a module's handle given it's module ID.
 * This is meant for other base project files to use, modules shouln't need this. 
 * 
 * @param moduleID  The module ID to get the handle for.
 * 
 * @return          The array handle to the module.
 */ 
stock Handle:ModuleManager_GetModuleArray(moduleID)
{
    // Return the module's array handle.
    return MM_HANDLE_FROM_ID(moduleID);
}

/**
 * Returns a module's ID given it's array handle.
 * This is meant for other base project files to use, modules shouln't need this. 
 * 
 * @param adtModule The module's array handle to get he module ID for.
 * 
 * @return          The module's module ID, -1 if no module matched the handle.
 */
stock ModuleManager_GetModuleID(Handle:adtModule)
{
    // Loop through all the modules.
    // x = module ID
    for (new x = 0; x < MODULE_COUNT; x++)
    {
        // If the handle's match, then return the current index, which is the module ID.
        if (adtModule == MM_HANDLE_FROM_ID(x))
            return x;
    }
    
    return -1;
}

/**
 * Allows other modules to get the handle of the main array to manipulate data.
 * This is meant for other base project files to use, modules shouln't need this.
 */
stock Handle:ModuleManager_GetList()
{
    return g_adtModuleList;
}