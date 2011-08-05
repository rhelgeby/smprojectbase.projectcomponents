#pragma semicolon 1
#include <sourcemod>
#include "libraries/dynmenulib"

new Handle:DummyStringEntries;

public OnPluginStart()
{
    LoadTranslations("project/dynmenu.phrases");
    
    RegConsoleCmd("sm_dynmenu_int_test", Command_IntTest);
    RegConsoleCmd("sm_dynmenu_float_test", Command_FloatTest);
    RegConsoleCmd("sm_dynmenu_string_list_test", Command_StringListTest);
    RegConsoleCmd("sm_dynmenu_multiselect_list_test", Command_MultiselectListTest);
    
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

PrintSelectedEntries(client, Handle:selectedEntries)
{
    new size = GetArraySize(selectedEntries);
    
    new String:list[128];
    new String:buffer[64];
    list[0] = 0;
    buffer[0] = 0;
    
    for (new entry = 0; entry < size; entry++)
    {
        if (bool:GetArrayCell(selectedEntries, entry) == true)
        {
            GetArrayString(DummyStringEntries, entry, buffer, sizeof(buffer));
            StrCat(list, sizeof(list), buffer);
            StrCat(list, sizeof(list), ", ");
        }
    }
    
    PrintToChat(client, "Selected entries: %s", list);
}

// -----------------------------------------------------------------------------

public Action:Command_IntTest(client, argc)
{
    new Handle:menu;
    
    // Must be set before building menu, so phrases are translated to correct
    // language.
    SetGlobalTransTarget(client);
    
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

/**
 * User selected/changed a value, or aborted selection (see action parameter).
 *
 * @param menu      Handle to dynamic menu.
 * @param client    Client that performed the action. Will be 0 if the menu was
 *                  aborted, but by something else than the client.
 * @param action    Menu action performed.
 * @param value     Value(s) selected by user. See below for details:
 *
 *                  Action selected
 *                  Numeric:     Selected value (cell or float).
 *                  String:      Entry index of selected string (in entry array).
 *                  Multiselect: Handle to array with entry indexes of selected
 *                               entries. This handle must be closed when it's
 *                               no longer in use.
 *
 *                  Action aborted
 *                  Always zero.
 *
 * @noreturn
 */
public IntTestHandler(Handle:menu, client, DynMenuAction:action, any:value)
{
    switch (action)
    {
        case DynMenuAction_Select:
        {
            PrintToChat(client, "Value changed: %d", value);
        }
        case DynMenuAction_Close:
        {
            PrintToServer("Integer test menu closed.");
            
            // Cleanup.
            DynMenuLib_DeleteMenu(menu, true);
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
    
    menu = DynMenuLib_CreateMenuEx("myMenu",                // ID
                                   "Select a value",        // Prompt
                                   DynMenu_Float,           // Data type
                                   DynMenu_Loop,            // Numerics do loop mode only
                                   INVALID_HANDLE,          // No array with entries for numerics
                                   FloatTestHandler,        // Callback
                                   -5.0,                    // Lower limit
                                   16.0,                    // Upper limit
                                   1.0,                     // Small step
                                   5.0,                     // Large step
                                   4.0);                    // Initial value
    
    DynMenuLib_DisplayMenu(menu, client);
    
    ReplyToCommand(client, "Menu displayed.");
    return Plugin_Handled;
}

/**
 * User selected/changed a value, or aborted selection (see action parameter).
 *
 * @param menu      Handle to dynamic menu.
 * @param client    Client that performed the action. Will be 0 if the menu was
 *                  aborted, but by something else than the client.
 * @param action    Menu action performed.
 * @param value     Value(s) selected by user. See below for details:
 *
 *                  Action selected
 *                  Numeric:     Selected value (cell or float).
 *                  String:      Entry index of selected string (in entry array).
 *                  Multiselect: Handle to array with entry indexes of selected
 *                               entries. This handle must be closed when it's
 *                               no longer in use.
 *
 *                  Action aborted
 *                  Always zero.
 *
 * @noreturn
 */
public FloatTestHandler(Handle:menu, client, DynMenuAction:action, any:value)
{
    switch (action)
    {
        case DynMenuAction_Select:
        {
            PrintToChat(client, "Value changed: %f", value);
        }
        case DynMenuAction_Close:
        {
            PrintToServer("Float test menu closed.");
            
            // Cleanup.
            DynMenuLib_DeleteMenu(menu, true);
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
    
    menu = DynMenuLib_CreateMenuEx("myMenu",                // ID
                                   "Select an option",      // Prompt
                                   DynMenu_String,          // Data type
                                   DynMenu_List,            // Mode
                                   DummyStringEntries,      // Dummy entries
                                   StringListHandler,       // Callback
                                   0,                       // Lower limit (not used with string type)
                                   0,                       // Upper limit (not used with string type)
                                   0,                       // Small step (not used in list mode)
                                   0,                       // Large step (not used in list mode)
                                   2);                      // Initial value (selected index)
    ReplyToCommand(client, "Menu created: %x", menu);
    
    DynMenuLib_DisplayMenu(menu, client);
    
    ReplyToCommand(client, "Menu displayed.");
    return Plugin_Handled;
}

/**
 * User selected/changed a value, or aborted selection (see action parameter).
 *
 * @param menu      Handle to dynamic menu.
 * @param client    Client that performed the action. Will be 0 if the menu was
 *                  aborted, but by something else than the client.
 * @param action    Menu action performed.
 * @param value     Value(s) selected by user. See below for details:
 *
 *                  Action selected
 *                  Numeric:     Selected value (cell or float).
 *                  String:      Entry index of selected string (in entry array).
 *                  Multiselect: Handle to array with entry indexes of selected
 *                               entries. This handle must be closed when it's
 *                               no longer in use.
 *
 *                  Action aborted
 *                  Always zero.
 *
 * @noreturn
 */
public StringListHandler(Handle:menu, client, DynMenuAction:action, any:value)
{
    switch (action)
    {
        case DynMenuAction_Select:
        {
            // Get entry string.
            new String:strValue[64];
            GetArrayString(DummyStringEntries, value, strValue, sizeof(strValue));
            
            PrintToChat(client, "Value selected: %s", strValue);
        }
        case DynMenuAction_Close:
        {
            PrintToServer("String list menu closed.");
            
            // Cleanup.
            DynMenuLib_DeleteMenu(menu, true);
        }
    }
}

// -----------------------------------------------------------------------------

public Action:Command_MultiselectListTest(client, argc)
{
    new Handle:menu;
    
    // Must be set before building menu, so phrases are translated to correct
    // language.
    SetGlobalTransTarget(client);
    
    menu = DynMenuLib_CreateMenuEx("myMenu",                // ID
                                   "Toggle options",        // Prompt
                                   DynMenu_Multiselect,     // Data type
                                   DynMenu_List,            // Mode
                                   DummyStringEntries,      // Dummy entries
                                   MultiselectListHandler,  // Callback
                                   0,                       // Lower limit (not used with string type)
                                   0,                       // Upper limit (not used with string type)
                                   0,                       // Small step (not used in list mode)
                                   0,                       // Large step (not used in list mode)
                                   INVALID_HANDLE);         // Initial value (no list of selected indexes; none selected)
    ReplyToCommand(client, "Menu created: %x", menu);
    
    DynMenuLib_DisplayMenu(menu, client);
    
    ReplyToCommand(client, "Menu displayed.");
    return Plugin_Handled;
}

/**
 * User selected/changed a value, or aborted selection (see action parameter).
 *
 * @param menu      Handle to dynamic menu.
 * @param client    Client that performed the action. Will be 0 if the menu was
 *                  aborted, but by something else than the client.
 * @param action    Menu action performed.
 * @param value     Value(s) selected by user. See below for details:
 *
 *                  Action selected
 *                  Numeric:     Selected value (cell or float).
 *                  String:      Entry index of selected string (in entry array).
 *                  Multiselect: Handle to array with entry indexes of selected
 *                               entries. This handle must be closed when it's
 *                               no longer in use.
 *
 *                  Action aborted
 *                  Always zero.
 *
 * @noreturn
 */
public MultiselectListHandler(Handle:menu, client, DynMenuAction:action, any:value)
{
    switch (action)
    {
        case DynMenuAction_Select:
        {
            PrintSelectedEntries(client, value);
        }
        case DynMenuAction_Close:
        {
            PrintToServer("Multiselect list menu closed.");
            
            // Cleanup.
            DynMenuLib_DeleteMenu(menu, true);
        }
    }
}
