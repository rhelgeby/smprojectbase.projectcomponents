/*
 * ============================================================================
 *
 *  Project
 *
 *  File:          project.sp
 *  Type:          Base
 *  Description:   Base file.
 *
 *  Copyright (C) 2009-2010  Greyscale, Richard Helgeby
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

// Comment out to not require semicolons at the end of each line of code.
#pragma semicolon 1

#include <sourcemod>

// Base project includes.

#include "project/project"
#include "project/versioninfo"
#include "project/accessmanager"
#include "project/translationsmanager"
#include "project/logmanager"
#include "project/configmanager"
#include "project/eventmanager"
#include "project/modulemanager"

// Module includes

#include "project/testmodule"

/**
 * Record plugin info.
 */
public Plugin:myinfo =
{
    name = PROJECT_FULLNAME,
    author = PROJECT_AUTHOR,
    description = PROJECT_DESCRIPTION,
    version = PROJECT_VERSION,
    url = PROJECT_URL
};

/**
 * Called before plugin is loaded.
 * 
 * @param myself	Handle to the plugin.
 * @param late		Whether or not the plugin was loaded "late" (after map load).
 * @param error		Error message buffer in case load failed.
 * @param err_max	Maximum number of characters for error message buffer.
 * @return			APLRes_Success for load success, APLRes_Failure or APLRes_SilentFailure otherwise.
 */
public APLRes:AskPluginLoad2(Handle:myself, bool:late, String:error[], err_max)
{
    // Let plugin load successfully.
    return APLRes_Success;
}

/**
 * Plugin is loading.
 */
public OnPluginStart()
{
    // Forward event to other project base components.
    ModuleMgr_OnPluginStart();
    
    #if defined EVENT_MANAGER
        EventMgr_OnPluginStart();
    #endif
    
    #if defined CONFIG_MANAGER
        ConfigMgr_OnPluginStart();
    #endif
    
    #if defined TRANSLATIONS_MANAGER
        TransMgr_OnPluginStart();
    #endif
    
    #if defined LOG_MANAGER
        LogMgr_OnPluginStart();
    #endif
    
    #if defined ACCESS_MANAGER
        AccessMgr_OnPluginStart();
    #endif
    
    #if defined VERSION_INFO
        VersionInfo_OnPluginStart();
    #endif
    
    // Forward the OnPluginStart event to all modules.
    ForwardOnPluginStart();
    
    // All modules should be registered by this point!
    
    // Forward event to other project base components.
    #if defined TRANSLATIONS_MANAGER
        TransMgr_OnModulesRegistered();
    #endif
}

/**
 * All plugins have finished loading.
 */
public OnAllPluginsLoaded()
{
    #if defined EVENT_MANAGER
        // Forward event to all modules.
        EventMgr_Forward(Event_OnAllPluginsLoaded, g_CommonEventData1, 0, 0, g_CommonDataType1);
    #endif
}

/**
 * The map is starting.
 */
public OnMapStart()
{
    #if defined EVENT_MANAGER
        // Forward event to all modules.
        EventMgr_Forward(Event_OnMapStart, g_CommonEventData1, 0, 0, g_CommonDataType1);
    #endif
}

/**
 * The map is ending.
 */
public OnMapEnd()
{
    #if defined EVENT_MANAGER
        // Forward event to all modules.
        EventMgr_Forward(Event_OnMapEnd, g_CommonEventData1, 0, 0, g_CommonDataType1);
    #endif
}

/**
 * Main configs were just executed.
 */
public OnAutoConfigsBuffered()
{
    #if defined EVENT_MANAGER
        // Forward event to all modules.
        EventMgr_Forward(Event_OnAutoConfigsBuffered, g_CommonEventData1, 0, 0, g_CommonDataType1);
    #endif
}

/**
 * Configs just finished getting executed.
 */
public OnConfigsExecuted()
{
    #if defined EVENT_MANAGER
        // Forward event to all modules.
        EventMgr_Forward(Event_OnConfigsExecuted, g_CommonEventData1, 0, 0, g_CommonDataType1);
    #endif
}

/**
 * Client is joining the server.
 * 
 * @param client    The client index.
 */
public OnClientPutInServer(client)
{
    #if defined EVENT_MANAGER
        // Forward event to all modules.
        new any:eventdata[1][1];
        
        eventdata[0][0] = client;
        
        EventMgr_Forward(Event_OnClientPutInServer, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
    #endif
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
public OnClientDisconnect(client)
{
    #if defined EVENT_MANAGER
        // Forward event to all modules.
        new any:eventdata[1][1];
        
        eventdata[0][0] = client;
        
        EventMgr_Forward(Event_OnClientDisconnect, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
    #endif
}

// **********************************************
//           Project Base Utilities
// **********************************************

/**
 * Wrapper of CreateConVar to prefix the cvars with the project's shortname.
 * 
 * See native CreateConVar.
 */
stock Handle:Project_CreateConVar(const String:name[], const String:defaultValue[], const String:description[]="", flags=0, bool:hasMin=false, Float:min=0.0, bool:hasMax=false, Float:max=0.0)
{
    decl String:project_cvarname[64];
    Format(project_cvarname, sizeof(project_cvarname), "%s_%s", PROJECT_CVAR_PREFIX, name);
    
    return CreateConVar(project_cvarname, defaultValue, description, flags, hasMin, min, hasMax, max);
}

/**
 * Wrapper of RegServerCmd to prefix the commands with the project's shortname.
 * 
 * See native RegServerCmd.
 */
stock Project_RegServerCmd(const String:cmd[], SrvCmd:callback, const String:description[]="", flags=0)
{
    decl String:project_cmdname[64];
    Format(project_cmdname, sizeof(project_cmdname), "%s_%s", PROJECT_CMD_PREFIX, cmd);
    
    RegServerCmd(project_cmdname, callback, description, flags);
}

/**
 * Wrapper of RegConsoleCmd to prefix the commands with the project's shortname.
 * 
 * See native RegConsoleCmd.
 */
stock Project_RegConsoleCmd(const String:cmd[], ConCmd:callback, const String:description[]="", flags=0)
{
    decl String:project_cmdname[64];
    Format(project_cmdname, sizeof(project_cmdname), "%s_%s", PROJECT_CMD_PREFIX, cmd);
    
    RegConsoleCmd(project_cmdname, callback, description, flags);
}