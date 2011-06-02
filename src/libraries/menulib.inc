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
#include "zr/libraries/utilities"

/**
 * The max string/array lengths of module data variables.
 */
#define ML_DATA_LABEL       32
#define ML_DATA_STRINFO     128

enum MenuLib_ButtonType
{
    ButtonType_ActionNoInput,   // Calls a function with no parameters.
    ButtonType_ActionOneInput,  // Calls a function with a single input.
    ButtonType_LinkMenu         // Opens another menu.
}

enum MenuLib_Button
{
    String:Button_Label[ML_DATA_LABEL],     // What to display for this menu button.
    MenuLib_ButtonType:Button_Type,         // See enum MenuLib_ButtonType.
    Handle:hButton_Info,                    // Data associated with this button. (Depends on ButtonType)
    String:strButton_Info[ML_DATA_STRINFO]  // ^^
}

/**
 * Dummy array used as a way to count the cells required to store menu data.
 */
stock g_DummyMenuButtonData[MenuLib_Button];

/**
 * Creates a menu ready to be filled with menu buttons.
 * 
 * @param title The menu title.
 * 
 * @return      Handle to menu.
 */
stock Handle:MenuLib_CreateMenu(const String:title[])
{
    new Handle:hMenu = CreateArray(sizeof(g_DummyMenuButtonData));
    PushArrayString(hMenu, title);
    return hMenu;
}

/**
 * Simply deletes a menu page.  Menus under this will not be touched.  See MenuLib_ExplodeMenu for more options.
 * 
 * @param   hMenu The handle to the menu page to delete.
 * 
 * @error   Invalid menu handle.
 */
stock MenuLib_DeleteMenu(Handle:hMenu)
{
    CloseHandle(hMenu);
}

static stock MenuLib_ExplodeMenuR(Handle:hMenu)
{
    if (hMenu == INVALID_HANDLE)
        return;
    
    new buttoninfo[MenuLib_Button];
    new count = GetArraySize(hMenu);
    for (new bindex = 1; bindex < count; bindex++)
    {
        // Get all info about this menu button.
        MenuLib_GetMenuButton(hMenu, bindex, buttoninfo);
        
        // If it links to another menu, recurse into it.
        if (buttoninfo[Button_Type] == ButtonType_LinkMenu)
            MenuLib_ExplodeMenuR(buttoninfo[hButton_Info]);
    }
    
    CloseHandle(hMenu);
}

/**
 * Deletes a menu page and all menu pages it links to, recursively.
 * 
 * @param   hMenu The handle to the menu page to delete.
 * 
 * @error   Invalid menu handle.
 */
stock MenuLib_ExplodeMenu(Handle:hMenu)
{
    MenuLib_ExplodeMenuR(hMenu);
}

/**
 * Adds a menu button to a menu. (See below for easier methods)
 * 
 * @param hMenu         Menu to add button to.
 * @param buttoninfo    Array containing button data.  See enum MenuLib_Button.
 */
stock MenuLib_AddMenuButton(Handle:hMenu, buttoninfo[MenuLib_Button])
{
    PushArrayArray(hMenu, buttoninfo[0]);
}

/**
 * Adds a menu button that performs an action with no input.
 * 
 * @param hMenu         Menu to add button to.
 * @param label         What to display for this menu button.
 */
stock MenuLib_AddMenuButtonNoInput(Handle:hMenu, String:label[])
{
    // Create array.
    new buttoninfo[MenuLib_Button];
    strcopy(buttoninfo[Button_Label], sizeof(buttoninfo[Button_Label]), label);
    buttoninfo[Button_Type] = ButtonType_ActionNoInput;
    
    PushArrayArray(hMenu, buttoninfo[0]);
}

/**
 * Adds a menu button that performs an action with one input.
 * 
 * @param hMenu         Menu to add button to.
 * @param label         What to display for this menu button.
 * @param input         
 */
/*
stock MenuLib_AddMenuButtonOneInput(Handle:hMenu, String:label[], String:input[])
{
    // Create array.
    new buttoninfo[MenuLib_Button];
    strcopy(buttoninfo[Button_Label], sizeof(buttoninfo[ButtonLabel]), label);
    buttoninfo[Button_Type] = ButtonType_ActionOneInput;
    strcopy(buttoninfo[strButton_Info], sizeof(buttoninfo[ButtonLabel]), label);
    
    PushArrayArray(hMenu, buttoninfo[0]);
}
*/

/**
 * Adds a menu button that performs an action with no input.
 * 
 * @param hMenu         Menu to add button to.
 * @param label         What to display for this menu button.
 * @param hLinkedMenu   The menu to link this button to. 
 */
stock MenuLib_AddMenuButtonLinkMenu(Handle:hMenu, String:label[], Handle:hLinkedMenu)
{
    // Create array.
    new buttoninfo[MenuLib_Button];
    strcopy(buttoninfo[Button_Label], sizeof(buttoninfo[Button_Label]), label);
    buttoninfo[Button_Type] = ButtonType_LinkMenu;
    buttoninfo[hButton_Info] = hLinkedMenu;
    
    PushArrayArray(hMenu, buttoninfo[0]);
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
stock MenuLib_GetMenuButton(Handle:hMenu, buttonindex, buttoninfo[MenuLib_Button])
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index given to function MenuLib_GetMenuButton (menulib.inc)");
    
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
stock MenuLib_GetMenuButtonLabel(Handle:hMenu, buttonindex, String:label[], maxlen)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index given to function MenuLib_GetMenuButton (menulib.inc)");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    
    strcopy(label, maxlen, buttoninfo[Button_Label]);
}

/**
 * Get a menu button's type.
 * 
 * @param hMenu         Menu to get button from.
 * @param buttonindex   The index of the button to get data for. (Button 1 = 1, Button 2 = 2, etc)
 * 
 * @error               Invalid button.
 * @return              The type of the button.  See enum MenuLib_ButtonType.
 */
stock MenuLib_ButtonType:MenuLib_GetMenuButtonType(Handle:hMenu, buttonindex)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index given to function MenuLib_GetMenuButton (menulib.inc)");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    
    return buttoninfo[Button_Type];
}

/**
 * Get the menu linked to a menu button.
 * 
 * @param hMenu         Menu to get button from.
 * @param buttonindex   The index of the button to get data for. (Button 1 = 1, Button 2 = 2, etc)
 * 
 * @error               Invalid button.
 * @return              The handle to the menu this button is linked to.
 */
stock Handle:MenuLib_GetMenuButtonInfoHndl(Handle:hMenu, buttonindex)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index given to function MenuLib_GetMenuButton (menulib.inc)");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    
    return buttoninfo[hButton_Info];
}

/**
 * Get the <CLARIFY THIS>
 * 
 * @param hMenu         Menu to get button from.
 * @param buttonindex   The index of the button to get data for. (Button 1 = 1, Button 2 = 2, etc)
 * @param info          <CLARIFY>
 * @param maxlen        The max length of the return string. 
 * 
 * @error               Invalid button.
 */
stock MenuLib_GetMenuButtonInfoStr(Handle:hMenu, buttonindex, String:info[], maxlen)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index given to function MenuLib_GetMenuButton (menulib.inc)");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    
    strcopy(info, maxlen, buttoninfo[strButton_Info]);
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
stock MenuLib_EditMenuButton(Handle:hMenu, buttonindex, buttoninfo[MenuLib_Button])
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index given to function MenuLib_GetMenuButton (menulib.inc)");
    
    SetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
}

/**
 * Edits the label of a menu button.
 * 
 * @param hMenu         Menu containing the button we want to edit.
 * @param buttonindex   The index of the button to modify in the menu. (Button 1 = 1, Button 2 = 2, etc)
 * @param label         New menu button label.
 */
stock MenuLib_EditMenuButtonLabel(Handle:hMenu, buttonindex, String:label[])
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index given to function MenuLib_GetMenuButton (menulib.inc)");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    strcopy(buttoninfo[Button_Label], sizeof(buttoninfo[Button_Label]), label);
    SetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
}

/**
 * Edits the type of a menu button.
 * 
 * @param hMenu         Menu containing the button we want to edit.
 * @param buttonindex   The index of the button to modify in the menu. (Button 1 = 1, Button 2 = 2, etc)
 * @param buttontype    New menu button type.
 */
stock MenuLib_EditMenuButtonType(Handle:hMenu, buttonindex, MenuLib_ButtonType:buttontype)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index given to function MenuLib_GetMenuButton (menulib.inc)");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    buttoninfo[Button_Type] = buttontype;
    SetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
}

/**
 * Edits the menu this button is linked to.
 * 
 * @param hMenu         Menu containing the button we want to edit.
 * @param buttonindex   The index of the button to modify in the menu. (Button 1 = 1, Button 2 = 2, etc)
 * @param hInfo         New menu to link button to.
 */
stock MenuLib_EditMenuButtonInfoHndl(Handle:hMenu, buttonindex, Handle:hInfo)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index given to function MenuLib_GetMenuButton (menulib.inc)");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    buttoninfo[hButton_Info] = hInfo;
    SetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
}

/**
 * <CLARIFY>
 * 
 * @param hMenu         Menu containing the button we want to edit.
 * @param buttonindex   The index of the button to modify in the menu. (Button 1 = 1, Button 2 = 2, etc)
 * @param strInfo       <CLARIFY>
 */
stock MenuLib_EditMenuButtonInfoStr(Handle:hMenu, buttonindex, String:strInfo[])
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index given to function MenuLib_GetMenuButton (menulib.inc)");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    strcopy(buttoninfo[strButton_Info], sizeof(buttoninfo[strButton_Info]), strInfo);
    SetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
}

/**
 * Send a menu to a client.
 * 
 * @param hMenu     The handle of the menu to send.
 * @param client    The client to send the menu to.
 */
stock MenuLib_SendMenu(Handle:hMenu, client)
{
    // Get menu title.
    decl String:title[ML_DATA_LABEL];
    GetArrayString(hMenu, 0, title, sizeof(title));
    
    // Create menu and set title.
    new Handle:hSMMenu = CreateMenu(MenuLib_Handler);
    SetMenuTitle(hSMMenu, title);
    
    // Add menu items.
    decl String:blabel[ML_DATA_STRINFO];
    new count = GetArraySize(hMenu);
    for (new bindex = 1; bindex < count; bindex++)
    {
        MenuLib_GetMenuButtonLabel(hMenu, bindex, blabel, sizeof(blabel));
        AddMenuItem(hSMMenu, "", blabel);
    }
}

stock MenuLib_Handler(Handle:menu, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
    }
    else if (action == MenuAction_Cancel)
    {
        if (slot == MenuCancel_ExitBack)
        {
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
