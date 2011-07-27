#pragma semicolon 1
#include <sourcemod>
#include "libraries/dynmenulib"

/*
 * Testing note: Before displaying a new menu (repeating the command), the old
 *               menu must be deleted. Between each test command, do a delete
 *               command. Otherwise errors will be thrown.
 *
 */

new Handle:DummyStringEntries;

public OnPluginStart()
{
    LoadTranslations("project/dynmenu.phrases");
    
    RegConsoleCmd("sm_dynmenu_delete", Command_Delete);
    
    RegConsoleCmd("sm_dynmenu_int_test", Command_IntTest);
    RegConsoleCmd("sm_dynmenu_float_test", Command_FloatTest);
    RegConsoleCmd("sm_dynmenu_string_list_test", Command_StringListTest);
    
    DummyStringEntries = CreateArray(64);
    PushArrayString(DummyStringEntries, "Entry 1");
    PushArrayString(DummyStringEntries, "Entry 2");
    PushArrayString(DummyStringEntries, "Entry 3");
    PushArrayString(DummyStringEntries, "Dummy 1");
    PushArrayString(DummyStringEntries, "Dummy 2");
    PushArrayString(DummyStringEntries, "Dummy 3");
    PushArrayString(DummyStringEntries, "Object 1");
    PushArrayString(DummyStringEntries, "Object 2");
    PushArrayString(DummyStringEntries, "Object 3");
    PushArrayString(DummyStringEntries, "Item 1");
    PushArrayString(DummyStringEntries, "Item 2");
    PushArrayString(DummyStringEntries, "Item 3");
}

public Action:Command_Delete(client, argc)
{
    // Check if a menu already exist.
    new Handle:menu = DynMenuLib_FindMenuById("myMenu");
    if (menu != INVALID_HANDLE)
    {
        DynMenuLib_DeleteMenu("myMenu", false); // No recursive delete, keep entries.
        ReplyToCommand(client, "Menu deleted. Ready for new test.");
    }
    else
    {
        ReplyToCommand(client, "Nothing to delete. Ready for testing.");
    }
    
    return Plugin_Handled;
}

// -----------------------------------------------------------------------------

public Action:Command_IntTest(client, argc)
{
    new Handle:menu;
    
    // Must be set before building menu, so phrases are translated to correct
    // language.
    SetGlobalTransTarget(client);
    
    // Check if a menu already exist.
    menu = DynMenuLib_FindMenuById("myMenu");
    if (menu != INVALID_HANDLE)
    {
        ReplyToCommand(client, "Delete existing menu first: sm_dynmenu_delete");
        return Plugin_Handled;
    }
    
    menu = DynMenuLib_CreateMenuEx("myMenu",                // ID
                                   "Select a value",        // Prompt
                                   DynMenu_Integer,         // Data type
                                   DynMenu_Loop,            // Numerics do loop mode only
                                   INVALID_HANDLE,          // No array with entries for numerics
                                   IntTestHandler,          // Callback
                                   -5,                      // Lower limit
                                   16,                      // Upper limit
                                   1,                       // Small step
                                   5,                       // Large step
                                   4);                      // Initial value
    
    DynMenuLib_DisplayMenu(menu, client);
    
    ReplyToCommand(client, "Menu displayed.");
    return Plugin_Handled;
}

public IntTestHandler(Handle:menu, client, DynMenuAction:action, any:value)
{
    switch (action)
    {
        case DynMenuAction_Select:
        {
            PrintToChat(client, "Value changed: %d", value);
        }
        case DynMenuAction_Abort:
        {
            // Cleanup.
            
            // resolve id, delete.
            //DynMenuLib_DeleteMenu("myMenu");
        }
    }
}

// -----------------------------------------------------------------------------

public Action:Command_FloatTest(client, argc)
{
    new Handle:menu;
    
    // Must be set before building menu, so phrases are translated to correct
    // language.
    SetGlobalTransTarget(client);
    
    // Check if a menu already exist.
    menu = DynMenuLib_FindMenuById("myMenu");
    if (menu != INVALID_HANDLE)
    {
        ReplyToCommand(client, "Delete existing menu first: sm_dynmenu_delete");
        return Plugin_Handled;
    }
    
    menu = DynMenuLib_CreateMenuEx("myMenu",                // ID
                                   "Select a value",        // Prompt
                                   DynMenu_Float,           // Data type
                                   DynMenu_Loop,            // Numerics do loop mode only
                                   INVALID_HANDLE,          // No array with entries for numerics
                                   IntTestHandler,          // Callback
                                   -5.0,                    // Lower limit
                                   16.0,                    // Upper limit
                                   1.0,                     // Small step
                                   5.0,                     // Large step
                                   4.0);                    // Initial value
    
    DynMenuLib_DisplayMenu(menu, client);
    
    ReplyToCommand(client, "Menu displayed.");
    return Plugin_Handled;
}

public FloatTestHandler(Handle:menu, client, DynMenuAction:action, any:value)
{
    switch (action)
    {
        case DynMenuAction_Select:
        {
            PrintToChat(client, "Value changed: %f", value);
        }
        case DynMenuAction_Abort:
        {
            // Cleanup.
            
            // resolve id, delete.
            //DynMenuLib_DeleteMenu("myMenu");
        }
    }
}

// -----------------------------------------------------------------------------

public Action:Command_StringListTest(client, argc)
{
    new Handle:menu;
    
    // Must be set before building menu, so phrases are translated to correct
    // language.
    SetGlobalTransTarget(client);
    
    // Check if a menu already exist.
    menu = DynMenuLib_FindMenuById("myMenu");
    if (menu != INVALID_HANDLE)
    {
        ReplyToCommand(client, "Delete existing menu first: sm_dynmenu_delete");
        return Plugin_Handled;
    }
    
    menu = DynMenuLib_CreateMenuEx("myMenu",                // ID
                                   "Select an option",      // Prompt
                                   DynMenu_String,          // Data type
                                   DynMenu_List,            // Mode
                                   DummyStringEntries,      // Dummy entries
                                   StringListHandler,       // Callback
                                   0,                       // Lower limit (not used with string type)
                                   0,                       // Upper limit (not used with string type)
                                   1,                       // Small step
                                   5,                       // Large step
                                   2);                      // Initial value (selected index)
    
    DynMenuLib_DisplayMenu(menu, client);
    
    ReplyToCommand(client, "Menu displayed.");
    return Plugin_Handled;
}

public StringListHandler(Handle:menu, client, DynMenuAction:action, any:value)
{
    switch (action)
    {
        case DynMenuAction_Select:
        {
            PrintToChat(client, "Value changed: %f", value);
        }
        case DynMenuAction_Abort:
        {
            // Cleanup.
            
            // resolve id, delete.
            //DynMenuLib_DeleteMenu("myMenu");
        }
    }
}

