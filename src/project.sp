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

#define VERSION "1.0"

// Comment this line to exclude version info command. Enable this if you have
// the repository and HG installed (Mercurial or TortoiseHG).
#define ADD_VERSION_INFO

// Base project includes.

#include "project/project"
#include "project/translationsmanager"
#include "project/eventmanager"
//#include "project/logmanager"
#include "project/modulemanager"

// Module includes

// Special "dual" module.  hgversion is generated automatically and versioninfo uses it.
#if defined ADD_VERSION_INFO
#include "project/hgversion"
#include "project/versioninfo"
#endif

#include "project/testmodule"

/**
 * Record plugin info.
 */
public Plugin:myinfo =
{
    name = "Project",
    author = "Author",
    description = "Project description",
    version = VERSION,
    url = ""
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
    // Forward event to other project base files.
    ModuleMgr_OnPluginStart();
    
    #if defined TRANSLATIONS_MANAGER
        TransMgr_OnPluginStart();
    #endif
    
    #if defined EVENT_MANAGER
        EventMgr_OnPluginStart();
    #endif
    
    #if defined LOG_MANAGER
        LogMgr_OnPluginStart();
    #endif
    
    // Register modules here.
    #if defined ADD_VERSION_INFO
        VersionInfo_OnPluginStart();
    #endif
    TestModule_OnPluginStart();
    
    
    // All modules should be registered by this point!
    
    // Forward event to other project base files.
    #if defined TRANSLATIONS_MANAGER
        TransMgr_OnModulesRegistered();
    #endif
}

/**
 * All plugins have finished loading.
 */
public OnAllPluginsLoaded()
{
}

/**
 * The map is starting.
 */
public OnMapStart()
{
}

/**
 * The map is ending.
 */
public OnMapEnd()
{
}

/**
 * Main configs were just executed.
 */
public OnAutoConfigsBuffered()
{
}

/**
 * Configs just finished getting executed.
 */
public OnConfigsExecuted()
{
}

/**
 * Client is joining the server.
 * 
 * @param client    The client index.
 */
public OnClientPutInServer(client)
{
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
public OnClientDisconnect(client)
{
}
