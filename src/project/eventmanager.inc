/*
 * ============================================================================
 *
 *  Project
 *
 *  File:          eventmanager.inc
 *  Type:          Base
 *  Description:   Manages project events.
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
 * Provides the plugin a way to know if the event manager is included in the project.
 */
#define EVENT_MANAGER

// ---------------
//     Public
// ---------------

/**
 * This is a collection of ALL game and custom events.
 */
enum ProjectEvents
{
    Event_RoundStart,
    Event_RoundFreezeEnd,
    Event_RoundEnd,
    Event_PlayerSpawn,
    Event_PlayerHurt,
    Event_PlayerDeath,
    Event_PlayerJump,
    Event_PlayerFire
}

/**
 * String array of display names for project events. (above)
 * Keep this updated as ProjectEvents is modified.
 */
new const String:g_ProjectEventDisplay[ProjectEvents][] = { "Round Start",
                                                            "Round Freeze End",
                                                            "Round End",
                                                            "Player Spawn",
                                                            "Player Hurt",
                                                            "Player Death",
                                                            "Player Jump",
                                                            "Player Fire" };

// ---------------
//     Private
// ---------------

/**
 * The max number of cells needed for each of the event manager's allocated indexes.
 */
#define EM_DATA_MAX_CELLS EVENT_DATA_CELL_COUNT
// The number of max cells needed is equal to the number of cells needed to store all data for enum ProjectEvents.

/**
 * Defines the block of data in the module data arrays that contains event enable data.
 */
#define EVENT_DATA_ENABLE g_iEMAllocatedIndexes[0]

/**
 * Defines the block of data in the module data arrays that contains event function data.
 */
#define EVENT_DATA_FUNCTION g_iEMAllocatedIndexes[1]

/**
 * The number of cells used to store all the event enable data in enum ProjectEvents.
 */
#define EVENT_DATA_CELL_COUNT 8

/**
 * Array to store the indexes of the allocated space in the module data arrays for the event manager.
 */
new g_iEMAllocatedIndexes[2];

// **********************************************
//                 Forwards
// **********************************************

/**
 * Plugin has loaded.
 * TODO: Support multiple games.
 */
EventMgr_OnPluginStart()
{
    // Hook events to manage and forward to modules.
    HookEvent("round_start", GameEvent_RoundStart);
    //HookEvent("round_freeze_end", GameEvent_RoundFreezeEnd);
    //HookEvent("round_end", GameEvent_RoundEnd);
    //HookEvent("player_spawn", GameEvent_PlayerSpawn);
    //HookEvent("player_hurt", GameEvent_PlayerHurt);
    //HookEvent("player_death", GameEvent_PlayerDeath);
    //HookEvent("player_jump", GameEvent_PlayerJump);
    //HookEvent("weapon_fire", GameEvent_WeaponFire);
    
    // Allocate 2 indexes for the data we want to store for each module.
    ModuleMgr_Allocate(2, g_iEMAllocatedIndexes);
}

/**
 * A module was just registered.  This is being called before the module has been loaded.
 * 
 * @param adtModule The adt array of the module being registered.
 */
EventMgr_OnModuleRegister(Handle:adtModule)
{
    // Push the ProjectEvents enum as the "bool" type to the module's data array.
    // This is being pushed into our allocated space for event enable data.
    new bool:bProjectEvents[ProjectEvents];         // All values default to false.
    PushArrayArray(adtModule, bProjectEvents[0], sizeof(bProjectEvents));
    
    // Push the ProjectEvents enum as the "Function" type to the module's data array.
    // This is being pushed into our allocated space for event function data.
    new Function:funcProjectEvents[ProjectEvents];  // All values default to INVALID_FUNCTION.
    PushArrayArray(adtModule, funcProjectEvents[0], sizeof(funcProjectEvents));
}

/**
 * Command project_module_info is being called.
 * Print the module data allocated by the event manager.
 * Note: |stock| tag will stop this function from being compiled if the project_module_info command
 * isn't working due to required base files not being included.
 * 
 * @param client    The client index the text is being printed to.
 * @param moduleID  The module to print info for.
 * @param msgtype   The type of message to tell Translations Manager to print.
 */
stock EventMgr_OnCommandModuleInfo(client, moduleID, MsgTypes:msgtype)
{
    decl String:registeredevents[512];
    registeredevents[0] = 0;
    
    // Get all event enable data to print about the module.
    new bool:eventenabledata[ProjectEvents];
    EventMgr_ReadAllEnableData(moduleID, eventenabledata);
    
    new ProjectEvents:event;
    for (new eindex = 0; eindex < sizeof(eventenabledata); eindex++)
    {
        // Read the int as a ProjectEvent data type.
        event = ProjectEvents:eindex;
        
        // If the event isn't enabled, then skip the next event.
        if (!EventMgr_ReadEnableValue(moduleID, event))
            continue;
        
        // Format each event onto a string.
        if (eindex == 0)
            strcopy(registeredevents, sizeof(registeredevents), g_ProjectEventDisplay[event]);
        else
            Format(registeredevents, sizeof(registeredevents), "%s, %s", registeredevents, g_ProjectEventDisplay[event]);
    }
    
    // If the module has no registered events, then format in the "_None" phrase.
    if (registeredevents[0] == 0)
        Format(registeredevents, sizeof(registeredevents), "%t", "_None");
    
    // Print the module event info.
    TransMgr_PrintText(client, MsgFormat_None, msgtype, _, false, "EventMgr cmd moduleinfo events registered", registeredevents);
}

// **********************************************
//                Public API
// **********************************************

/**
 * Registers an event to be forwarded to the module.
 * 
 * @param moduleID      The module to forward event to.
 * @param event         The event to forward to the module.
 * @param functionname  The name of the function to forward the event to. (The function needs to be public)
 * 
 * return               The function ID to the event forward.  INVALID_FUNCTION is returned upon failure.
 */
stock Function:EventMgr_RegisterEvent(moduleID, ProjectEvents:event, const String:functionname[])
{
    // Find the function ID for the given function name.
    new Function:func = GetFunctionByName(GetMyHandle(), functionname);
    
    // If the function ID isn't INVALID_FUNCTION then the function ID was found successfully.
    if (func != INVALID_FUNCTION)
    {
        // Enable the event to be forwarded to the module, and set the function ID to the event forward.
        EventMgr_WriteEnableValue(moduleID, event, true);
        EventMgr_WriteFuncValue(moduleID, event, func);
    }
    
    // Return the function ID.
    return func;
}

/**
 * Disables an event in a module.
 * If the event is already disabled, nothing will change.
 * 
 * @param moduleID  The module ID of the module whose event is being disabled.
 * @param event     The event to disable in the module.
 */
stock EventMgr_Disable(moduleID, ProjectEvents:event)
{
    // Disable the event in the module.
    EventMgr_WriteEnableValue(moduleID, event, false);
}

/**
 * Enables an event in a module.
 * If the event is already enabled, nothing will change.
 * 
 * @param moduleID  The module ID of the module whose event is being enabled.
 * @param event     The event to enable in the module.
 */
stock EventMgr_Enable(moduleID, ProjectEvents:event)
{
    // Enable the event in the module.
    EventMgr_WriteEnableValue(moduleID, event, true);
}

/**
 * Sets a new event forward function for the given event.
 * 
 * @param moduleID      The module ID of the module whose event forward is being set.
 * @param event         The event to set the event forward on.
 * @param functionname  The name of the function to forward the event to. (The function needs to be public)
 * 
 * return               The function ID to the event forward.  INVALID_FUNCTION is returned upon failure.
 */
stock Function:EventMgr_SetEventForward(moduleID, ProjectEvents:event, const String:functionname[])
{
    // Find the function ID for the given function name.
    new Function:func = GetFunctionByName(GetMyHandle(), functionname);
    
    // If the function ID isn't INVALID_FUNCTION then the function ID was found successfully.
    if (func != INVALID_FUNCTION)
    {
        // Enable the event in the module.
        EventMgr_WriteFuncValue(moduleID, event, func);
    }
    
    // Return the function ID.
    return func;
}

// **********************************************
//   Private API (For base project files only)
// **********************************************

/**
 * Event data reader that returns all event enable data.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param moduleID  The unique ID given to the registered module.
 */
stock EventMgr_ReadAllEnableData(moduleID, bool:eventenabledata[ProjectEvents])
{
    GetArrayArray(ModuleMgr_GetModuleArray(moduleID), EVENT_DATA_ENABLE, eventenabledata[0], sizeof(eventenabledata));
}

/**
 * Sync all event enable data for all events with the given value.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param moduleID  The unique ID given to the registered module.
 * @param enable    True to enable all events, false to disable all.
 */
stock EventMgr_SyncEnableData(moduleID, bool:enable)
{
    // Read all the event function data.
    new Function:eventenabledata[ProjectEvents];
    EventMgr_ReadAllEnableData(moduleID, eventenabledata);
    
    new ProjectEvents:event;
    
    // Loop through all events.
    // eindex = Index of the current event (see enum ProjectEvents)
    for (new eindex = 0; eindex < sizeof(eventenabledata); eindex++)
    {
        // Read the int as a ProjectEvent data type.
        event = ProjectEvents:eindex;
        
        // Sync event enable value with the given value.
        eventenabledata[event] = enable;
    }
    
    // Overwrite the old array with the modified one.
    SetArrayArray(MM_HANDLE_FROM_ID(moduleID), EVENT_DATA_ENABLE, eventenabledata[0], sizeof(eventenabledata));
}

/**
 * Event data reader that returns a single event enable value.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param moduleID  The unique ID given to the registered module.
 * @param event     An event from enum ProjectEvents. (top of file)
 * 
 * @return          True if the event is enabled in the module, false if not.
 */
stock bool:EventMgr_ReadEnableValue(moduleID, ProjectEvents:event)
{
    new bool:eventenabledata[ProjectEvents];
    GetArrayArray(ModuleMgr_GetModuleArray(moduleID), EVENT_DATA_ENABLE, eventenabledata[0], sizeof(eventenabledata));
    
    // Return event's enable value.
    return eventenabledata[event];
}

/**
 * Module data writer that writes a new enable value for an event.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param moduleID  The unique ID given to the registered module.
 * @param event     The event to write new function ID to.
 * @param enable    The new enable value of the module's event.
 */
stock EventMgr_WriteEnableValue(moduleID, ProjectEvents:event, bool:enable)
{
    // Read all the module data.
    new bool:eventenabledata[ProjectEvents];
    EventMgr_ReadAllEnableData(moduleID, eventenabledata);
    
    // Write the new function ID.
    eventenabledata[event] = enable;
    
    // Overwrite the old array with the modified one.
    SetArrayArray(ModuleMgr_GetModuleArray(moduleID), EVENT_DATA_ENABLE, eventenabledata[0], sizeof(eventenabledata));
}

/**
 * Event data reader that returns all event function data.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param moduleID  The unique ID given to the registered module.
 */
stock EventMgr_ReadAllFuncData(moduleID, Function:eventfuncdata[ProjectEvents])
{
    GetArrayArray(ModuleMgr_GetModuleArray(moduleID), EVENT_DATA_FUNCTION, eventfuncdata[0], sizeof(eventfuncdata));
}

/**
 * Reset's all event function data for all events.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param moduleID  The unique ID given to the registered module.
 */
stock EventMgr_ResetFuncData(moduleID)
{
    // Read all the event function data.
    new Function:eventfuncdata[ProjectEvents];
    EventMgr_ReadAllFuncData(moduleID, eventfuncdata);
    
    new ProjectEvents:event;
    
    // Loop through all events.
    // x = Index of the current event (see enum ProjectEvents)
    for (new eindex = 0; eindex < sizeof(eventfuncdata); eindex++)
    {
        // Read the int as a ProjectEvent data type.
        event = ProjectEvents:eindex;
        
        // Reset the function ID to invalid.
        eventfuncdata[event] = INVALID_FUNCTION;
    }
    
    // Overwrite the old array with the modified one.
    SetArrayArray(MM_HANDLE_FROM_ID(moduleID), EVENT_DATA_FUNCTION, eventfuncdata[0], sizeof(eventfuncdata));
}

/**
 * Event data reader that returns a single event function value.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param moduleID  The unique ID given to the registered module.
 * @param event     An event from enum ProjectEvents. (top of file)
 * 
 * @return          The function ID of the event forward in the module.
 */
stock Function:EventMgr_ReadFuncValue(moduleID, ProjectEvents:event)
{
    new Function:eventfuncdata[ProjectEvents];
    EventMgr_ReadAllFuncData(moduleID, eventfuncdata);
    
    // Return event's enable value.
    return eventfuncdata[event];
}

/**
 * Module data writer that writes a new function ID for an event.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param moduleID  The unique ID given to the registered module.
 * @param event     The event to write new function ID to.
 * @param func      The new function ID to write as the event's forward function.
 */
stock EventMgr_WriteFuncValue(moduleID, ProjectEvents:event, Function:func)
{
    // Read all the module data.
    new Function:eventfuncdata[ProjectEvents];
    EventMgr_ReadAllFuncData(moduleID, eventfuncdata);
    
    // Write the new function ID.
    eventfuncdata[event] = func;
    
    // Overwrite the old array with the modified one.
    SetArrayArray(ModuleMgr_GetModuleArray(moduleID), EVENT_DATA_FUNCTION, eventfuncdata[0], sizeof(eventfuncdata));
}

// **********************************************
//             Game Event Callbacks
// **********************************************

/**
 * Round has started.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_RoundStart(Handle:event, const String:name[], bool:dontBroadcast)
{
    new Function:func;
    
    // Loop through all the modules.
    for (new moduleID = 0; moduleID < MODULE_COUNT; moduleID++)
    {
        // If the module is disabled, then stop this iteration.
        if (ModuleMgr_IsDisabled(moduleID))
            continue;
        
        // Check if this event is enabled for this module.
        if (EventMgr_ReadEnableValue(moduleID, Event_RoundStart))
        {
            // Read the function ID.
            func = EventMgr_ReadFuncValue(moduleID, Event_RoundStart);
            
            // If the function ID is invalid, then stop this iteration.
            if (func == INVALID_FUNCTION)
            {
                LogError("NO ROUND START EVENT FORWARD");
                continue;
            }
            
            // Call the event forward.
            Call_StartFunction(GetMyHandle(), func);
            Call_Finish();
        }
    }
}
