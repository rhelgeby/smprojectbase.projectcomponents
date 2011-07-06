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
#define ML_DATA_ID          128
#define ML_DATA_TITLE       128
#define ML_DATA_LABEL       64
#define ML_DATA_INFO        128

/**
 * Menu data.
 */
enum MenuLib_Menu
{
    String:MenuData_Id[ML_DATA_ID],
    String:MenuData_Title[ML_DATA_TITLE],
    bool:MenuData_Translate
}

/**
 * What menu should a button open.
 */
enum MenuLib_BtnNextMenu
{
    BtnNextMenu_None,           // Don't open any menu when this button is used.
    BtnNextMenu_LinkMenu,       // Define a new menu to open.
    BtnNextMenu_LinkCurrent,    // Resend current menu.
    BtnNextMenu_LinkBack        // Send previous menu.  If there is no previous menu, it will resend the current one.
}

/**
 * Information about a single menu button.
 */
enum MenuLib_Button
{
    String:MenuBtn_Label[ML_DATA_LABEL],    // What to display for this menu button.
    String:MenuBtn_Info[ML_DATA_INFO],      // Identical to the "info" parameter in AddMenuItem, see that.
    bool:MenuBtn_Translate,                 // Instead of printing MenuBtn_Label, it will print the translation phrase it points to.
    MenuBtn_Style,                          // See SM include menus.inc ITEMDRAW_* defines.
    Function:MenuBtn_Function,              // Function to call when this button is selected.  INVALID_FUNCTION to call no function.
    MenuLib_BtnNextMenu:MenuBtn_NextMenu,   // See enum MenuLib_BtnNextMenu.
    Handle:MenuBtn_LinkedMenu               // If MenuBtn_NextMenu = BtnNextMenu_LinkMenu, this is the menu that is linked.
}

/**
 * Keeps track of the last menu the user had open.
 */
new Handle:g_hPrevMenus[MAXPLAYERS + 1];

/**
 * Keeps a cache of all current menus that are looked up by their id.
 */
new Handle:g_hMenuLibCache;

/**
 * Dummy array used as a way to count the cells required to store menu data.
 */
static stock g_DummyMenuData[MenuLib_Menu];
static stock g_DummyMenuBtnData[MenuLib_Button];

/**
 * Creates a menu ready to be filled with menu buttons.
 * 
 * @param id        The a unique string identifier for the menu. (can only be set on creation)
 * @param title     The menu title.
 * @param translate True if the title text is a translations phrase, false if just text.
 * 
 * @return          Handle to menu.  INVALID_HANDLE if the given id is already taken.
 */
stock Handle:MenuLib_CreateMenu(const String:id[], const String:title[], bool:translate)
{
    // Initialize if not already.
    if (g_hMenuLibCache == INVALID_HANDLE)
        g_hMenuLibCache = CreateTrie();
    else
    {
        // Check if this unique id is available.
        if (MenuLib_FindMenuById(id) != INVALID_HANDLE)
            return INVALID_HANDLE;
    }
    
    // Use the max value of both data structures.
    new high = sizeof(g_DummyMenuData);
    if (sizeof(g_DummyMenuBtnData) > high) high = sizeof(g_DummyMenuBtnData);
    new Handle:hMenu = CreateArray(high);
    
    // Create menu data object.
    new menudata[MenuLib_Menu];
    strcopy(menudata[MenuData_Id], sizeof(menudata[MenuData_Id]), id);
    strcopy(menudata[MenuData_Title], sizeof(menudata[MenuData_Title]), title);
    menudata[MenuData_Translate] = translate;
    
    PushArrayArray(hMenu, menudata[0]);
    SetTrieValue(g_hMenuLibCache, id, hMenu);
    return hMenu;
}

/**
 * Returns a MenuLib menu handle given it's unique id.
 * 
 * @param id    The unique id of the sought menu.
 * 
 * @return      Handle belonging to the given unique id.  INVALID_HANDLE if not found.
 */
stock Handle:MenuLib_FindMenuById(const String:id[])
{
    new Handle:hMenu;
    if (!GetTrieValue(g_hMenuLibCache, id, hMenu))
        return INVALID_HANDLE;
    return hMenu;
}

/**
 * Deletes a menu page and/or all menu pages it links to.
 * This will set each page's LinkedMenu value to INVALID_HANDLE as it kills them.
 * Warning: The root menu along with menus not in this chain may still be linked to menus that were deleted
 * in which case you will most likely get an error on the 2nd line of MenuLib_GetMenuTitle!
 * Always use this to delete these menus, don't just close the handle!
 * 
 * @param hMenu         The handle to the menu page to delete.
 * @param recursive     True to delete all menus this links to, false to just delete this menu.
 * 
 * @error   Invalid menu handle.
 */
stock MenuLib_DeleteMenu(Handle:hMenu, bool:recursive = false)
{
    if (hMenu == INVALID_HANDLE)
        ThrowError("Invalid menu handle: %x", hMenu);
    
    if (recursive)
    {
        // Have to keep track of killed handles because I can't know if it's valid..
        new Handle:hKilledHandles = CreateArray();
        MenuLib_RecursiveDelete(hMenu, hKilledHandles);
        CloseHandle(hKilledHandles);
    }
    else
        MenuLib_EradicateMenu(hMenu);
}

static stock MenuLib_RecursiveDelete(Handle:hMenu, Handle:hKilledHandles)
{
    // We killed this one already.
    if (FindValueInArray(hKilledHandles, hMenu) > -1)
        return;
    
    new buttoninfo[MenuLib_Button];
    new count = GetArraySize(hMenu);
    for (new bindex = 1; bindex < count; bindex++)
    {
        // Get all info about this menu button.
        MenuLib_GetMenuBtn(hMenu, bindex, buttoninfo);
        
        // If it links to another menu, recurse into it.
        if (buttoninfo[MenuBtn_LinkedMenu] != INVALID_HANDLE)
        {
            MenuLib_RecursiveDelete(buttoninfo[MenuBtn_LinkedMenu], hKilledHandles);
            buttoninfo[MenuBtn_LinkedMenu] = INVALID_HANDLE;
            MenuLib_EditMenuBtn(hMenu, bindex, buttoninfo);
        }
    }
    MenuLib_EradicateMenu(hMenu);
    PushArrayCell(hKilledHandles, hMenu);
}

static stock MenuLib_EradicateMenu(Handle:hMenu)
{
    // Look through all menus and unlink this menu from all to tie up loose ends.
    // ..Can't iterate tries >:|
    
    // Remove from cache/lookup table.
    decl String:id[ML_DATA_ID];
    MenuLib_GetMenuId(hMenu, id, sizeof(id));
    RemoveFromTrie(g_hMenuLibCache, id);
    
    // Destroy menu data.
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
 * @param translate     Instead of printing MenuBtn_Label, it will print the translation phrase it points to.
 * @param style         See SM include menus.inc ITEMDRAW_* defines.
 * @param funcCallback  The function to call when this button is selected.  INVALID_FUNCTION to call no function.
 * @param NextMenu      Where this menu button wants to link to.
 * @param hLinkedMenu   The handle of the menu to link this button to if NextMenu = BtnNextMenu_LinkMenu, otherwise INVALID_HANDLE.
 */
stock MenuLib_AddMenuBtnEx(Handle:hMenu, String:label[], const String:info[], bool:translate, style, Function:funcCallback, MenuLib_BtnNextMenu:NextMenu, Handle:hLinkedMenu)
{
    // Create array.
    new buttoninfo[MenuLib_Button];
    strcopy(buttoninfo[MenuBtn_Label], sizeof(buttoninfo[MenuBtn_Label]), label);
    strcopy(buttoninfo[MenuBtn_Info], sizeof(buttoninfo[MenuBtn_Info]), info);
    buttoninfo[MenuBtn_Translate] = translate;
    buttoninfo[MenuBtn_Style] = style;
    buttoninfo[MenuBtn_Function] = funcCallback;
    buttoninfo[MenuBtn_NextMenu] = NextMenu;
    buttoninfo[MenuBtn_LinkedMenu] = hLinkedMenu;
    
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
 * Get id of a menu.
 * 
 * @param hMenu     The handle of menu whose id to get.
 * @param title     The returned menu id.
 * @param maxlen    The max length of the return string.
 */
stock MenuLib_GetMenuId(Handle:hMenu, String:id[], maxlen)
{
    new menudata[MenuLib_Menu];
    GetArrayArray(hMenu, 0, menudata[0], sizeof(menudata));
    strcopy(id, maxlen, menudata[MenuData_Id]);
}

/**
 * Get title of a menu and if its translated.
 * 
 * @param hMenu     The handle of menu whose title to get.
 * @param title     The returned menu title.
 * @param maxlen    The max length of the return string.
 * @param translate True if the title text is a translation phrase.
 */
stock MenuLib_GetMenuTitle(Handle:hMenu, String:title[], maxlen, &bool:translate = false)
{
    new menudata[MenuLib_Menu];
    GetArrayArray(hMenu, 0, menudata[0], sizeof(menudata));
    strcopy(title, maxlen, menudata[MenuData_Title]);
    translate = menudata[MenuData_Translate];
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
 * Get if a menu button is translated or not.
 * 
 * @param hMenu         Menu to get button from.
 * @param buttonindex   The index of the button to get data for. (Button 1 = 1, Button 2 = 2, etc)
 * 
 * @error               Invalid button.
 * @return              True if translated, false if not.
 */
stock MenuLib_GetMenuBtnTranslate(Handle:hMenu, buttonindex)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    
    return buttoninfo[MenuBtn_Translate];
}

/**
 * Get the menu button's style.
 * 
 * @param hMenu         Menu to get button from.
 * @param buttonindex   The index of the button to get data for. (Button 1 = 1, Button 2 = 2, etc)
 * 
 * @error               Invalid button.
 * @return              The style of the button.  Compare with an ITEMDRAW_* define.
 */
stock MenuLib_GetMenuBtnStyle(Handle:hMenu, buttonindex)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    
    return buttoninfo[MenuBtn_Style];
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
 * Tells us what menu this menu button wants to link to.
 * 
 * @param hMenu         Menu to get button from.
 * @param buttonindex   The index of the button to get data for. (Button 1 = 1, Button 2 = 2, etc)
 * 
 * @error               Invalid button.
 * @return              Where the menu button wants to be linked.
 */
stock MenuLib_BtnNextMenu:MenuLib_GetMenuBtnNextMenu(Handle:hMenu, buttonindex)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    
    return buttoninfo[MenuBtn_NextMenu];
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
 * Edit title of a menu.
 * 
 * @param hMenu     The handle of menu whose title to set.
 * @param title     The new menu title.
 */
stock MenuLib_EditMenuTitle(Handle:hMenu, const String:title[])
{
    new menudata[MenuLib_Menu];
    GetArrayArray(hMenu, 0, menudata[0], sizeof(menudata));
    strcopy(menudata[MenuData_Title], sizeof(menudata[MenuData_Title]), title);
    SetArrayArray(hMenu, 0, menudata[0], sizeof(menudata));
}

/**
 * Set if a menu title is translated
 * 
 * @param hMenu     The handle of menu whose title translate to set.
 * @param translate True if title is a translation phrase, false if not.
 */
stock MenuLib_EditMenuTranslate(Handle:hMenu, bool:translate)
{
    new menudata[MenuLib_Menu];
    GetArrayArray(hMenu, 0, menudata[0], sizeof(menudata));
    menudata[MenuData_Translate] = translate;
    SetArrayArray(hMenu, 0, menudata[0], sizeof(menudata));
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
 * Edits if a button is a translated.
 * 
 * @param hMenu         Menu containing the button we want to edit.
 * @param buttonindex   The index of the button to modify in the menu. (Button 1 = 1, Button 2 = 2, etc)
 * @param style         True to translate, false to not.
 */
stock MenuLib_EditMenuBtnTranslate(Handle:hMenu, buttonindex, bool:translate)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    buttoninfo[MenuBtn_Translate] = translate;
    SetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
}

/**
 * Edits the style of the button.
 * 
 * @param hMenu         Menu containing the button we want to edit.
 * @param buttonindex   The index of the button to modify in the menu. (Button 1 = 1, Button 2 = 2, etc)
 * @param style         New style.  Use an ITEMDRAW_* define.
 */
stock MenuLib_EditMenuBtnStyle(Handle:hMenu, buttonindex, style)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    buttoninfo[MenuBtn_Style] = style;
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
 * Edits where this menu button wants to be linked to.
 * 
 * @param hMenu         Menu containing the button we want to edit.
 * @param buttonindex   The index of the button to modify in the menu. (Button 1 = 1, Button 2 = 2, etc)
 * @param NextMenu      Set where this menu button wants to link to.
 */
stock MenuLib_EditMenuBtnNextMenu(Handle:hMenu, buttonindex, MenuLib_BtnNextMenu:NextMenu)
{
    if (buttonindex <= 0 || buttonindex >= GetArraySize(hMenu))
        ThrowError("Invalid button index.");
    
    new buttoninfo[MenuLib_Button];
    GetArrayArray(hMenu, buttonindex, buttoninfo[0], sizeof(buttoninfo));
    buttoninfo[MenuBtn_NextMenu] = NextMenu;
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
 * @param hMenu             The handle of the menu to send.
 * @param client            The client to send the menu to.
 */
stock MenuLib_SendMenu(Handle:hMenu, client)
{
    // Ensure that there is a prev menu stack available.
    if (g_hPrevMenus[client] == INVALID_HANDLE)
        g_hPrevMenus[client] = CreateStack();
    
    // Get menu title.
    decl String:titletext[ML_DATA_TITLE];
    new bool:translate;
    MenuLib_GetMenuTitle(hMenu, titletext, sizeof(titletext), translate);
    
    decl String:title[ML_DATA_TITLE];
    if (translate)  Format(title, sizeof(title), "%T", titletext, client);
    else            strcopy(title, sizeof(title), titletext);
    
    // Create menu and set title.
    new Handle:hSMMenu = CreateMenu(MenuLib_Handler);
    SetMenuTitle(hSMMenu, title);
    
    // Add exit back button if applicable.
    SetMenuExitBackButton(hSMMenu, !IsStackEmpty(g_hPrevMenus[client]));
    
    // Add menu items.
    decl String:strHandle[16];
    decl String:buttontext[ML_DATA_LABEL];
    new count = GetArraySize(hMenu);
    if (count <= 1)
        AddMenuItem(hSMMenu, "", " ", ITEMDRAW_DISABLED);
    else
    {
        new buttoninfo[MenuLib_Button];
        for (new bindex = 1; bindex < count; bindex++)
        {
            GetArrayArray(hMenu, bindex, buttoninfo[0], sizeof(buttoninfo));
            IntToString(_:hMenu, strHandle, sizeof(strHandle));
            
            if (buttoninfo[MenuBtn_Translate])
                Format(buttontext, sizeof(buttontext), "%T", buttoninfo[MenuBtn_Label], client);
            else
                strcopy(buttontext, sizeof(buttontext), buttoninfo[MenuBtn_Label]);
            AddMenuItem(hSMMenu, strHandle, buttontext, buttoninfo[MenuBtn_Style]);
        }
    }
    
    DisplayMenu(hSMMenu, client, MENU_TIME_FOREVER);
}

/**
 * Re-sends the previous menu the client had.  Note that this will work even if there is no menu on their screen.
 * If there is no previous menu this function will do nothing.
 * 
 * @param client    Client to perform action on.
 */
stock MenuLib_MenuGoBack(client)
{
    // Open previous menu.
    new Handle:hPrevMenu;
    if (PopStackCell(g_hPrevMenus[client], hPrevMenu))
        MenuLib_SendMenu(hPrevMenu, client);
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
        
        new MenuLib_BtnNextMenu:NextMenu = MenuLib_GetMenuBtnNextMenu(hMenu, slot+1);
        switch(NextMenu)
        {
            case BtnNextMenu_LinkMenu:
            {
                // If the next menu is defined, send it.
                new Handle:hLinkedMenu = MenuLib_GetMenuBtnLinkedMenu(hMenu, slot+1);
                if (hLinkedMenu != INVALID_HANDLE)
                {
                    PushStackCell(g_hPrevMenus[client], hMenu);
                    MenuLib_SendMenu(hLinkedMenu, client);
                }
            }
            case BtnNextMenu_LinkCurrent:
            {
                // Resend current menu.
                MenuLib_SendMenu(hMenu, client);
            }
            case BtnNextMenu_LinkBack:
            {
                // If there are previous menus to go to, then jump back, otherwise resend this menu.
                if (g_hPrevMenus[client] != INVALID_HANDLE && !IsStackEmpty(g_hPrevMenus[client]))
                    MenuLib_MenuGoBack(client);
                else
                    MenuLib_SendMenu(hMenu, client);
            }
        }
    }
    else if (action == MenuAction_Cancel)
    {
        if (slot == MenuCancel_ExitBack)
        {
            MenuLib_MenuGoBack(client);
        }
    }
    else if (action == MenuAction_End)
    {
        CloseHandle(menu);
    }
}

/**
 * Called for each client added to the menu being generated by MenuLib_GenerateClientMenu. 
 *
 * @param client        Index of client that is about to be added to the menu.
 * @param buttontxt     The text that will be shown for this client's button.
 *                      Default is just the client's in-game name.
 * @param info          The button's information string.  Be careful modifying this, it will break MenuLib_GetClientIndex so you will have to interpret this data yourself.
 *
 * @return              Follows rules of a normal hook.  (Plugin_Stop will behave the same as Plugin_Handled)
 * 
 * Action:FuncName(client, String:buttontxt[], String:info[])
 */

/**
 * Shows a list of all clients to a client, different handlers can be used for this, as well as title.
 * 
 * @param client        Who is this being generated for?
 * @param hClientMenu   A menu handle created by MenuLib_CreateMenu to populate with list of clients currently in the server.
 * @param strCallback   Name of the function to call when a client is selected.
 * @param NextMenu      Where all buttons in this menu will link to.
 * @param hLinkedMenu   The handle of the menu to link this button to if NextMenu = BtnNextMenu_LinkMenu, otherwise INVALID_HANDLE.
 * @param filters       A bit field made up of UTILS_FILTER_* defines.  See utilities.inc.
 *                      Note: This is only checked at time of menu creation.  It is
 *                      recommended to re-check these filters in the callback.
 * @param filterFunc    Name of a function with the structure shown in the above comments.  Leave blank for no filter.
 * 
 * @error               Invalid function given for either the callback or filter function.
 */
stock Handle:MenuLib_GenerateClientMenu(client, Handle:hClientMenu, const String:strCallback[], MenuLib_BtnNextMenu:NextMenu, Handle:hLinkedMenu, filters, const String:filterFunc[] = "")
{
    // Verify callback function exists.
    new Function:funcCallback = GetFunctionByName(GetMyHandle(), strCallback);
    if (funcCallback == INVALID_FUNCTION)
        ThrowError("Invalid callback function specified: \"%s\"", strCallback);
        
    // Verify filter function exists.
    new Function:funcFilter = GetFunctionByName(GetMyHandle(), filterFunc);
    if (filterFunc[0] && funcFilter == INVALID_FUNCTION)
        ThrowError("Invalid filter function specified: \"%s\"", filterFunc);
    
    // Clear menu.
    MenuLib_RemoveAllButtons(hClientMenu);
    
    decl String:clientoption[256];
    decl String:clientuserid[256];
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
        
        // Call the filter function and return proper values.
        if (funcFilter != INVALID_FUNCTION)
        {
            if (!MenuLib_ClientMenuCallFilter(funcFilter, menuclient, clientoption, sizeof(clientoption), clientuserid, sizeof(clientuserid)))
                continue;
        }
        
        // Add option to menu.
        MenuLib_AddMenuBtnEx(hClientMenu, clientoption, clientuserid, false, ITEMDRAW_DEFAULT, funcCallback, NextMenu, hLinkedMenu);
    }
    CloseHandle(adtClients);
}

/**
 * Retrieves the client index given a menu slot from a MenuLib_ClientMenu-constructed menu.
 * Must be used from inside the menu's callback.
 * 
 * @param hMenu         The handle to the menulib menu.
 * @param buttonindex   The index of the button to modify in the menu. (Button 1 = 1, Button 2 = 2, etc)
 * 
 * @return              The client index, 0 if the selected client is no longer in the server.
 */
stock MenuLib_GetClientIndex(Handle:hMenu, buttonindex)
{
    // Get string stored in the menu slot.
    decl String:clientuserid[8];
    MenuLib_GetMenuBtnInfo(hMenu, buttonindex, clientuserid, sizeof(clientuserid));
    
    // Return the targeted client through their userid stored as a string in the menu slot.
    return GetClientOfUserId(StringToInt(clientuserid));
}

/**
 * Calls a filter function and determines appropriate values to return based on returned hook action.
 * 
 * @param funcFilter    A VALID function ID to the filter func.
 * @param client        Any cell value to pass to the filter function.
 * @param buttontxt     The filter function will possibly change this value but will only be passed back if Plugin_Changed is returned.
 * @param buttoninfo    The filter function will possibly change this value but will only be passed back if Plugin_Changed is returned.
 * 
 * @return              True if the action should be completed, false if the filter asked to block action.
 */
stock bool:MenuLib_ClientMenuCallFilter(Function:funcFilter, client, String:buttontxt[], maxlen, String:buttoninfo[], maxlen2)
{
    decl String:newbuttontxt[256];
    decl String:newbuttoninfo[256];
    strcopy(newbuttontxt, sizeof(newbuttontxt), buttontxt);
    strcopy(newbuttoninfo, sizeof(newbuttoninfo), buttoninfo);
    
    Call_StartFunction(GetMyHandle(), funcFilter);
    Call_PushCell(client);
    Call_PushString(newbuttontxt);
    Call_PushStringEx(newbuttoninfo, sizeof(newbuttoninfo), 0, SM_PARAM_COPYBACK);
    new Action:result;
    Call_Finish(result);
    
    // Interpret hook return.
    if (result == Plugin_Continue) return true;
    else if (result == Plugin_Changed)
    {
        strcopy(buttontxt, maxlen, newbuttontxt);
        strcopy(buttoninfo, maxlen2, newbuttoninfo);
        return true;
    }
    return true;
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
