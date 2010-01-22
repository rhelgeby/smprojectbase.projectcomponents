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
    PushArrayArray(adtModule, bProjectEvents);
    
    // shit... any ideas?
    // didn't get this warning until i coded so much around this structure..
    
    // Push the ProjectEvents enum as the "Function" type to the module's data array.
    new Function:funcProjectEvents[ProjectEvents];  // All values default to INVALID_FUNCTION.
    PushArrayArray(adtModule, funcProjectEvents);
}

// **********************************************
//                Public API
// **********************************************

stock EventManager_RegisterEvent(moduleID, ProjectEvents:event, Function:funcid)
{
    // Get the module data array handle from the given module ID.
    new Handle:adtModule = ModuleManager_GetModuleArray(moduleID);
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
    GetArrayArray(ModuleManager_GetModuleArray(moduleID), EVENT_DATA_ENABLE, eventenabledata);
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
    GetArrayArray(ModuleManager_GetModuleArrayHandle(moduleID), EVENT_DATA_ENABLE, eventenabledata);
    
    // Return event's enable value.
    return eventenabledata[event];
}

/**
 * Event data reader that returns all event function data.
 * 
 * @param moduleID  The unique ID given to the registered module.
 */
stock EventManager_ReadAllFuncData(moduleID, Function:eventfunctiondata[ProjectEvents])
{
    GetArrayArray(ModuleManager_GetModuleArrayHandle(moduleID), EVENT_DATA_FUNCTION, eventfunctiondata);
}

/**
 * Event data reader that returns a single event function value.
 * 
 * @param moduleID  The unique ID given to the registered module.
 * @param event     An event from enum ProjectEvents. (top of file)
 * 
 * @return          The function ID of the event forward in the module.
 */
stock Function:EventManager_ReadFunctionValue(moduleID, ProjectEvents:event)
{
    new Function:eventfunctiondata[ProjectEvents];
    GetArrayArray(ModuleManager_GetModuleArrayHandle(moduleID), EVENT_DATA_FUNCTION, eventfunctiondata);
    
    // Return event's enable value.
    return eventfunctiondata[event];
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
    // send event to all modules...
    new Handle:hModuleList = ModuleManager_GetList();
    
    // Loop through all the modules.
    // x = module ID.
    for (new x = 0; x < MODULE_COUNT; x++)
    {
    }
}