/*
 * ============================================================================
 *
 *  Project
 *
 *  File:          testmodule.inc
 *  Type:          Base
 *  Description:   Test module designed with no other purpose other than testing project base components.
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
 * A global variable to store this module's ID in.
 */
new Module:g_iTestModule;
new Module:g_iTestModule2;

/**
 * Plugin is loading.
 * Register TestModule.
 */
public TestModule_OnPluginStart()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData] = {false, false, "Test Module", "testmodule", "Module designed to test the project base."};
    
    // Register the module with this data.
    g_iTestModule = ModuleMgr_Register(moduledata);
    
    new moduledata2[ModuleData] = {false, false, "", "testmoduleblank", "Module designed to fail to load."};
    ModuleMgr_Register(moduledata2);
    
    new moduledata3[ModuleData] = {false, false, "Test Module Fail!", "testmodule", "Module designed to fail to load."};
    ModuleMgr_Register(moduledata3);
    
    new moduledata4[ModuleData] = {false, true, "Hidden Module", "hiddenmodule", "Hidden module."};
    g_iTestModule2 = ModuleMgr_Register(moduledata4);
    
    // Now register the events we're going to use.
    EventMgr_RegisterEvent(g_iTestModule, Event_RoundStart, "TestModule_OnRoundStart");
    EventMgr_RegisterEvent(g_iTestModule, Event_OnMapStart, "TestModule_OnMapStart");
    EventMgr_RegisterEvent(g_iTestModule, Event_PlayerSpawn, "TestModule_OnPlayerSpawn");
    EventMgr_RegisterEvent(g_iTestModule, Event_PlayerHurt, "TestModule_OnPlayerHurt");
    EventMgr_RegisterEvent(g_iTestModule, Event_WeaponFire, "TestModule_OnWeaponFire");
    EventMgr_RegisterEvent(g_iTestModule2, Event_RoundStart, "TestModule_OnRoundStartHidden");
    
    // Register this module with the translations manager.
    // Make sure testmodule.phrase.txt exists in translations/project/
    //TransMgr_Register(g_iTestModule);
    
    ConfigMgr_Register(g_iTestModule, "configs/project/invalid.txt");
    ConfigMgr_Register(g_iTestModule, "configs/project/testconfig.txt");
    ConfigMgr_Register(g_iTestModule, "configs/project/testconfig.txt"); // duplicates test
    
    ConfigMgr_CacheKv(g_iTestModule, 0, "TestModuleConfigCache");
    
    // ************************
    // Done registering module.
    // ************************
    
}

public KvCache:TestModuleConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    PrintToServer("index %d name %s", sectionindex, sectionname);
}

public TestModule_OnMapStart()
{
    LogMgr_Print(g_iTestModule, LogType_Normal, "Event_OnMapStart", "OnMapStart");
}

public TestModule_OnPlayerSpawn(client)
{
    LogMgr_Print(g_iTestModule, LogType_Normal, "Event_PlayerSpawn", "Client %N spawned.", client);
    if (AccessMgr_HasAccess(client, g_iTestModule))
        LogMgr_Print(g_iTestModule, LogType_Normal, "Event_PlayerSpawn", "Client %N has access to testmodule.", client);
}

public TestModule_OnPlayerHurt(client, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    LogMgr_Print(g_iTestModule, LogType_Normal, "Event_PlayerHurt", "Client %N was hurt by %N health %d armor %d weapon %s dmg_health %d hitgroup %d", client, attacker, health, armor, weapon, dmg_health, hitgroup);
}

public TestModule_OnWeaponFire(client, const String:weapon[])
{
    LogMgr_Print(g_iTestModule, LogType_Normal, "Event_OnWeaponFire", "Client %N fired weapon %s", client, weapon);
}

/**
 * Round has started.
 */
public TestModule_OnRoundStart(dcell, Float:dfloat, any:darray[], String:dstring[])
{
    //TransMgr_PrintText(SERVER_INDEX, MsgFormat_Both, MsgType_Server, g_iTestModule, false, "Event round_start");
    //TransMgr_PrintTextAll(true, false, MsgFormat_Both, MsgType_Chat, g_iTestModule, false, "Event round_start");
    LogMgr_Print(g_iTestModule, LogType_Normal, "Round_Start", "The round has started.");
    
    // Switching event forward.
    EventMgr_SetEventForward(g_iTestModule, Event_RoundStart, "TestModule_OnRoundStart2");
}

/**
 * Round has started. (Alternate)
 */
public TestModule_OnRoundStart2(dcell, Float:dfloat, any:darray[], String:dstring[])
{
    //TransMgr_PrintText(SERVER_INDEX, MsgFormat_Plugin, MsgType_Server, _, true, "Event round_start alternate");
    //TransMgr_PrintTextAll(true, false, MsgFormat_Both, MsgType_Chat, g_iTestModule, true, "Event round_start alternate");
    //TransMgr_PrintTextAll(false, false, MsgFormat_None, MsgType_Center, _, true, "Event round_start alternate");
    //TransMgr_PrintTextAll(false, false, MsgFormat_Plugin, MsgType_Hint, _, true, "Event round_start alternate");
    
    LogMgr_Print(g_iTestModule, LogType_Normal, "Round_Start", "Alternate round_start event.");
    //LogMgr_Print(g_iTestModule, LogType_Fatal_Module, "Round_Start", "Fatal error!  Disabling module.");
    
    // Disable event.
    //EventMgr_Disable(g_iTestModule, Event_RoundStart);
}

/**
 * Round has started.
 */
public TestModule_OnRoundStartHidden()
{
    //TransMgr_PrintTextAll(true, false, MsgFormat_Both, MsgType_Chat, g_iTestModule2, true, "(Hidden) Round started!");
}
