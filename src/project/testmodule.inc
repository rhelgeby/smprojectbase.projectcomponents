/*
 * ============================================================================
 *
 *  Project
 *
 *  File:          testmodule.inc
 *  Type:          Module
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

#define NUM_MODULES 50

/**
 * A global variable to store the module identifiers in.
 */
stock Module:g_iTestModules[NUM_MODULES];

/**
 * Plugin is loading.
 * Register TestModule.
 */
public TestModule_OnPluginStart()
{
    new moduledata2[ModuleData] = {false, false, "", "testmoduleblank", "Module designed to fail to load."};
    ModuleMgr_Register(moduledata2);
    
    new moduledata3[ModuleData] = {false, false, "Fail Module", "", "Module designed to fail to load."};
    ModuleMgr_Register(moduledata3);
    
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    decl String:format1[32];
    for (new x = 0; x < NUM_MODULES; x++)
    {
        moduledata[ModuleData_Disabled] = !(bool:GetRandomInt(0, 50));
        moduledata[ModuleData_Hidden] = !(bool:GetRandomInt(0, 50));
        
        Format(format1, sizeof(format1), "Module_%02d", x);
        strcopy(moduledata[ModuleData_FullName], CM_DATA_FULLNAME, format1);
        
        Format(format1, sizeof(format1), "module%02d", x);
        strcopy(moduledata[ModuleData_ShortName], CM_DATA_SHORTNAME, format1);
        
        strcopy(moduledata[ModuleData_Description], CM_DATA_DESCRIPTION, "Module designed to test the project base.");
        
        // Register the module with this data.
        g_iTestModules[x] = ModuleMgr_Register(moduledata);
        
        // Now register the events we're going to use.
        #if defined EVENT_MANAGER
            EventMgr_RegisterEvent(g_iTestModules[x], Event_RoundStart, "TestModule_OnRoundStart");
            //EventMgr_RegisterEvent(g_iTestModules[x], Event_OnMapStart, "TestModule_OnMapStart");
            EventMgr_RegisterEvent(g_iTestModules[x], Event_PlayerSpawn, "TestModule_OnPlayerSpawn");
            EventMgr_RegisterEvent(g_iTestModules[x], Event_PlayerHurt, "TestModule_OnPlayerHurt");    
        #endif
        
        #if defined TRANSLATIONS_MANAGER
            // Register this module with the translations manager.
            // Make sure testmodule.phrase.txt exists in translations/project/
            //TransMgr_Register(g_iTestModules[x]);
        #endif
        
        #if defined CONFIG_MANAGER
            //ConfigMgr_Register(g_iTestModules[x], "TestModule_ReloadConfig", "configs/project/testconfig.txt");
        #endif
    }
    
    // ************************
    // Done registering module.
    // ************************
}

enum TestModuleConfigs
{
    String:TMConfig_SectionName[32] = 0,
    TMConfig_Int,
    Float:TMConfig_Float,
    String:TMConfig_String[32]
}

#if defined CONFIG_MANAGER
#define TM_MAX_KEYS 64
//new g_TMConfigCache[TM_MAX_KEYS][TestModuleConfigs];
#endif

/*
public TestModule_OnMapStart()
{
    #if defined CONFIG_MANAGER
        // Cache our configs.
        new count = ConfigMgr_CacheKv(g_iTestModule, CM_CONFIGINDEX_FIRST, "TestModule_ConfigCache");
        for (new x = 0; x < count; x++)
            PrintToServer("CACHED: INDEX: %d SECTION: %s INT %d FLOAT %f STRING %s", x, g_TMConfigCache[x][TMConfig_SectionName], g_TMConfigCache[x][TMConfig_Int], g_TMConfigCache[x][TMConfig_Float], g_TMConfigCache[x][TMConfig_String]);
    #endif
}

#if defined CONFIG_MANAGER
public TestModule_ReloadConfig(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_iTestModule, LogType_Normal, "TestConfigReload", "Reloaded test module's config (index %d)", configindex);
    #endif
    
    new count = ConfigMgr_CacheKv(g_iTestModule, CM_CONFIGINDEX_FIRST, "TestModule_ConfigCache");
    for (new x = 0; x < count; x++)
        PrintToServer("CACHED: INDEX: %d SECTION: %s INT %d FLOAT %f STRING %s", x, g_TMConfigCache[x][TMConfig_SectionName], g_TMConfigCache[x][TMConfig_Int], g_TMConfigCache[x][TMConfig_Float], g_TMConfigCache[x][TMConfig_String]);
}

public KvCache:TestModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    strcopy(g_TMConfigCache[sectionindex][TMConfig_SectionName], 32, sectionname);
    
    g_TMConfigCache[sectionindex][TMConfig_Int] =      KvGetNum(kv,    "int");
    g_TMConfigCache[sectionindex][TMConfig_Float] =    KvGetFloat(kv,  "float");
    
    KvGetString(kv, "string", g_TMConfigCache[sectionindex][TMConfig_String], 32);
}
#endif
*/

public TestModule_OnPlayerSpawn(client)
{
    //new Module:curmodule = EventMgr_GetEventOwner();
    
    #if defined LOG_MANAGER
        //LogMgr_Print(curmodule, LogType_Normal, "Event_PlayerSpawn", "Client %N spawned.", client);
    #endif
    
    #if defined ACCESS_MANAGER
    #if defined LOG_MANAGER
    //if (AccessMgr_HasAccess(client, curmodule))
        //LogMgr_Print(curmodule, LogType_Normal, "Event_PlayerSpawn", "Client %N has access to testmodule.", client);
    #endif
    #endif
}

public TestModule_OnPlayerHurt(client, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    //new Module:curmodule = EventMgr_GetEventOwner();
    
    #if defined LOG_MANAGER
        //LogMgr_Print(curmodule, LogType_Normal, "Event_PlayerHurt", "Client %N was hurt by %N health %d armor %d weapon %s dmg_health %d hitgroup %d", client, attacker, health, armor, weapon, dmg_health, hitgroup);
    #endif
}

/**
 * Round has started.
 */
public TestModule_OnRoundStart(dcell, Float:dfloat, any:darray[], String:dstring[])
{
    new Module:curmodule = EventMgr_GetEventOwner();
    
    #if defined TRANSLATIONS_MANAGER
        //TransMgr_PrintText(SERVER_INDEX, MsgFormat_Both, MsgType_Server, curmodule, false, "Event round_start");
        //TransMgr_PrintTextAll(false, false, MsgFormat_Both, MsgType_Chat, curmodule, false, "Event round_start");
    #endif
    
    #if defined EVENT_MANAGER
        // Switching event forward.
        EventMgr_SetEventForward(curmodule, Event_RoundStart, "TestModule_OnRoundStart2");
    #endif
}

/**
 * Round has started. (Alternate)
 */
public TestModule_OnRoundStart2(dcell, Float:dfloat, any:darray[], String:dstring[])
{
    new Module:curmodule = EventMgr_GetEventOwner();
    
    #if defined LOG_MANAGER
        //LogMgr_Print(curmodule, LogType_Normal, "Round_Start", "Round start!");
    #endif
    
    #if defined EVENT_MANAGER
        // Toggle player_hurt event
        if (EventMgr_IsEventActive(curmodule, Event_PlayerHurt))
        {
            //EventMgr_Disable(curmodule, Event_PlayerHurt);
            //PrintToServer("Disabled event player_hurt");
        }
        else
        {
            //EventMgr_Enable(curmodule, Event_PlayerHurt);
            //PrintToServer("Enabled event player_hurt");
        }
    #endif
}
