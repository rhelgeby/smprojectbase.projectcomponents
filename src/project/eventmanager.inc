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

// See project.inc

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
    HookEvent("round_freeze_end", GameEvent_RoundFreezeEnd);
    HookEvent("round_end", GameEvent_RoundEnd);
    HookEvent("player_spawn", GameEvent_PlayerSpawn);
    HookEvent("player_hurt", GameEvent_PlayerHurt);
    HookEvent("player_death", GameEvent_PlayerDeath);
    HookEvent("player_jump", GameEvent_PlayerJump);
    HookEvent("weapon_fire", GameEvent_WeaponFire);
    
    // Allocate 2 indexes for the data we want to store for each module.
    ModuleMgr_Allocate(2, g_iEMAllocatedIndexes);
}

/**
 * A module was just registered.  This is being called before the module has been assigned a module identifier.
 * 
 * @param adtModule The adt array of the module being registered.
 */
stock EventMgr_OnModuleRegister(Handle:adtModule)
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
 * isn't working due to required base components not being included.
 * 
 * @param client    The client index the text is being printed to.
 * @param module    The module to print info for.
 * @param msgtype   The type of message to tell Translations Manager to print.
 */
stock EventMgr_OnPrintModuleInfo(client, Module:module, MsgTypes:msgtype)
{
    decl String:registeredevents[512];
    registeredevents[0] = 0;
    
    // Get all event enable data to print about the module.
    new bool:eventenabledata[ProjectEvents];
    EventMgr_ReadAllEnableData(module, eventenabledata);
    
    new ProjectEvents:event;
    for (new eindex = 0; eindex < sizeof(eventenabledata); eindex++)
    {
        // Read the int as a ProjectEvent data type.
        event = ProjectEvents:eindex;
        
        // If the event isn't enabled, then skip the next event.
        if (!EventMgr_ReadEnableValue(module, event))
            continue;
        
        // Format each event onto a string.
        if (registeredevents[0] == 0)
            strcopy(registeredevents, sizeof(registeredevents), g_ProjectEventDisplay[event]);
        else
            Format(registeredevents, sizeof(registeredevents), "%s, %s", registeredevents, g_ProjectEventDisplay[event]);
    }
    
    // If the module has no registered events, then format in the "_None" phrase.
    if (registeredevents[0] == 0)
        Format(registeredevents, sizeof(registeredevents), "%t", "_None");
    
    // Print the module event info.
    TransMgr_PrintText(client, MsgFormat_None, msgtype, _, false, "EventMgr cmd base lvl2 modules info print", registeredevents);
}

// **********************************************
//                Public API
// **********************************************

/**
 * Registers an event to be forwarded to the module.
 * 
 * @param module        The module to forward event to.
 * @param event         The event to forward to the module.
 * @param functionname  The name of the function to forward the event to. (The function needs to be public)
 * 
 * return               The function ID to the event forward.  INVALID_FUNCTION is returned upon failure.
 */
stock Function:EventMgr_RegisterEvent(Module:module, ProjectEvents:event, const String:functionname[])
{
    // Find the function ID for the given function name.
    new Function:func = GetFunctionByName(GetMyHandle(), functionname);
    
    // If the function ID isn't INVALID_FUNCTION then the function ID was found successfully.
    if (func != INVALID_FUNCTION)
    {
        // Enable the event to be forwarded to the module, and set the function ID to the event forward.
        EventMgr_WriteEnableValue(module, event, true);
        EventMgr_WriteFuncValue(module, event, func);
    }
    
    // Return the function ID.
    return func;
}

/**
 * Disables an event in a module.
 * If the event is already disabled, nothing will change.
 * 
 * @param module    The module whose event is being disabled.
 * @param event     The event to disable in the module.
 */
stock EventMgr_Disable(Module:module, ProjectEvents:event)
{
    // Disable the event in the module.
    EventMgr_WriteEnableValue(module, event, false);
}

/**
 * Enables an event in a module.
 * If the event is already enabled, nothing will change.
 * 
 * @param module    The module whose event is being enabled.
 * @param event     The event to enable in the module.
 */
stock EventMgr_Enable(Module:module, ProjectEvents:event)
{
    // Enable the event in the module.
    EventMgr_WriteEnableValue(module, event, true);
}

/**
 * Sets a new event forward function for the given event.
 * 
 * @param module        The module whose event forward is being set.
 * @param event         The event to set the event forward on.
 * @param functionname  The name of the function to forward the event to. (The function needs to be public)
 * 
 * return               The function ID to the event forward.  INVALID_FUNCTION is returned upon failure.
 */
stock Function:EventMgr_SetEventForward(Module:module, ProjectEvents:event, const String:functionname[])
{
    // Find the function ID for the given function name.
    new Function:func = GetFunctionByName(GetMyHandle(), functionname);
    
    // If the function ID isn't INVALID_FUNCTION then the function ID was found successfully.
    if (func != INVALID_FUNCTION)
    {
        // Enable the event in the module.
        EventMgr_WriteFuncValue(module, event, func);
    }
    
    // Return the function ID.
    return func;
}

/**
 * Forwards an event and a list of data values to all modules who registered it.
 * 
 * @param event             The event to forward.  See enum ProjectEvents.
 * @param eventdata         An array with all data to forward to each module.
 * @param numValues         The number of elements in the first dimension of the eventdata array.
 * @param size              The max size of the arrays/strings if passing them through eventdata.  Otherwise set this to 1.
 * @param eventdatatypes    An array of datatypes for each event value.  See enum EventDataTypes.  This should line up with eventdata.
 * @param modules           An extra filter to narrow down which modules will see the event.  This MUST end with INVALID_MODULE.
 */
stock EventMgr_Forward(ProjectEvents:event, any:eventdata[][], numValues, size, EventDataTypes:eventdatatypes[], Module:modules[] = {INVALID_MODULE})
{
    new Function:func;
    new Module:module;
    
    // Loop through all the modules.
    for (new moduleindex = 0; moduleindex < MODULE_COUNT; moduleindex++)
    {
        // Read moduleindex as a Module type.
        module = Module:moduleindex;
        
        // If the module is disabled, then stop this iteration.
        if (ModuleMgr_IsDisabled(module))
            continue;
        
        // Check if this event is enabled for this module.
        if (!EventMgr_ReadEnableValue(module, event))
            continue;
        
        // Check if the module should see this event.
        new bool:pass_filter = bool:(modules[0] == INVALID_MODULE);
        for (new filter_module = 0; modules[filter_module] != INVALID_MODULE; filter_module++)
        {
            if (module == modules[filter_module])
                pass_filter = true;
        }
        
        if (!pass_filter)
            continue;
        
        // Read the function ID.
        func = EventMgr_ReadFuncValue(module, event);
        
        // If the function ID is invalid, then stop this iteration.
        if (func == INVALID_FUNCTION)
        {
            LogError("INVALID FORWARD");
            continue;
        }
        
        // Call the event forward.
        Call_StartFunction(GetMyHandle(), func);
        
        // Push each data value into the function parameter list.
        // Loop through each event value.
        for (new x = 0; x < numValues; x++)
        {
            switch (eventdatatypes[x])
            {
                case DataType_Cell:     Call_PushCell(_:eventdata[x][0]);
                case DataType_Float:    Call_PushFloat(Float:eventdata[x][0]);
                case DataType_Array:    Call_PushArray(eventdata[x], size);
                case DataType_String:   Call_PushString(eventdata[x]);
            }
        }
        
        // Call the function.
        Call_Finish();
    }
}

// **********************************************
//   Private API (For base project files only)
// **********************************************

/**
 * Event data reader that returns all event enable data.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param module    The module whose event enable data to read.
 */
stock EventMgr_ReadAllEnableData(Module:module, bool:eventenabledata[ProjectEvents])
{
    GetArrayArray(ModuleMgr_GetModuleArray(module), EVENT_DATA_ENABLE, eventenabledata[0], sizeof(eventenabledata));
}

/**
 * Sync all event enable data for all events with the given value.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param module    The module whose event enable data to sync.
 * @param enable    True to enable all events, false to disable all.
 */
stock EventMgr_SyncEnableData(Module:module, bool:enable)
{
    // Read all the event function data.
    new Function:eventenabledata[ProjectEvents];
    EventMgr_ReadAllEnableData(module, eventenabledata);
    
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
    SetArrayArray(MM_HANDLE_FROM_ID(module), EVENT_DATA_ENABLE, eventenabledata[0], sizeof(eventenabledata));
}

/**
 * Event data reader that returns a single event enable value.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param module    The module whose event enable data to read.
 * @param event     An event from enum ProjectEvents. (top of file)
 * 
 * @return          True if the event is enabled in the module, false if not.
 */
stock bool:EventMgr_ReadEnableValue(Module:module, ProjectEvents:event)
{
    new bool:eventenabledata[ProjectEvents];
    GetArrayArray(ModuleMgr_GetModuleArray(module), EVENT_DATA_ENABLE, eventenabledata[0], sizeof(eventenabledata));
    
    // Return event's enable value.
    return eventenabledata[event];
}

/**
 * Module data writer that writes a new enable value for an event.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param module    The module whose event enable data to write.
 * @param event     The event to write new function ID to.
 * @param enable    The new enable value of the module's event.
 */
stock EventMgr_WriteEnableValue(Module:module, ProjectEvents:event, bool:enable)
{
    // Read all the module data.
    new bool:eventenabledata[ProjectEvents];
    EventMgr_ReadAllEnableData(module, eventenabledata);
    
    // Write the new function ID.
    eventenabledata[event] = enable;
    
    // Overwrite the old array with the modified one.
    SetArrayArray(ModuleMgr_GetModuleArray(module), EVENT_DATA_ENABLE, eventenabledata[0], sizeof(eventenabledata));
}

/**
 * Event data reader that returns all event function data.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param module    The module whose event function data to read.
 */
stock EventMgr_ReadAllFuncData(Module:module, Function:eventfuncdata[ProjectEvents])
{
    GetArrayArray(ModuleMgr_GetModuleArray(module), EVENT_DATA_FUNCTION, eventfuncdata[0], sizeof(eventfuncdata));
}

/**
 * Reset's all event function data for all events.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param module    The module whose event function data to reset.
 */
stock EventMgr_ResetFuncData(Module:module)
{
    // Read all the event function data.
    new Function:eventfuncdata[ProjectEvents];
    EventMgr_ReadAllFuncData(module, eventfuncdata);
    
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
    SetArrayArray(MM_HANDLE_FROM_ID(module), EVENT_DATA_FUNCTION, eventfuncdata[0], sizeof(eventfuncdata));
}

/**
 * Event data reader that returns a single event function value.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param module    The module whose event function data to read.
 * @param event     An event from enum ProjectEvents. (top of file)
 * 
 * @return          The function ID of the event forward in the module.
 */
stock Function:EventMgr_ReadFuncValue(Module:module, ProjectEvents:event)
{
    new Function:eventfuncdata[ProjectEvents];
    EventMgr_ReadAllFuncData(module, eventfuncdata);
    
    // Return event's enable value.
    return eventfuncdata[event];
}

/**
 * Module data writer that writes a new function ID for an event.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param module    The module whose event function data to write.
 * @param event     The event to write new function ID to.
 * @param func      The new function ID to write as the event's forward function.
 */
stock EventMgr_WriteFuncValue(Module:module, ProjectEvents:event, Function:func)
{
    // Read all the module data.
    new Function:eventfuncdata[ProjectEvents];
    EventMgr_ReadAllFuncData(module, eventfuncdata);
    
    // Write the new function ID.
    eventfuncdata[event] = func;
    
    // Overwrite the old array with the modified one.
    SetArrayArray(ModuleMgr_GetModuleArray(module), EVENT_DATA_FUNCTION, eventfuncdata[0], sizeof(eventfuncdata));
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
    // Forward event to all modules.
    EventMgr_Forward(Event_RoundStart, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * Pre-round has freezetime has finished.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_RoundFreezeEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    EventMgr_Forward(Event_RoundFreezeEnd, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * Round has ended.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_RoundEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    EventMgr_Forward(Event_RoundEnd, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * Client has joined a team.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_PlayerTeam(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell, DataType_Cell, DataType_Cell};
    new any:eventdata[sizeof(eventdatatypes)][1];
    
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    eventdata[1][0] = GetEventInt(event, "team");
    eventdata[2][0] = GetEventInt(event, "oldteam");
    eventdata[3][0] = GetEventBool(event, "disconnect");
    
    EventMgr_Forward(Event_PlayerTeam, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
}

/**
 * Client has spawned.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_PlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    EventMgr_Forward(Event_PlayerSpawn, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}

/**
 * Client has been damaged.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_PlayerHurt(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell, DataType_Cell, DataType_Cell, DataType_String, DataType_Cell, DataType_Cell, DataType_Cell};
    new any:eventdata[sizeof(eventdatatypes)][32];
    
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    eventdata[1][0] = GetClientOfUserId(GetEventInt(event, "attacker"));
    eventdata[2][0] = GetEventInt(event, "health");
    eventdata[3][0] = GetEventInt(event, "armor");
    GetEventString(event, "weapon", eventdata[4], sizeof(eventdata[]));
    eventdata[5][0] = GetEventInt(event, "dmg_health");
    eventdata[6][0] = GetEventInt(event, "dmg_armor");
    eventdata[7][0] = GetEventInt(event, "hitgroup");
    
    EventMgr_Forward(Event_PlayerHurt, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
}

/**
 * Client has been killed.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_PlayerDeath(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell, DataType_String, DataType_Cell};
    new any:eventdata[sizeof(eventdatatypes)][32];
    
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    eventdata[1][0] = GetClientOfUserId(GetEventInt(event, "attacker"));
    GetEventString(event, "weapon", eventdata[2], sizeof(eventdata[]));
    eventdata[3][0] = GetEventInt(event, "headshot");
    
    EventMgr_Forward(Event_PlayerDeath, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
}

/**
 * Client has jumped.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_PlayerJump(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    
    EventMgr_Forward(Event_PlayerJump, eventdata, 1, 1, g_CommonDataType2);
}

/**
 * Client has fired a weapon.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_WeaponFire(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules. (WeaponFire)
    new EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_String};
    new any:eventdata[sizeof(eventdatatypes)][32];
    
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    GetEventString(event, "weapon", eventdata[1], sizeof(eventdata[]));
    
    EventMgr_Forward(Event_WeaponFire, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
    
    /*
    // Forward event to all modules. (WeaponEntityFire)
    new eventdata2[sizeof(eventdatatypes)][1];
    eventdatatypes[1] = DataType_Cell;
    
    decl String:classname[32];
    new weaponentity;
    
    // Loop through all entities.
    for (new entity = MaxClients; entity < GetMaxEntities(); entity++)
    {
        if (!IsValidEntity(entity))
            continue;
        
        GetEdictClassname(entity, classname, sizeof(classname));
        if (StrContains(classname, eventdata[1], false) == -1)
            continue;
        
        if (eventdata[0][0] == GetEntPropEnt(entity, Prop_Data, "m_hOwner"))
        {
            weaponentity = entity;
            break;
        }
    }
    
    eventdata2[0][0] = eventdata[0][0];
    eventdata2[1][0] = weaponentity;
    
    EventMgr_Forward(Event_WeaponEntityFire, eventdata2, sizeof(eventdata2), sizeof(eventdata2[]), eventdatatypes);
    */
}
