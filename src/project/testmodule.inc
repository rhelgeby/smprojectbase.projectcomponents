/*
 * ============================================================================
 *
 *  Project
 *
 *  File:          testmodule.inc
 *  Type:          Base
 *  Description:   Test module designed with no other purpose but to test the project base.
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
new g_iTestModuleID;

/**
 * Plugin is loading.
 * Register TestModule.
 */
public TestModule_OnPluginStart()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData] = {false, "Test Module", "testmodule"};
    
    // Register the module with this data.
    g_iTestModuleID = ModuleMgr_Register(moduledata);
    
    // Now register the events we're going to use.
    EventMgr_RegisterEvent(g_iTestModuleID, Event_RoundStart, "TestModule_OnRoundStart");
    
    // Register this module with the translations manager.
    // Make sure testmodule.phrase.txt exists in translations/project/
    TransMgr_Register(g_iTestModuleID);
    
    // ************************
    // Done registering module.
    // ************************
    
}

/**
 * Round has started.
 */
public TestModule_OnRoundStart()
{
    //TransMsg_PrintText(PRINT_SERVER, MsgFormat_Both, MsgType_Server, g_iTestModuleID, false, "Event round_start");
    TransMsg_PrintTextAll(true, false, MsgFormat_Both, MsgType_Chat, g_iTestModuleID, "Event round_start");
    
    // Switching event forward.
    EventMgr_SetEventForward(g_iTestModuleID, Event_RoundStart, "TestModule_OnRoundStart2");
}

/**
 * Round has started. (Alternate)
 */
public TestModule_OnRoundStart2()
{
    //TransMsg_PrintText(PRINT_SERVER, MsgFormat_Plugin, MsgType_Server, -1, false, "Event round_start alternate");
    TransMsg_PrintTextAll(true, false, MsgFormat_Both, MsgType_Chat, g_iTestModuleID, "Event round_start alternate");
    TransMsg_PrintTextAll(false, false, MsgFormat_None, MsgType_Center, _, "Event round_start alternate");
    TransMsg_PrintTextAll(false, false, MsgFormat_Plugin, MsgType_Hint, _, "Event round_start alternate");
    
    // Disable event.
    //EventMgr_Disable(g_iTestModuleID, Event_RoundStart);
}
