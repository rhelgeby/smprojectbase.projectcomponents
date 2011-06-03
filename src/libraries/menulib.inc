/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:          menulib.inc
 *  Type:          Library
 *  Description:   A menu system that multiple modules can build upon.
 *
 *  Copyright (C) 2009-2011  Greyscale
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

#if defined _menulib_included
 #endinput
#endif
#define _menulib_included

// Include libraries.
#include "<project>/libraries/utilities"

/**
 * The max string/array lengths of module data variables.
 */
#define ML_DATA_LABEL       32
#define ML_DATA_INFO        64

enum MenuLib_Button
{
    String:MenuBtn_Label[ML_DATA_LABEL],    // What to display for this menu button.
    String:MenuBtn_Info[ML_DATA_INFO],      // Identical to the "info" parameter in AddMenuItem, see that.
    Handle:MenuBtn_LinkedMenu,              // Menu to link this button to.  INVALID_HANDLE if this button doesn't link to another menu.
    Function:MenuBtn_Function               // Function to call when this button is selected.  INVALID_FUNCTION to call no function.
}

/**
 * Keeps track of the last menu the user had open.
 */
new Handle:g_hPrevMenus[MAXPLAYERS + 1];

/**
 * Dummy array used as a way to count the cells required to store menu data.
 */
static stock g_DummyMenuBtnData[MenuLib_Button];

/**
 * Creates a menu ready to be filled with menu buttons.
 * 
 * @param title The menu title.
 * 
 * @return      Handle to menu.
 */
stock Handle:MenuLib_CreateMenu(const String:title[])
{
    new Handle:hMenu = CreateArray(sizeof(g_DummyMenuBtnData));
    PushArrayString(hMenu, title);
    return hMenu;
}

/**
 * Deletes a menu page and all menu pages it links to, recursively.
 * 
 * @param   hMenu The handle to the menu page to delete.
 * 
 * @error   Invalid menu handle.
 */
stock MenuLib_DeleteMenu(Handle:hMenu, bool:recursive = false)
{
    if (hMenu == INVALID_HANDLE)
        return;
    
    if (recursive)
    {
        new buttoninfo[MenuLib_Button];
        new count = GetArraySize(hMenu);
        for (new bindex = 1; bindex < count; bindex++)
        {
            // Get all info about this menu button.
            MenuLib_GetMenuBtn(hMenu, bindex, buttoninfo);
            
            // If it links to another menu, recurse into it.
            if (buttoninfo[MenuBtn_LinkedMenu] == INVALID_HANDLE)
                MenuLib_DeleteMenu(buttoninfo[MenuBtn_LinkedMenu], true);
        }
    }
    
    CloseHandle(hMenu);
}

/**
 * Adds a menu button to a menu. (See below for easier method)
 * 
 * @param hMenu         Menu to add button to.
 * @param buttoninfo    Array containing button data.  See enum MenuLib_Button.
 */
stock MenuLib_AddMenuBtn(Handle:hMenu, buttoninfo[MenuLib_Button])
{
    PushArrayArray(hMenu, buttoninfo[0]);
}

/**
 * Adds a menu button with different parameters.
 * 
 * @param hMenu         Menu to add button to.
 * @param label         What to display for this menu button.
 * @param info          Identical to the "info" parameter in AddMenuItem, see that.
 * @param hLinkedMenu   The handle of the menu to link this button to.  INVALID_HANDLE to link to no other menu.
 * @param funcCallback  The function to call when this button is selected.  INVALID_FUNCTION to call no function.
 */
stock MenuLib_AddMenuBtnEx(Handle:hMenu, String:label[], const String:info[] = "", Handle:hLinkedMenu = INVALID_HANDLE, Function:funcCallback = INVALID_FUNCTION)
{
    // Create array.
    new buttoninfo[MenuLib_Button];
    strcopy(buttoninfo[MenuBtn_Label], sizeof(buttoninfo[MenuBtn_Label]), label);
    strcopy(buttoninfo[MenuBtn_Info], sizeof(buttoninfo[MenuBtn_Info]), info);
    buttoninfo[MenuBtn_LinkedMenu] = hLinkedMenu;
    buttoninfo[MenuBtn_Function] = funcCallback;
    
    PushArrayArray(hMenu, buttoninfo[0]);
}

/**
 * Removes a menu button from the menu.
 * 
 * @param hMenu         Menu to add button to.
 * @param buttonindex   The index of the button to modify in the menu. (Button 1 = 1, Button 2 = 2, etc)
 */
stock MenuLib_RemoveButton(Handle:hMenu, buttonindex)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    RemoveFromArray(hMenu, buttonindex);
}

/**
 * Removes all menu buttons from the menu.
 * 
 * @param hMenu         Menu to add button to.
 */
stock MenuLib_RemoveAllButtons(Handle:hMenu)
{
    while(GetArraySize(hMenu) > 1)
        RemoveFromArray(hMenu, 1);
}

/**
 * Get title of a menu.
 * 
 * @param hMenu     The handle of menu whose title to get.
 * @param title     The returned menu title.
 * @param maxlen    The max length of the return string.
 */
stock MenuLib_GetMenuTitle(Handle:hMenu, String:title[], maxlen)
{
    GetArrayString(hMenu, 0, title, maxlen);
}

/**
 * Gets all information about a menu button.
 * 
 * @param hMenu         Menu to get button from.
 * @param buttonindex   The index of the button to get data for. (Button 1 = 1, Button 2 = 2, etc)
 * @param buttoninfo    Data is returned in this array.
 * 
 * @error               Invalid button.
 */
stock MenuLib_GetMenuBtn(Handle:hMenu, buttonindex, buttoninfo[MenuLib_Button])
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
}

/**
 * Get a menu button's label.
 * 
 * @param hMenu         Menu to get button from.
 * @param buttonindex   The index of the button to get data for. (Button 1 = 1, Button 2 = 2, etc)
 * @param label         The button label is returned in this variable.
 * @param maxlen        The max length of the return string. 
 * 
 * @error               Invalid button.
 */
stock MenuLib_GetMenuBtnLabel(Handle:hMenu, buttonindex, String:label[], maxlen)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    
    strcopy(label, maxlen, buttoninfo[MenuBtn_Label]);
}

/**
 * Get a menu button's label.
 * 
 * @param hMenu         Menu to get button from.
 * @param buttonindex   The index of the button to get data for. (Button 1 = 1, Button 2 = 2, etc)
 * @param info          The button info is returned in this variable.
 * @param maxlen        The max length of the return string. 
 * 
 * @error               Invalid button.
 */
stock MenuLib_GetMenuBtnInfo(Handle:hMenu, buttonindex, String:info[], maxlen)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    
    strcopy(info, maxlen, buttoninfo[MenuBtn_Info]);
}

/**
 * Get a menu button's linked menu.
 * 
 * @param hMenu         Menu to get button from.
 * @param buttonindex   The index of the button to get data for. (Button 1 = 1, Button 2 = 2, etc)
 * 
 * @error               Invalid button.
 * @return              Handle of the menu linked to this button, INVALID_HANDLE if no menu is linked.
 */
stock Handle:MenuLib_GetMenuBtnLinkedMenu(Handle:hMenu, buttonindex)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    
    return buttoninfo[MenuBtn_LinkedMenu];
}

/**
 * Get the menu button's function.
 * 
 * @param hMenu         Menu to get button from.
 * @param buttonindex   The index of the button to get data for. (Button 1 = 1, Button 2 = 2, etc)
 * 
 * @error               Invalid button.
 * @return              The function ID of the function attached to this button, INVALID_FUNCTION if no function is attached.
 */
stock Function:MenuLib_GetMenuBtnFunction(Handle:hMenu, buttonindex)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    
    return buttoninfo[MenuBtn_Function];
}

/**
 * Edit title of a menu.
 * 
 * @param hMenu     The handle of menu whose title to edit. 
 * @param title     New menu title.
 */
stock MenuLib_EditMenuTitle(Handle:hMenu, const String:title[])
{
    SetArrayString(hMenu, 0, title);
}

/**
 * Edits an existing menu button.
 * 
 * @param hMenu         Menu containing the button we want to edit.
 * @param buttonindex   The index of the button to modify in the menu. (Button 1 = 1, Button 2 = 2, etc)
 * @param buttoninfo    Array containing button data.  See enum MenuLib_Button.
 */
stock MenuLib_EditMenuBtn(Handle:hMenu, buttonindex, buttoninfo[MenuLib_Button])
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    SetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
}

/**
 * Edits the label of a menu button.
 * 
 * @param hMenu         Menu containing the button we want to edit.
 * @param buttonindex   The index of the button to modify in the menu. (Button 1 = 1, Button 2 = 2, etc)
 * @param label         New menu button label.
 */
stock MenuLib_EditMenuBtnLabel(Handle:hMenu, buttonindex, String:label[])
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    strcopy(buttoninfo[MenuBtn_Label], sizeof(buttoninfo[MenuBtn_Label]), label);
    SetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
}

/**
 * Edits the info of a menu button.
 * 
 * @param hMenu         Menu containing the button we want to edit.
 * @param buttonindex   The index of the button to modify in the menu. (Button 1 = 1, Button 2 = 2, etc)
 * @param info          New menu button info.
 */
stock MenuLib_EditMenuBtnInfo(Handle:hMenu, buttonindex, String:info[])
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    strcopy(buttoninfo[MenuBtn_Info], sizeof(buttoninfo[MenuBtn_Info]), info);
    SetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
}

/**
 * Edits the menu linked to this button.
 * 
 * @param hMenu         Menu containing the button we want to edit.
 * @param buttonindex   The index of the button to modify in the menu. (Button 1 = 1, Button 2 = 2, etc)
 * @param hLinkedMenu   New menu that this button is linked to.
 */
stock MenuLib_EditMenuBtnLinkedMenu(Handle:hMenu, buttonindex, Handle:hLinkedMenu)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    buttoninfo[MenuBtn_LinkedMenu] = hLinkedMenu;
    SetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
}

/**
 * Edits the function that is attached to this button.
 * 
 * @param hMenu         Menu containing the button we want to edit.
 * @param buttonindex   The index of the button to modify in the menu. (Button 1 = 1, Button 2 = 2, etc)
 * @param funcCallback  New function attached to this button.
 */
stock MenuLib_EditMenuBtnFunction(Handle:hMenu, buttonindex, Function:funcCallback)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    buttoninfo[MenuBtn_Function] = funcCallback;
    SetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
}

/**
 * Displays a menulib-built menu to a client, resetting the previous menu stack, which guarantees this menu will have no "Back" button.
 * 
 * @param hMenu     The handle of the menu to send.
 * @param client    The client to send the menu to.
 */
stock MenuLib_DisplayMenu(Handle:hMenu, client)
{
    if (g_hPrevMenus[client] != INVALID_HANDLE)
        CloseHandle(g_hPrevMenus[client]);
    g_hPrevMenus[client] = CreateStack();
    
    MenuLib_SendMenu(Handle:hMenu, client);
}

/**
 * Send a menu to a client, without resetting the previous menu stack. (the "Back" button on this menu will point back to the menu currently open on their screen.)
 * 
 * @param hMenu     The handle of the menu to send.
 * @param client    The client to send the menu to.
 */
stock MenuLib_SendMenu(Handle:hMenu, client)
{
    // Ensure that there is a prev menu stack available.
    if (g_hPrevMenus[client] == INVALID_HANDLE)
        g_hPrevMenus[client] = CreateStack();
    
    // Get menu title.
    decl String:title[ML_DATA_LABEL];
    GetArrayString(hMenu, 0, title, sizeof(title));
    
    // Create menu and set title.
    new Handle:hSMMenu = CreateMenu(MenuLib_Handler);
    SetMenuTitle(hSMMenu, title);
    
    // Add exit back button if applicable.
    SetMenuExitBackButton(hSMMenu, !IsStackEmpty(g_hPrevMenus[client]));
    
    // Add menu items.
    decl String:blabel[ML_DATA_LABEL];
    decl String:strHandle[16];
    new count = GetArraySize(hMenu);
    for (new bindex = 1; bindex < count; bindex++)
    {
        MenuLib_GetMenuBtnLabel(hMenu, bindex, blabel, sizeof(blabel));
        IntToString(_:hMenu, strHandle, sizeof(strHandle));
        AddMenuItem(hSMMenu, strHandle, blabel);
    }
    
    DisplayMenu(hSMMenu, client, MENU_TIME_FOREVER);
}

public MenuLib_Handler(Handle:menu, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        // Resolve string into the handle of the menu this menu button is from.
        decl String:strHandle[16];
        GetMenuItem(menu, slot, strHandle, sizeof(strHandle));
        new Handle:hMenu = Handle:StringToInt(strHandle);   // Handle of menulib menu, not SM menu handle.
        
        // Call the module's callback for this button.
        new Function:funcCallback = MenuLib_GetMenuBtnFunction(hMenu, slot+1);
        if (funcCallback != INVALID_FUNCTION)
        {
            Call_StartFunction(GetMyHandle(), funcCallback);
            Call_PushCell(hMenu);
            Call_PushCell(action);
            Call_PushCell(client);
            Call_PushCell(slot+1);
            Call_Finish();
        }
        
        new Handle:hLinkedMenu = MenuLib_GetMenuBtnLinkedMenu(hMenu, slot+1);
        if (hLinkedMenu != INVALID_HANDLE)
        {
            PushStackCell(g_hPrevMenus[client], hMenu);
            MenuLib_SendMenu(hLinkedMenu, client);
        }
    }
    else if (action == MenuAction_Cancel)
    {
        if (slot == MenuCancel_ExitBack)
        {
            new Handle:hPrevMenu;
            if (PopStackCell(g_hPrevMenus[client], hPrevMenu))
                MenuLib_SendMenu(hPrevMenu, client);
        }
        else
        {
            CloseHandle(g_hPrevMenus[client]);
            g_hPrevMenus[client] = INVALID_HANDLE;
        }
    }
    else if (action == MenuAction_End)
    {
        CloseHandle(menu);
    }
}

// *****************************
//        Input Menus
// *****************************

/**
 * Shows a list of all clients to a client, different handlers can be used for this, as well as title.
 * 
 * @param client    The client index.
 * @param handler   The menu handler.
 * @param title     The title of the menu.
 * @param filters   A bit field made up of UTILS_FILTER_* defines.  See utilities.inc.
 *                  Note: This is only checked at time of menu creation.  It is
 *                  recommended to re-check these filters in the callback.
 */
stock MenuLib_ClientMenu(client, MenuHandler:handler, const String:title[], filters)
{
    new Handle:menu_clients = CreateMenu(handler);
    SetMenuTitle(menu_clients, title);
    
    decl String:clientoption[64];
    decl String:clientuserid[8];
    new menuclient;
    
    new Handle:adtClients;
    new count = Util_BuildClientList(adtClients, filters, client);
    for (new cindex = 0; cindex < count; cindex++)
    {
        // Get the client index from the array.
        menuclient = GetArrayCell(adtClients, cindex);
        
        // Get client info.
        GetClientName(menuclient, clientoption, sizeof(clientoption));
        IntToString(GetClientUserId(menuclient), clientuserid, sizeof(clientuserid));
        
        // Add option to menu.
        AddMenuItem(menu_clients, clientuserid, clientoption);
        count++;
    }
    
    CloseHandle(adtClients);
    
    // If there are no clients, add an "(Empty)" line.
    if (count == 0)
    {
        decl String:empty[64];
        Format(empty, sizeof(empty), "%T", client, "Menu empty");
        AddMenuItem(menu_clients, "empty", empty, ITEMDRAW_DISABLED);
    }
    
    SetMenuExitBackButton(menu_clients, true);
    DisplayMenu(menu_clients, client, MENU_TIME_FOREVER);
}

/**
 * Retrieves the client index given a menu slot from a MenuLib_ClientMenu-constructed menu.
 * Must be used from inside the menu's callback.
 * 
 * @param menu  The menu handle.
 * @param slot  The menu slot that was selected.
 * 
 * @return      The client index, 0 if the selected client is no longer in the server.
 */
stock MenuLib_GetClientIndex(Handle:menu_clients, slot)
{
    // Get string stored in the menu slot.
    decl String:clientuserid[8];
    GetMenuItem(menu_clients, slot, clientuserid, sizeof(clientuserid));
    
    // Return the targeted client through their userid stored as a string in the menu slot.
    return GetClientOfUserId(StringToInt(clientuserid));
}

// ***********************************
//      Miscellaneous Utilities
// ***********************************

/**
 * Converts a boolean value into a menu draw define.
 * true =   ITEMDRAW_DEFAULT
 * false =  ITEMDRAW_DISABLED
 * 
 * @param condition The boolean to convert. 
 */
stock MenuLib_GetMenuItemDraw(bool:condition)
{
    return condition ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED;
}
