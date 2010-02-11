/*
 * ============================================================================
 *
 *  <Project Name>
 *
 *  File:          .inc
 *  Type:          
 *  Description:   
 *
 *  Copyright (C) 2009-2010  <Author>
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
 * This module's identifier.
 */
new Module:g_moduleModule;

/**
 * Register this module.
 */
_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = ;
    moduledata[ModuleData_Disabled] = ;
    moduledata[ModuleData_Hidden] = ;
    strcopy(moduledata[ModuleData_FullName], CM_DATA_FULLNAME, "");
    strcopy(moduledata[ModuleData_ShortName], CM_DATA_SHORTNAME, "");
    strcopy(moduledata[ModuleData_Description], CM_DATA_DESCRIPTION, "");
    
    // Send this array of data to the module manager.
    g_moduleModule = ModuleMgr_Register(moduledata);
    
    // Register all the events needed for this module.
    //EventMgr_RegisterEvent(g_moduleModule, Event_OnAllPluginsLoaded,      "_OnAllPluginsLoaded");
    //EventMgr_RegisterEvent(g_moduleModule, Event_OnPluginEnd,             "_OnPluginEnd");
    //EventMgr_RegisterEvent(g_moduleModule, Event_OnMapStart,              "_OnMapStart");
    //EventMgr_RegisterEvent(g_moduleModule, Event_OnMapEnd,                "_OnMapEnd");
    //EventMgr_RegisterEvent(g_moduleModule, Event_OnAutoConfigsBuffered,   "_OnAutoConfigsBuffered");
    //EventMgr_RegisterEvent(g_moduleModule, Event_OnConfigsExecuted,       "_OnConfigsExecuted");
    //EventMgr_RegisterEvent(g_moduleModule, Event_OnClientPutInServer,     "_OnClientPutInServer");
    //EventMgr_RegisterEvent(g_moduleModule, Event_OnClientDisconnect,      "_OnClientDisconnect");
    //EventMgr_RegisterEvent(g_moduleModule, Event_RoundStart,              "_RoundStart");
    //EventMgr_RegisterEvent(g_moduleModule, Event_RoundFreezeEnd,          "_RoundFreezeEnd");
    //EventMgr_RegisterEvent(g_moduleModule, Event_RoundEnd,                "_RoundEnd");
    //EventMgr_RegisterEvent(g_moduleModule, Event_PlayerTeam,              "_PlayerTeam");
    //EventMgr_RegisterEvent(g_moduleModule, Event_PlayerSpawn,             "_PlayerSpawn");
    //EventMgr_RegisterEvent(g_moduleModule, Event_PlayerHurt,              "_PlayerHurt");
    //EventMgr_RegisterEvent(g_moduleModule, Event_PlayerDeath,             "_PlayerDeath");
    //EventMgr_RegisterEvent(g_moduleModule, Event_PlayerJump,              "_PlayerJump");
    //EventMgr_RegisterEvent(g_moduleModule, Event_WeaponFire,              "_WeaponFire");
    
    // Register a translations file for this module.
    // Make sure <moduleshortname>.phrases.txt exists in translations/<projectshortname>/
    // TransMgr_Register(g_moduleModule);
    
    // Register config file(s) that this module will use.
    // ConfigMgr_Register(g_moduleModule, "_OnConfigReload", "configs/<projectshortname>/<config>.txt");
}

/**
 * Plugin is loading.
 */
_OnPluginStart()
{
    // Register the module.
    _Register();
}

/**
 * All plugins have loaded.
 */
public _OnAllPluginsLoaded()
{
}

/**
 * Plugin is ending.
 */
public _OnPluginEnd()
{
}

/**
 * The map has started.
 */
public _OnMapStart()
{
}

/**
 * The map has ended.
 */
public _OnMapEnd()
{
}

/**
 * This is called before OnConfigsExecuted but any time after OnMapStart.
 * Per-map settings should be set here. 
 */
public _OnAutoConfigsBuffered()
{
}

/**
 * All convars are set, cvar-dependent code should use this.
 */
public _OnConfigsExecuted()
{
}

/**
 * Called when a registered config file (by this module) is manually.
 */
public _OnConfigReload(configindex)
{
}

/**
 * Client has joined the server.
 * 
 * @param client    The client index.
 */
public _OnClientPutInServer(client)
{
}

/**
 * Client is disconnecting from the server.
 * 
 * @param client    The client index.
 */
public _OnClientDisconnect(client)
{
}

/**
 * Round has started.
 */
public _RoundStart()
{
}

/**
 * Pre-round has freezetime has finished.
 */
public _RoundFreezeEnd()
{
}

/**
 * Round has ended.
 */
public _RoundEnd()
{
}

/**
 * Client has joined a team.
 * 
 * @param client        The client index.
 * @param team          The client's new team.
 * @param oldteam       The team the client is switching from.
 * @param disconnect    True if the client is disconnecting, false if switching to a team.
 */
public _PlayerTeam(client, team, oldteam, bool:disconnect)
{
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 * 
 */
public _PlayerSpawn(client)
{
}

/**
 * Client has been damaged.
 * 
 * @param victim        The index of the hurt client.
 * @param attacker      The index of the attacking client.
 * @param health        How much health the client has after the damage.
 * @param armor         How much armor the client has after the damage.
 * @param weapon        The weapon classname used to hurt the victim. (No weapon_ prefix)
 * @param dmg_health    The amount of health the victim lost.
 * @param dmg_armor     The amount of armor the victim lost.
 * @param hitgroup      The hitgroup index of the victim that was damaged.
 */
public _PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
}

/**
 * Client has been killed.
 * 
 * @param victim    The index of the killed client.
 * @param attacker  The killer of the victim.
 * @param weapon    The weapon classname used to kill the victim. (No weapon_ prefix)
 * @param headshot  True if the death was by headshot, false if not.
 */
public _PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
}

/**
 * Client has jumped.
 * 
 * @param client        The client index.
 */
public _PlayerJump(client)
{
}

/**
 * Client has fired a weapon.
 * 
 * @param client        The client index.
 * @param weapon        The weapon classname fired. (No weapon_ prefix)
 */
public _WeaponFire(client, const String:weapon[])
{
}
