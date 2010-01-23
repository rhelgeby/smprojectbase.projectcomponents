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
 * Defines the block of data in the module data arrays that contains event enable data.
 */
#define EVENT_DATA_ENABLE 1

/**
 * Defines the block of data in the module data arrays that contains event function data.
 */
#define EVENT_DATA_FUNCTION 2

/**
 * This is a collection of ALL game and event custom events.
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

// **********************************************
//                 Forwards
// **********************************************

/**
 * Plugin has loaded.
 * TODO: Support multiple games.
 */
public EventManager_OnPluginStart()
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
}

/**
 * A module was just registered.  This is being called before the module has been loaded.
 * 
 * @param adtModule The adt array of the module being registered.
 */
public EventManager_OnModuleRegister(Handle:adtModule)
{
    // Push the ProjectEvents enum as the "bool" type to the module's data array.
    new bool:bProjectEvents[ProjectEvents];         // All values default to false.
    PushArrayArray(adtModule, bProjectEvents[0], sizeof(bProjectEvents));
    
    // Push the ProjectEvents enum as the "Function" type to the module's data array.
    new Function:funcProjectEvents[ProjectEvents];  // All values default to INVALID_FUNCTION.
    PushArrayArray(adtModule, funcProjectEvents[0], sizeof(funcProjectEvents));
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
stock Function:EventManager_RegisterEvent(moduleID, ProjectEvents:event, const String:functionname[])
{
    // Find the function ID for the given function name.
    new Function:func = GetFunctionByName(GetMyHandle(), functionname);
    
    // If the function ID isn't INVALID_FUNCTION then the function ID was found successfully.
    if (func != INVALID_FUNCTION)
    {
        // Enable the event to be forwarded to the module, and set the function ID to the event forward.
        EventManager_WriteEnableValue(moduleID, event, true);
        EventManager_WriteFuncValue(moduleID, event, func);
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
stock EventManager_Disable(moduleID, ProjectEvents:event)
{
    // Disable the event in the module.
    EventManager_WriteEnableValue(moduleID, event, false);
}

/**
 * Enables an event in a module.
 * If the event is already enabled, nothing will change.
 * 
 * @param moduleID  The module ID of the module whose event is being enabled.
 * @param event     The event to enable in the module.
 */
stock EventManager_Enable(moduleID, ProjectEvents:event)
{
    // Enable the event in the module.
    EventManager_WriteEnableValue(moduleID, event, true);
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
stock Function:EventManager_SetEventForward(moduleID, ProjectEvents:event, const String:functionname[])
{
    // Find the function ID for the given function name.
    new Function:func = GetFunctionByName(GetMyHandle(), functionname);
    
    // If the function ID isn't INVALID_FUNCTION then the function ID was found successfully.
    if (func != INVALID_FUNCTION)
    {
        // Enable the event in the module.
        EventManager_WriteFuncValue(moduleID, event, func);
    }
    
    // Return the function ID.
    return func;
}

// **********************************************
//   Private API (For base project files only)
// **********************************************

/**
 * Event data reader that returns all event enable data.
 * 
 * @param moduleID  The unique ID given to the registered module.
 */
stock EventManager_ReadAllEnableData(moduleID, bool:eventenabledata[ProjectEvents])
{
    GetArrayArray(ModuleManager_GetModuleArray(moduleID), EVENT_DATA_ENABLE, eventenabledata[0], sizeof(eventenabledata));
}

/**
 * Sync all event enable data for all events with the given value.
 * 
 * @param moduleID  The unique ID given to the registered module.
 * @param enable    True to enable all events, false to disable all.
 */
stock EventManager_SyncEnableData(moduleID, bool:enable)
{
    // Read all the event function data.
    new Function:eventenabledata[ProjectEvents];
    EventManager_ReadAllEnableData(moduleID, eventenabledata);
    
    new ProjectEvents:event;
    
    // Loop through all events.
    // x = Index of the current event (see enum ProjectEvents)
    for (new x = 0; x < sizeof(eventenabledata); x++)
    {
        // Read the int as a ProjectEvent data type.
        event = ProjectEvents:x;
        
        // Sync event enable value with the given value.
        eventenabledata[event] = enable;
    }
    
    // Overwrite the old array with the modified one.
    SetArrayArray(MM_HANDLE_FROM_ID(moduleID), EVENT_DATA_ENABLE, eventenabledata[0], sizeof(eventenabledata));
}

/**
 * Event data reader that returns a single event enable value.
 * 
 * @param moduleID  The unique ID given to the registered module.
 * @param event     An event from enum ProjectEvents. (top of file)
 * 
 * @return          True if the event is enabled in the module, false if not.
 */
stock bool:EventManager_ReadEnableValue(moduleID, ProjectEvents:event)
{
    new bool:eventenabledata[ProjectEvents];
    GetArrayArray(ModuleManager_GetModuleArray(moduleID), EVENT_DATA_ENABLE, eventenabledata[0], sizeof(eventenabledata));
    
    // Return event's enable value.
    return eventenabledata[event];
}

/**
 * Module data writer that writes a new enable value for an event.
 * 
 * @param moduleID  The unique ID given to the registered module.
 * @param event     The event to write new function ID to.
 * @param enable    The new enable value of the module's event.
 */
stock EventManager_WriteEnableValue(moduleID, ProjectEvents:event, bool:enable)
{
    // Read all the module data.
    new bool:eventenabledata[ProjectEvents];
    EventManager_ReadAllEnableData(moduleID, eventenabledata);
    
    // Write the new function ID.
    eventenabledata[event] = enable;
    
    // Overwrite the old array with the modified one.
    SetArrayArray(ModuleManager_GetModuleArray(moduleID), EVENT_DATA_ENABLE, eventenabledata[0], sizeof(eventenabledata));
}

/**
 * Event data reader that returns all event function data.
 * 
 * @param moduleID  The unique ID given to the registered module.
 */
stock EventManager_ReadAllFuncData(moduleID, Function:eventfuncdata[ProjectEvents])
{
    GetArrayArray(ModuleManager_GetModuleArray(moduleID), EVENT_DATA_FUNCTION, eventfuncdata[0], sizeof(eventfuncdata));
}

/**
 * Reset's all event function data for all events.
 * 
 * @param moduleID  The unique ID given to the registered module.
 */
stock EventManager_ResetFuncData(moduleID)
{
    // Read all the event function data.
    new Function:eventfuncdata[ProjectEvents];
    EventManager_ReadAllFuncData(moduleID, eventfuncdata);
    
    new ProjectEvents:event;
    
    // Loop through all events.
    // x = Index of the current event (see enum ProjectEvents)
    for (new x = 0; x < sizeof(eventfuncdata); x++)
    {
        // Read the int as a ProjectEvent data type.
        event = ProjectEvents:x;
        
        // Reset the function ID to invalid.
        eventfuncdata[event] = INVALID_FUNCTION;
    }
    
    // Overwrite the old array with the modified one.
    SetArrayArray(MM_HANDLE_FROM_ID(moduleID), EVENT_DATA_FUNCTION, eventfuncdata[0], sizeof(eventfuncdata));
}

/**
 * Event data reader that returns a single event function value.
 * 
 * @param moduleID  The unique ID given to the registered module.
 * @param event     An event from enum ProjectEvents. (top of file)
 * 
 * @return          The function ID of the event forward in the module.
 */
stock Function:EventManager_ReadFuncValue(moduleID, ProjectEvents:event)
{
    new Function:eventfuncdata[ProjectEvents];
    EventManager_ReadAllFuncData(moduleID, eventfuncdata);
    
    // Return event's enable value.
    return eventfuncdata[event];
}

/**
 * Module data writer that writes a new function ID for an event.
 * 
 * @param moduleID  The unique ID given to the registered module.
 * @param event     The event to write new function ID to.
 * @param func      The new function ID to write as the event's forward function.
 */
stock EventManager_WriteFuncValue(moduleID, ProjectEvents:event, Function:func)
{
    // Read all the module data.
    new Function:eventfuncdata[ProjectEvents];
    EventManager_ReadAllFuncData(moduleID, eventfuncdata);
    
    // Write the new function ID.
    eventfuncdata[event] = func;
    
    // Overwrite the old array with the modified one.
    SetArrayArray(MM_HANDLE_FROM_ID(moduleID), EVENT_DATA_FUNCTION, eventfuncdata[0], sizeof(eventfuncdata));
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
    // x = module ID.
    for (new x = 0; x < MODULE_COUNT; x++)
    {
        // If the module is disabled, then stop this iteration.
        if (ModuleManager_IsDisabled(x))
            continue;
        
        // Check if this event is enabled for x module.
        if (EventManager_ReadEnableValue(x, Event_RoundStart))
        {
            // Read the function ID.
            func = EventManager_ReadFuncValue(x, Event_RoundStart);
            
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
