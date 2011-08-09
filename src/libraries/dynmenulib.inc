/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:          dynmenulib.inc
 *  Type:          Library
 *  Description:   A wrapper around menulib for building dynamic input menus.
 *                 Dynamic menus present some options where the user can select
 *                 one or more of them.
 *                 
 *                 It supports numerics, strings and multiselect with limits and
 *                 step values. Either in loop mode (prev/next buttons) or in
 *                 list mode.
 *                 
 *                 This library removes the need to deal with menu code 
 *                 directly. When a user select a value (or aborts), a
 *                 simplified menu handler is called with the value(s) selected.
 *
 *  Copyright (C) 2009-2011  Richard Helgeby, Greyscale
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

#if defined _dynmenulib_included
 #endinput
#endif
#define _dynmenulib_included

//#include "<project>/libraries/menulib"
//#include "<project>/libraries/utilities"
#include "libraries/menulib"
#include "libraries/utilities"

/**
 * Character that's displayed between brackets on selected entries in
 * multiselect list ("[X]" and "[ ]").
 */
#define DYNMENULIB_SELECTED_CHAR    'X'

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
functag public DynMenuLib_Handler(Handle:menu, client, DynMenuAction:action, any:value);

/**
 * Menu entry data types.
 */
enum DynMenuType
{
    DynMenu_Integer,        /** Selects a integer value (cell). */
    DynMenu_Float,          /** Selects a decimal value. */
    DynMenu_String,         /** Selects a string value. */
    DynMenu_Multiselect     /** Selects one or more string values (check boxes on entries). */
}

/**
 * Display modes for dynamic menus.
 */
enum DynMenuMode
{
    DynMenu_List,   /** Present a list of all available options. */
    DynMenu_Loop    /** Loop through options one by one via next/previous commands. */
}

/**
 * Abstract menu action for dynamic menus. This is a simplification of all
 * MenuEnd and MenuCancel reasons. Either the user selected an item, or din't.
 */
enum DynMenuAction
{
    DynMenuAction_Select,       /** User selected/changed a value. */
    DynMenuAction_Close         /** Menu was closed (simplified action for MenuCancel and MenuEnd reasons). */
}

/**
 * Actions performed in loop menus.
 *
 * Note: The order must match the order displayed in the menu because the menu
 *       item slot index is directly casted to a loop action.
 */
enum DynMenuLoopAction
{
    DynMenuLoop_IncLarge,       /** Increased large step. */
    DynMenuLoop_IncSmall,       /** Increased small step. */
    DynMenuLoop_DecSmall,       /** Decreased small step. */
    DynMenuLoop_DecLarge        /** Decreased large step. */
}

/**
 * Dynamic menu data structure.
 *
 * Note: Some attributes have different purposes depending on DynMenu_Type. See
 *       attribute comments for details.
 */
enum DynMenuData
{
    String:DynMenu_Id[ML_DATA_ID],          /** Unique menu identifier. Also used in menulib. */
    String:DynMenu_Prompt[ML_DATA_TITLE],   /** Prompt text (displayed in menu title). */
    Handle:DynMenu_MenuLib,         /** Handle to menulib menu. Initialized by DynMenuLib_CreateMenu. */
    Handle:DynMenu_FromMenu,        /** Handle to previous menulib menu, if any (optional). */
    
    DynMenuType:DynMenu_Type,       /** Entry data type. */
    DynMenuMode:DynMenu_Mode,       /** Menu display mode. */
    Handle:DynMenu_Entries,         /** String/Multiselect: ADT array of entry strings | Others: Not used (INVALID_HANDLE) */
    DynMenuLib_Handler:DynMenu_Handler,     /** Function to call when the user performs an action. */
    MenuAction:DynMenu_HandlerAction,       /** Buffer for current action being handled. Only valid in handler. */
    
    any:DynMenu_LowerLimit,         /** Numeric: Lowest value   | Multiselect: Min no. of entries selected | String: No effect */
    any:DynMenu_UpperLimit,         /** Numeric: Highest value  | Multiselect: Max no. of entries selected | String: No effect */
    
    any:DynMenu_SmallStep,          /** Loop mode only -- Numeric: Smallest change | String: Smallest index change */
    any:DynMenu_LargeStep,          /** Loop mode only -- Numeric: Largest change  | String: Largest index cange */
    
    any:DynMenu_CurrentValue,       /** Numeric: Current value | String: Current entry index | Multiselect: Handle to boolean array for selected entries. */
    DynMenu_CurrentPosition,        /** List mode: Current position in the menu. */
}

/**
 * @section Data array indexes. Must match DynMenuData.
 */
#define DYNMENULIB_ID                   0
#define DYNMENULIB_PROMPT               1
#define DYNMENULIB_MENULIB              2
#define DYNMENULIB_FROM_MENU            3

#define DYNMENULIB_TYPE                 4
#define DYNMENULIB_MODE                 5
#define DYNMENULIB_ENTRIES              6
#define DYNMENULIB_HANDLER              7
#define DYNMENULIB_HANDLER_ACTION       8

#define DYNMENULIB_LOWER_LIMIT          9
#define DYNMENULIB_UPPER_LIMIT          10

#define DYNMENULIB_SMALL_STEP           11
#define DYNMENULIB_LARGE_STEP           12

#define DYNMENULIB_CURRENT_VALUE        13
#define DYNMENULIB_CURRENT_POSITION     14
/**
 * @endsection
 */

/**
 * Trie handle for the menu name index.
 */
static stock Handle:DynMenu_NameIndex;

/**
 * Trie handle for the menulib handle to dynmenulib handle lookup.
 */
static stock Handle:DynMenu_MenulibIndex;


/******************
 *   Public API   *
 ******************/

/**
 * Creates a dynamic selection menu from a menu data structure. A menu handler
 * (DynMenuLib_Handler) is called when the user select/change a value or if
 * the menu was aborted.
 *
 * See DynMenuLib_CreateMenuEx to create a menu by specifying individual values.
 *
 * Note: The menu is immutable. Once it's created it cannot be modified, for
 *       simplicity reasons.
 *       
 * Note: It can also only be used by one client a time. If multiple clients use
 *       the same menu at the same time their current value/selections will be
 *       corrupted. Create individual menus per client.
 *
 * Note: Translations must be loaded (dynmenulib.phrases.txt) and language set
 *       before building/updating (using SetGlobalTransTarget()).
 *
 * @param menuData      Menu data structure.
 *
 * @return              Handle to dynamic menu. Do not close the handle
 *                      directly. Use DynMenuLib_DeleteMenu when the menu is no
 *                      longer used. This will close its handle.
 *
 * @error               Menu identifier already in use, or invalid data.
 */
stock Handle:DynMenuLib_CreateMenu(menuData[DynMenuData])
{
    /****************************
     *   Validation and fixes   *
     ****************************/
    
    // Validate that mode and types support eachother.
    switch (menuData[DynMenu_Mode])
    {
        case DynMenu_List:
        {
            if ((menuData[DynMenu_Type] == DynMenu_Integer || menuData[DynMenu_Type] == DynMenu_Float))
            {
                // Numeric type supports loop mode only.
                ThrowError("List mode doesn't suppoert numeric types. Use loop mode.");
            }
        }
        case DynMenu_Loop:
        {
            if (menuData[DynMenu_Type] == DynMenu_Multiselect)
            {
                ThrowError("Loop mode doesn't support multiselect type. Use list mode.");
            }
        }
    }
    
    // Validate entries if type is string or multiselect.
    new numEntries;
    if (menuData[DynMenu_Type] == DynMenu_String
        || menuData[DynMenu_Type] == DynMenu_Multiselect)
    {
        // Validate handle.
        if (menuData[DynMenu_Entries] == INVALID_HANDLE)
        {
            ThrowError("Invalid entry list handle: %x", INVALID_HANDLE);
        }
        
        // Check if empty.
        numEntries = GetArraySize(menuData[DynMenu_Entries]);
        if (numEntries == 0)
        {
            ThrowError("Entry list cannot be empty when using string/multiselect modes.");
        }
    }
    
    // Validate callback.
    if (menuData[DynMenu_Handler] == INVALID_FUNCTION)
    {
        ThrowError("Invalid function callback: %x", INVALID_FUNCTION);
    }
    
    // Validate ranges and initial value.
    switch (menuData[DynMenu_Type])
    {
        case DynMenu_Integer:
        {
            new lower = menuData[DynMenu_LowerLimit];
            new upper = menuData[DynMenu_UpperLimit];
            new initialValue = menuData[DynMenu_CurrentValue];
            
            // Swap if necessarry.
            if (lower > upper)
            {
                Util_SwapCell(lower, upper);
                menuData[DynMenu_LowerLimit] = lower;
                menuData[DynMenu_UpperLimit] = upper;
            }
            
            // Check if range is zero.
            if (upper - lower == 0)
            {
                ThrowError("Range cannot be zero.");
            }
            
            // Check if initial value is within bounds.
            if (initialValue < lower || initialValue > upper)
            {
                ThrowError("Initial value out of bounds: %d", initialValue);
            }
            
        }
        case DynMenu_Float:
        {
            new Float:lower = Float:menuData[DynMenu_LowerLimit];
            new Float:upper = Float:menuData[DynMenu_UpperLimit];
            new Float:initialValue = menuData[DynMenu_CurrentValue];
            
            // Swap if necessarry.
            if (lower > upper)
            {
                Util_SwapCell(lower, upper);
                menuData[DynMenu_LowerLimit] = lower;
                menuData[DynMenu_UpperLimit] = upper;
            }
            
            // Check if range is zero.
            if (upper - lower == 0)
            {
                ThrowError("Range cannot be zero.");
            }
            
            // Check if initial value is within bounds.
            if (initialValue < lower || initialValue > upper)
            {
                ThrowError("Initial value out of bounds: %f", initialValue);
            }
        }
        case DynMenu_Multiselect:
        {
            if (menuData[DynMenu_LowerLimit] < 0)
            {
                ThrowError("Lower limit can't be negative.");
            }
            if (menuData[DynMenu_UpperLimit] > numEntries)
            {
                ThrowError("Upper limit can't be bigger than number of entries.");
            }
        }
    }
    
    // Validate steps if loop mode.
    if (menuData[DynMenu_Mode] == DynMenu_Loop)
    {
        // Cannot be zero or outside range. Swap steps if necessarry.
    
        switch (menuData[DynMenu_Type])
        {
            case DynMenu_Integer:
            {
                new small = menuData[DynMenu_SmallStep];
                new large = menuData[DynMenu_LargeStep];
                
                // Validate step size.
                if (small == 0
                    || small < menuData[DynMenu_LowerLimit])
                {
                    ThrowError("Small step size out of bounds: %d", small);
                }
                if (large == 0
                    || large > menuData[DynMenu_UpperLimit])
                {
                    ThrowError("Large step size out of bounds: %d", large);
                }
                
                // Swap if necessarry.
                if (small > large)
                {
                    Util_SwapCell(small, large);
                    menuData[DynMenu_SmallStep] = small;
                    menuData[DynMenu_LargeStep] = large;
                }
            }
            case DynMenu_Float:
            {
                new Float:small = Float:menuData[DynMenu_SmallStep];
                new Float:large = Float:menuData[DynMenu_LargeStep];
                
                // Validate step size.
                if (small == 0
                    || small < Float:menuData[DynMenu_LowerLimit])
                {
                    ThrowError("Small step size out of bounds: %f", small);
                }
                if (large == 0
                    || large > Float:menuData[DynMenu_UpperLimit])
                {
                    ThrowError("Large step size out of bounds: %f", large);
                }
                
                // Swap if necessarry.
                if (small > large)
                {
                    Util_SwapCell(small, large);
                    menuData[DynMenu_SmallStep] = small;
                    menuData[DynMenu_LargeStep] = large;
                }
            }
            case DynMenu_String, DynMenu_Multiselect:
            {
                new small = menuData[DynMenu_SmallStep];
                new large = menuData[DynMenu_LargeStep];
                
                // Validate step size.
                if (small < 0
                    || small > numEntries)
                {
                    ThrowError("Small step size out of bounds: %d", small);
                }
                if (large < 0
                    || large > numEntries)
                {
                    ThrowError("Large step size out of bounds: %d", large);
                }
                
                // Swap if necessarry.
                if (small > large)
                {
                    Util_SwapCell(small, large);
                    menuData[DynMenu_SmallStep] = small;
                    menuData[DynMenu_LargeStep] = large;
                }
            }
        }
    }
    
    // Validate that initial value matches entry count in string and multiselect.
    switch (menuData[DynMenu_Type])
    {
        case DynMenu_String:
        {
            // Check if initial value is within bounds.
            new initialValue = menuData[DynMenu_CurrentValue];
            if (initialValue < 0 || initialValue > numEntries - 1)
            {
                ThrowError("Initial value out of bounds: %f", initialValue);
            }
        }
        case DynMenu_Multiselect:
        {
            // Initial value is a list. Check if specified.
            new Handle:initialValue = menuData[DynMenu_CurrentValue];
            if (initialValue != INVALID_HANDLE)
            {
                // Verify size.
                if (GetArraySize(initialValue) != numEntries)
                {
                    ThrowError("Initial value list size doesn't match entry list size.");
                }
            }
            else
            {
                // Not specified. Create boolean list with no entries selected.
                initialValue = CreateArray(_, numEntries);
                for (new i = 0; i < numEntries; i++)
                {
                    SetArrayCell(initialValue, i, false);
                }
                menuData[DynMenu_CurrentValue] = initialValue;
            }
        }
    }
    
    /******************
     *   Build menu   *
     ******************/
    
    // Create menu in menulib.
    new Handle:menuLibMenu = MenuLib_CreateMenu(menuData[DynMenu_Id],       // id
                                                INVALID_FUNCTION,           // pre-callback
                                                DynMenuLib_GeneralHandler,  // callback
                                                menuData[DynMenu_Prompt],   // title
                                                false,                      // translate (handled in this library)
                                                false,                      // forceback
                                                false);                     // temp
    menuData[DynMenu_MenuLib] = menuLibMenu;
    
    // Check if failed.
    if (menuLibMenu == INVALID_HANDLE)
    {
        ThrowError("Menu ID is already in use.");
    }
    
    // Add menu items.
    switch (menuData[DynMenu_Mode])
    {
        case DynMenu_List:
        {
            // Add entries as buttons.
            DynMenuLib_BuildListMenu(menuData);
        }
        case DynMenu_Loop:
        {
            // Add buttons for navigating in loop mode.
            DynMenuLib_BuildLoopMenu(menuData);
        }
    }
    
    // Get largest string size.
    new stringSize;
    #if (ML_DATA_ID > ML_DATA_TITLE)
        stringSize = ML_DATA_ID;
    #else
        stringSize = ML_DATA_TITLE;
    #endif
    
    // Create array and store dynamic menu data.
    new Handle:dynMenu = CreateArray(stringSize);
    DynMenuLib_SetMenuData(dynMenu, menuData);
    
    // Add menu handle to name index.
    DynMenuLib_InitIndexes();
    SetTrieValue(DynMenu_NameIndex, menuData[DynMenu_Id], dynMenu);
    
    // Add menu menulib handle to conversion index.
    decl String:strMenulibHandle[16];
    Format(strMenulibHandle, sizeof(strMenulibHandle), "%x", menuLibMenu);
    SetTrieValue(DynMenu_MenulibIndex, strMenulibHandle, dynMenu);
    
    // Return dyn menu handle.
    return dynMenu;
}

/**
 * Creates a dynamic selection menu. A menu handler (DynMenuLib_Handler) is
 * called when the user select/change a value or if the menu was aborted.
 *
 * Note: The menu is immutable. Once it's created it cannot be modified, for
 *       simplicity reasons.
 *       
 * Note: It can also only be used by one client a time. If multiple clients use
 *       the same menu at the same time their current value/selections will be
 *       corrupted. Create individual menus per client.
 *
 * Note: Translations must be loaded (dynmenulib.phrases.txt) and language set
 *       before building/updating (using SetGlobalTransTarget()).
 *
 * @param id            Unique menu identifier.
 * @param prompt        Prompt text (displayed in menu title).
 * @param type          Menu data type. See DynMenuType.
 * @param mode          Menu display mode. See DynMenuMode.
 * @param entries       Handle to string array with entries. Only used with
 *                      DynMenu_String or DynMenu_Multiselect as type. Otherwise
 *                      ignored (INVALID_HANDLE).
 * @param handler       Function to call when the user performs an action.
 *
 * @param lowerLimit    Lowest value or min no. of entries selected. No effect
 *                      with string type.
 * @param upperLimit    Highest value or max no. of entries selected. No effect
 *                      with string type.
 *
 * @param smallStep     Only used in loop mode (otherwise ignored). Smallest
 *                      value or entry index change. Bigger than zero.
 * @param largeStep     Only used in loop mode (otherwise ignored). Largest
 *                      value or entry index change. Bigger than zero.
 *
 * @param initialValue  Numeric: Initial value.
 *                      Multiselect: Handle to boolean array (matching size)
 *                      with selected entries set to true. The array may be
 *                      changed by dynmenulib. Or use INVALID_HANDLE to select
 *                      none.
 * @param fromMenu      Optional. Handle to previous menulib menu (for the
 *                      navigation stack). This menu is displayed if the user
 *                      press "back".
 *
 * @return              Handle to dynamic menu. Do not close the handle
 *                      directly. Use DynMenuLib_DeleteMenu when the menu is no
 *                      longer used. This will close its handle.
 *
 * @error               Menu identifier already in use, or invalid data.
 */
stock Handle:DynMenuLib_CreateMenuEx(const String:id[],
                                     const String:prompt[],
                                     DynMenuType:type,
                                     DynMenuMode:mode,
                                     Handle:entries,
                                     DynMenuLib_Handler:handler,
                                     any:lowerLimit,
                                     any:upperLimit,
                                     any:smallStep,
                                     any:largeStep,
                                     any:initialValue,
                                     Handle:fromMenu = INVALID_HANDLE)
{
    new menuData[DynMenuData];
    
    strcopy(menuData[DynMenu_Id], sizeof(menuData[DynMenu_Id]), id);
    strcopy(menuData[DynMenu_Prompt], sizeof(menuData[DynMenu_Prompt]), prompt);
    menuData[DynMenu_MenuLib] = INVALID_HANDLE;         // No menu in menulib created yet.
    menuData[DynMenu_FromMenu] = fromMenu;
    
    menuData[DynMenu_Type] = type;
    menuData[DynMenu_Mode] = mode;
    menuData[DynMenu_Entries] = entries;
    menuData[DynMenu_Handler] = handler;
    menuData[DynMenu_HandlerAction] = MenuAction:-1;    // Not initialized yet.
    
    menuData[DynMenu_LowerLimit] = lowerLimit;
    menuData[DynMenu_UpperLimit] = upperLimit;
    
    menuData[DynMenu_SmallStep] = smallStep;
    menuData[DynMenu_LargeStep] = largeStep;
    
    menuData[DynMenu_CurrentValue] = initialValue;      // Current value is initial value.
    menuData[DynMenu_CurrentPosition] = 0;              // Start at first page/entry.
    
    return DynMenuLib_CreateMenu(menuData);
}

/**
 * Deletes a dynamic menu and its belonging menulib menu.
 *
 * Note: Set inHandler to true if deleting from inside the menu's handler
 *       function.
 *
 * Note: The menu can only be deleted when not handling a DynMenuAction_Select
 *       action.
 *
 * @param menu          Dynamic menu handle.
 * @param inHandler     Optional. Whether the menu is deleted from inside its
 *                      handler function. Only valid in DynMenuAction_Close
 *                      action.
 * @param recursive     Optional. Recursive deletion. Destroys entry list if
 *                      available. Default is false (don't destroy).
 */
stock DynMenuLib_DeleteMenu(Handle:menu, bool:inHandler = false, bool:recursive = false)
{
    // Get menu data.
    new menuData[DynMenuData];
    DynMenuLib_GetMenuData(menu, menuData);
    
    // Check if menu is being deleted when handling a selection.
    if (menuData[DynMenu_HandlerAction] == MenuAction_Select)
    {
        ThrowError("Dynamic menu cannot be deleted while handling a select action.");
    }
    
    // Check if deletion is recursive.
    if (recursive)
    {
        // Destroy entry list.
        if (menuData[DynMenu_Entries] != INVALID_HANDLE)
        {
            CloseHandle(menuData[DynMenu_Entries]);
        }
    }
    
    // Delete temp array for selected entries when using multiselect.
    if (menuData[DynMenu_Type] == DynMenu_Multiselect)
    {
        CloseHandle(menuData[DynMenu_CurrentValue]);
    }
    
    // Get menulib menu and delete it.
    new Handle:menuLibMenu = menuData[DynMenu_MenuLib];
    if (inHandler)
    {
        // Menulib use menu data after the handler is done. Make it a temp menu
        // so menulib will delete it itself when safe.
        MenuLib_MenuWriteCell(menuLibMenu, MenuData_Temp, true);
    }
    else
    {
        MenuLib_DeleteMenu(menuLibMenu);
    }
    
    // Remove menu from name index.
    RemoveFromTrie(DynMenu_NameIndex, menuData[DynMenu_Id]);
    
    // Remove menu from menulib conversion index.
    decl String:strMenulibHandle[16];
    Format(strMenulibHandle, sizeof(strMenulibHandle), "%x", menuLibMenu);
    RemoveFromTrie(DynMenu_MenulibIndex, strMenulibHandle);
    
    // Destroy dynmenu data pack.
    CloseHandle(menu);
}

/**
 * Deletes a dynamic menu by identifier.
 *
 * @param id        Menu identifier.
 * @param recursive Optional. Recursive deletion. Destroys entry list if
 *                  available. Default is false (don't destroy).
 *
 * @error           Invalid identifier.
 */
stock DeleteMenuById(String:id[], bool:recursive = false)
{
    new Handle:menu = DynMenuLib_FindMenuById(id);
    
    // Validate identifier.
    if (menu == INVALID_HANDLE)
    {
        ThrowError("Invalid menu identifier: %s", id);
    }
    
    DynMenuLib_DeleteMenu(menu, recursive);
}

/**
 * Displays a dynamic menu to a client and resets the navigation stack.
 *
 * @param menu          Handle to dynamic menu.
 * @param client        Client to display menu to.
 */
stock DynMenuLib_DisplayMenu(Handle:menu, client)
{
    // Get the menulib menu handle.
    new Handle:menuLibMenu = DynMenuLib_GetMenuLib(menu);
    MenuLib_DisplayMenu(menuLibMenu, client);
}

/**
 * Displays a dynamic menu to a client without resetting the navigation stack.
 *
 * @param menu          Handle to dynamic menu.
 * @param client        Client to display menu to.
 * @param fromMenu      Optonal. Handle to menulib menu where this menu came
 *                      from. If not specified, the back button won't be visible.
 */
stock DynMenuLib_SendMenu(Handle:menu, client, Handle:fromMenu = INVALID_HANDLE)
{
    // Get the menulib menu handle.
    new Handle:menuLibMenu = DynMenuLib_GetMenuLib(menu);
    
    // Set where this menu came from, if any.
    DynMenuLib_SetFromMenu(menu, fromMenu);
    
    MenuLib_SendMenu(menuLibMenu, client, fromMenu);
}

/**
 * Gets the handle to a dynamic menu from a identifier.
 *
 * @param id    Menu identifier.
 * @return      Handle to dynamic menu, or INVALID_HANDLE if not found.
 */
stock Handle:DynMenuLib_FindMenuById(const String:id[])
{
    DynMenuLib_InitIndexes();
    
    new Handle:menu;
    
    if (!GetTrieValue(DynMenu_NameIndex, id, menu))
    {
        return INVALID_HANDLE;
    }
    
    return menu;
}

/**
 * Gets the handle to a dynamic menu from a menulib menu handle.
 *
 * @param menu  Menulib menu handle.
 * @return      Handle to dynamic menu, or INVALID_HANDLE if not found.
 */
stock Handle:DynMenuLib_FindMenuByMenulib(Handle:menu)
{
    DynMenuLib_InitIndexes();
    
    decl String:strMenulibHandle[16];
    Format(strMenulibHandle, sizeof(strMenulibHandle), "%x", menu);
    
    new Handle:dynMenu;
    
    if (!GetTrieValue(DynMenu_MenulibIndex, strMenulibHandle, dynMenu))
    {
        return INVALID_HANDLE;
    }
    
    return dynMenu;
}

/**
 * Gets the handle to a menulib menu from dynamic menu array.
 *
 * @param menu      Dynamic menu to read from.
 * 
 * @return          Handle to menulib menu.
 */
stock Handle:DynMenuLib_GetMenuLib(Handle:menu)
{
    return Handle:GetArrayCell(menu, DYNMENULIB_MENULIB);
}


/***************
 *   Private   *
 ***************/

/**
 * Initializes the trie indexes.
 */
static stock DynMenuLib_InitIndexes()
{
    if (DynMenu_NameIndex == INVALID_HANDLE)
    {
        DynMenu_NameIndex = CreateTrie();
    }
    if (DynMenu_MenulibIndex == INVALID_HANDLE)
    {
        DynMenu_MenulibIndex = CreateTrie();
    }
}

/**
 * Displays a dynamic menu to a client starting at the specified item, without
 * resetting the navigation stack.
 *
 * @param menu          Handle to dynamic menu.
 * @param client        Client to display menu to.
 * @param firstItem     First item to begin drawing from.
 * @param fromMenu      Optonal. Handle to menulib menu where this menu came
 *                      from. If not specified, the back button won't be visible.
 */
static stock DynMenuLib_SendMenuAtItem(Handle:menu, client, firstItem, Handle:fromMenu = INVALID_HANDLE)
{
    // Get the menulib menu handle.
    new Handle:menuLibMenu = DynMenuLib_GetMenuLib(menu);
    
    // Update where this menu came from.
    DynMenuLib_SetFromMenu(menu, fromMenu);
    
    MenuLib_SendMenuAtItem(menuLibMenu, client, firstItem, fromMenu);
}


// Get/set functions for menu data
// -------------------------------

/**
 * Extracts the menu data structure from the array.
 *
 * @param menu      Handle to dynamic menu.
 * @param menuData  Output. Destination buffer.
 */
static stock DynMenuLib_GetMenuData(Handle:menu, menuData[DynMenuData])
{
    GetArrayString(menu, DYNMENULIB_ID, menuData[DynMenu_Id], ML_DATA_ID);
    GetArrayString(menu, DYNMENULIB_PROMPT, menuData[DynMenu_Prompt], ML_DATA_TITLE);
    menuData[DynMenu_MenuLib] = Handle:GetArrayCell(menu, DYNMENULIB_MENULIB);
    menuData[DynMenu_FromMenu] = Handle:GetArrayCell(menu, DYNMENULIB_FROM_MENU);
    
    menuData[DynMenu_Type] = DynMenuType:GetArrayCell(menu, DYNMENULIB_TYPE);
    menuData[DynMenu_Mode] = DynMenuMode:GetArrayCell(menu, DYNMENULIB_MODE);
    menuData[DynMenu_Entries] = Handle:GetArrayCell(menu, DYNMENULIB_ENTRIES);
    menuData[DynMenu_Handler] = DynMenuLib_Handler:GetArrayCell(menu, DYNMENULIB_HANDLER);
    menuData[DynMenu_HandlerAction] = MenuAction:GetArrayCell(menu, DYNMENULIB_HANDLER_ACTION);
    
    menuData[DynMenu_LowerLimit] = GetArrayCell(menu, DYNMENULIB_LOWER_LIMIT);
    menuData[DynMenu_UpperLimit] = GetArrayCell(menu, DYNMENULIB_UPPER_LIMIT);
    
    menuData[DynMenu_SmallStep] = GetArrayCell(menu, DYNMENULIB_SMALL_STEP);
    menuData[DynMenu_LargeStep] = GetArrayCell(menu, DYNMENULIB_LARGE_STEP);
    
    menuData[DynMenu_CurrentValue] = GetArrayCell(menu, DYNMENULIB_CURRENT_VALUE);
    menuData[DynMenu_CurrentPosition] = GetArrayCell(menu, DYNMENULIB_CURRENT_POSITION);
}

/**
 * Sets the menu data structure in a array.
 *
 * @param menu      Handle to dynamic menu.
 * @param menuData  Output. Destination buffer.
 */
static stock DynMenuLib_SetMenuData(Handle:menu, menuData[DynMenuData])
{
    // Delete old data.
    ClearArray(menu);
    
    // Note: Must be in the same order as in enum DynMenuData.
    
    PushArrayString(menu, menuData[DynMenu_Id]);
    PushArrayString(menu, menuData[DynMenu_Prompt]);
    PushArrayCell(menu, _:menuData[DynMenu_MenuLib]);
    PushArrayCell(menu, _:menuData[DynMenu_FromMenu]);
    
    PushArrayCell(menu, _:menuData[DynMenu_Type]);
    PushArrayCell(menu, _:menuData[DynMenu_Mode]);
    PushArrayCell(menu, _:menuData[DynMenu_Entries]);
    PushArrayCell(menu, _:menuData[DynMenu_Handler]);
    PushArrayCell(menu, _:menuData[DynMenu_HandlerAction]);
    
    PushArrayCell(menu, _:menuData[DynMenu_LowerLimit]);
    PushArrayCell(menu, _:menuData[DynMenu_UpperLimit]);
    
    PushArrayCell(menu, _:menuData[DynMenu_SmallStep]);
    PushArrayCell(menu, _:menuData[DynMenu_LargeStep]);
    
    PushArrayCell(menu, _:menuData[DynMenu_CurrentValue]);
    PushArrayCell(menu, _:menuData[DynMenu_CurrentPosition]);
}

/**
 * Gets the menu handler from dynamic menu array.
 *
 * @param menu      Dynamic menu to read from.
 * 
 * @return          Dynamic menu handler.
 */
static stock DynMenuLib_Handler:DynMenuLib_GetHandler(Handle:menu)
{
    return DynMenuLib_Handler:GetArrayCell(menu, DYNMENULIB_HANDLER);
}

/**
 * Sets (or unsets) where a menu came from.
 *
 * @param menu      Dynamic menu.
 * @param fromMenu  Handle to menulib menu, or INVALID_HANDLE to unset.
 */
static stock DynMenuLib_SetFromMenu(Handle:menu, Handle:fromMenu)
{
    SetArrayCell(menu, DYNMENULIB_FROM_MENU, _:fromMenu);
}

/**
 * Sets current value.
 *
 * @param menu      Dynamic menu.
 * @param value     New value to set.
 */
static stock DynMenuLib_SetCurrentValue(Handle:menu, any:value)
{
    SetArrayCell(menu, DYNMENULIB_CURRENT_VALUE, value);
}

/**
 * Sets (or unsets) the current menu action being handled.
 *
 * @param menu      Dynamic menu.
 * @param action    Action to set, or -1 to unset.
 */
static stock DynMenuLib_SetHandlerAction(Handle:menu, MenuAction:action)
{
    SetArrayCell(menu, DYNMENULIB_HANDLER_ACTION, _:action);
}


// Menu builder helpers
// --------------------

/**
 * Adds entries to the menu.
 *
 * @param menuData      Dynamic menu data.
 * @param update        Optional. Update instead of building the menu. Default
 *                      is false.
 */
static stock DynMenuLib_BuildListMenu(menuData[DynMenuData], bool:update = false)
{
    new Handle:menuLibMenu = menuData[DynMenu_MenuLib];
    new currentValue = menuData[DynMenu_CurrentValue];  // It's a array handle if using multiselect.
    
    decl String:entryBuffer[ML_DATA_LABEL];
    decl String:buttonLabel[ML_DATA_LABEL];
    decl String:buttonInfo[ML_DATA_INFO];
    entryBuffer[0] = 0;
    buttonLabel[0] = 0;
    buttonInfo[0] = 0;
    
    // Loop through entries, prepare button data and update/add to menu.
    new Handle:entries = menuData[DynMenu_Entries];
    new numEntries = GetArraySize(entries);
    for (new entry = 0; entry < numEntries; entry++)
    {
        // Get entry string.
        GetArrayString(entries, entry, entryBuffer, sizeof(entryBuffer));
        
        // Prefix with check box if using multiselect.
        if (menuData[DynMenu_Type] == DynMenu_Multiselect)
        {
            // Get whether the current entry is selected, and store the
            // corresponding character. When using multiselect the current value
            // is a array handle to a boolean list.
            new selectedChar = bool:GetArrayCell(Handle:currentValue, entry) ? DYNMENULIB_SELECTED_CHAR : ' ';
            
            Format(buttonLabel, sizeof(buttonLabel), "[%c] %s", selectedChar, entryBuffer);
        }
        else
        {
            // Don't prefix.
            strcopy(buttonLabel, sizeof(buttonLabel), entryBuffer);
        }
        
        // Update or add button.
        if (update)
        {
            // Update button.
            MenuLib_BtnWriteString(menuLibMenu, entry, MenuBtn_Label, sizeof(buttonLabel), buttonLabel);
        }
        else
        {
            // Prepare button info with entry index.
            Format(buttonInfo, sizeof(buttonInfo), "%d", entry);
            
            // Add button.
            MenuLib_AddMenuBtnEx(menuLibMenu,               // menu
                                 buttonLabel,               // label
                                 buttonInfo,                // info
                                 false,                     // translate
                                 ITEMDRAW_DEFAULT,          // style
                                 DynMenuLib_ListHandler,    // callback
                                 BtnNextMenu_None,          // next menu
                                 "");                       // link id
        }
    }
}

/**
 * Builds or updates an existing loop menu.
 *
 * Note: Translations must be loaded (dynmenulib.phrases.txt) and language set
 *       before building/updating (using SetGlobalTransTarget()).
 *
 * @param menuData      Dynamic menu data.
 * @param update        Optional. Update instead of building the menu. Default
 *                      is false.
 */
static stock DynMenuLib_BuildLoopMenu(menuData[DynMenuData], bool:update = false)
{
    new Handle:menuLibMenu = menuData[DynMenu_MenuLib];
    
    // Numeric: Current value | String: Current entry index
    new any:currentValue = menuData[DynMenu_CurrentValue];
    
    new any:largeStep = menuData[DynMenu_LargeStep];
    new any:smallStep = menuData[DynMenu_SmallStep];
    
    // Title and current value.
    decl String:titleBuffer[ML_DATA_TITLE];
    decl String:strValue[ML_DATA_LABEL];
    titleBuffer[0] = 0;
    strValue[0] = 0;
    
    decl String:buttonLabel[ML_DATA_LABEL];
    decl String:buttonInfo[ML_DATA_INFO];
    buttonLabel[0] = 0;
    buttonInfo[0] = 0;
    
    // Control labels.
    decl String:increaseLarge[ML_DATA_LABEL];
    decl String:increaseSmall[ML_DATA_LABEL];
    decl String:decreaseLarge[ML_DATA_LABEL];
    decl String:decreaseSmall[ML_DATA_LABEL];
    increaseLarge[0] = 0;
    increaseSmall[0] = 0;
    decreaseLarge[0] = 0;
    decreaseSmall[0] = 0;
    
    // Prepare strings.
    switch (menuData[DynMenu_Type])
    {
        case DynMenu_Integer:
        {
            // Current value.
            IntToString(currentValue, strValue, sizeof(strValue));
            
            // Navigation control labels (only when building, not updating).
            if (!update)
            {
                Format(increaseLarge, sizeof(increaseLarge), "%t", "dynmenulib increase int", largeStep);
                Format(increaseSmall, sizeof(increaseSmall), "%t", "dynmenulib increase int", smallStep);
                Format(decreaseLarge, sizeof(decreaseLarge), "%t", "dynmenulib decrease int", largeStep);
                Format(decreaseSmall, sizeof(decreaseSmall), "%t", "dynmenulib decrease int", smallStep);
            }
        }
        case DynMenu_Float:
        {
            // Current value.
            FloatToString(Float:currentValue, strValue, sizeof(strValue));
            
            // Navigation control labels (only when building, not updating).
            if (!update)
            {
                Format(increaseLarge, sizeof(increaseLarge), "%t", "dynmenulib increase float", largeStep);
                Format(increaseSmall, sizeof(increaseSmall), "%t", "dynmenulib increase float", smallStep);
                Format(decreaseLarge, sizeof(decreaseLarge), "%t", "dynmenulib decrease float", largeStep);
                Format(decreaseSmall, sizeof(decreaseSmall), "%t", "dynmenulib decrease float", smallStep);
            }
        }
        case DynMenu_String:
        {
            // Get current entry viewing.
            new entry = menuData[DynMenu_CurrentPosition];
            
            // Get entry string value.
            GetArrayString(menuData[DynMenu_Entries], entry, strValue, sizeof(strValue));
            
            // Navigation control labels (only set when building, not updating).
            if (!update)
            {
                Format(increaseLarge, sizeof(increaseLarge), "%t", "dynmenulib next n", largeStep);
                Format(increaseSmall, sizeof(increaseSmall), "%t", "dynmenulib next");
                Format(decreaseSmall, sizeof(decreaseSmall), "%t", "dynmenulib previous");
                Format(decreaseLarge, sizeof(decreaseLarge), "%t", "dynmenulib previous n", largeStep);
            }
        }
    }
    
    // Update title with prompt and current value.
    Format(titleBuffer, sizeof(titleBuffer), "%s\n%t %s", menuData[DynMenu_Prompt], "dynmenulib current value", strValue);
    MenuLib_MenuWriteString(menuLibMenu, MenuData_Title, ML_DATA_TITLE, titleBuffer);
    
    // Add buttons. No update needed, control labels doesn't change.
    if (!update)
    {
        // Add button.
        MenuLib_AddMenuBtnEx(menuLibMenu, increaseLarge, "increase large", false, ITEMDRAW_DEFAULT, DynMenuLib_LoopHandler, BtnNextMenu_None, "");
        MenuLib_AddMenuBtnEx(menuLibMenu, increaseSmall, "increase small", false, ITEMDRAW_DEFAULT, DynMenuLib_LoopHandler, BtnNextMenu_None, "");
        MenuLib_AddMenuBtnEx(menuLibMenu, decreaseSmall, "decrease small", false, ITEMDRAW_DEFAULT, DynMenuLib_LoopHandler, BtnNextMenu_None, "");
        MenuLib_AddMenuBtnEx(menuLibMenu, decreaseLarge, "decrease large", false, ITEMDRAW_DEFAULT, DynMenuLib_LoopHandler, BtnNextMenu_None, "");
    }
}

/**
 * Returns number of selected entries.
 *
 * @param selectedEntries   Handle to boolean array for selected entries.
 *
 * @return                  Number of selected entries.
 */
static stock DynMenuLib_GetSelectedCount(Handle:selectedEntries)
{
    new size = GetArraySize(selectedEntries);
    new bool:selected;
    new count = 0;
    
    for (new entry = 0; entry < size; entry++)
    {
        selected = bool:GetArrayCell(selectedEntries, entry);
        if (selected)
        {
            count++;
        }
    }
    
    return count;
}


/**
 * Tests whether a selection/deselection in a multiselect menu will go out of
 * bounds.
 *
 * Note: This function assumes that the specified entry will be selected or
 *       deselected, depending on it's current state. Only call this function
 *       before toggling the selected state.
 *
 * @param menuData      Dynamic menu data.
 * @param selected      Current entry's selected state.
 *
 * @return              0 if inside bounds,
 *                      -1 if outside lower bound, or 
 *                      1 if outside upper bound.
 */
static stock DynMenuLib_MultiselBoundsCheck(menuData[DynMenuData], bool:selected)
{
    new selectedCount = DynMenuLib_GetSelectedCount(menuData[DynMenu_CurrentValue]);
    
    if (selected)
    {
        // Entry is being deselected. Check min bounds.
        if (selectedCount - 1 < menuData[DynMenu_LowerLimit])
        {
            // Lower bound reached.
            return -1;
        }
    }
    else
    {
        // Entry is being selected. Check max bounds.
        if (selectedCount + 1 > menuData[DynMenu_UpperLimit])
        {
            // Upper bound reached.
            return 1;
        }
    }
    
    // Inside bounds.
    return 0;
}

/**
 * Handles the multiselect bounds testing and displays error messages if outside
 * bounds.
 *
 * The selected entry's state is toggled if bounds test was passed.
 *
 * @param client    Client to display message to.
 * @param menuData  Dynamic menu data.
 * @param entry     Current entry.
 *
 * @return          True if passed bounds test, false otherwise.
 */
static stock bool:DynMenuLib_MultiselectHandler(client, menuData[DynMenuData], entry)
{
    // Get array with selected entries.
    new Handle:selectedEntries = menuData[DynMenu_CurrentValue];
    
    // Get whether current entry is selected.
    new bool:selected = GetArrayCell(selectedEntries, entry);
    
    SetGlobalTransTarget(client);
    decl String:errMsg[128];
    errMsg[0] = 0;
    
    // Check bounds. Print error message if reached.
    new bounds = DynMenuLib_MultiselBoundsCheck(menuData, selected);
    if (bounds > 0)
    {
        // Upper bound reached.
        Format(errMsg, sizeof(errMsg), "%t", "dynmenulib upper bound select");
        PrintToChat(client, errMsg);
    }
    else if (bounds < 0)
    {
        // Lower bound reached.
        Format(errMsg, sizeof(errMsg), "%t", "dynmenulib lower bound select");
        PrintToChat(client, errMsg);
    }
    else
    {
        // Toggle selected entry.
        SetArrayCell(selectedEntries, entry, !selected);
    }
    
    return (bounds == 0);
}

/**
 * Tests wheter changing a value a certain amount will go outside bounds.
 *
 * @param menuData      Dynamic menu data.
 * @param value         New value to check.
 * @param floatType     Optional. Treat values and limits as float values.
 *                      Default is false.
 *
 * @return              0 if inside bounds,
 *                      -1 if outside lower bound, or 
 *                      1 if outside upper bound.
 */
static stock DynMenuLib_NumericBoundsCheck(menuData[DynMenuData], any:value, bool:floatType = false)
{
    if (floatType)
    {
        if (Float:value < Float:menuData[DynMenu_LowerLimit])
        {
            // Lower bound reached.
            return -1;
        }
        if (Float:value > Float:menuData[DynMenu_UpperLimit])
        {
            // Upper bound reached.
            return 1;
        }
    }
    else
    {
        if (value < 0 &&
            value < menuData[DynMenu_LowerLimit])
        {
            // Lower bound reached.
            return -1;
        }
        if (value > 0 &&
                 value > menuData[DynMenu_UpperLimit])
        {
            // Upper bound reached.
            return 1;
        }
    }
    
    // Inside bounds.
    return 0;
}

/**
 * Calculates a step value to add to the current value, according to the
 * specified loop action.
 *
 * @param menuData      Dynamic menu data.
 * @param loopAction    Loop action performed.
 * @param floatType     Treat value as a float value.
 *
 * @return              Step value.
 */
static stock any:DynMenuLib_GetStep(menuData[DynMenuData], DynMenuLoopAction:loopAction, bool:floatType)
{
    switch (loopAction)
    {
        case DynMenuLoop_DecLarge:
        {
            if (floatType)
            {
                return 0.0 - Float:menuData[DynMenu_LargeStep];
            }
            else
            {
                return -menuData[DynMenu_LargeStep];
            }
        }
        case DynMenuLoop_DecSmall:
        {
            if (floatType)
            {
                return 0.0 - Float:menuData[DynMenu_SmallStep];
            }
            else
            {
                return -menuData[DynMenu_SmallStep];
            }
        }
        case DynMenuLoop_IncSmall:
        {
            return menuData[DynMenu_SmallStep];
        }
        case DynMenuLoop_IncLarge:
        {
            return menuData[DynMenu_LargeStep];
        }
    }
    
    // Should never reach this line.
    return 0;
}


// Callback handlers (for menulib)
// -------------------------------

/**
 * Menulib callback.
 * Called for all MenuAction actions except voting.  See menus.inc.
 * 
 * @param menu      The menulib menu handle.
 * @param action    Action client is doing in menu.  May be any action except
 *                  voting ones.  MenuAction_Select can be handled in other
 *                  callbacks.
 * @param client    The client index.
 * @param slot      The menu slot selected. (starting from 0)
 */
public DynMenuLib_GeneralHandler(Handle:menu, MenuAction:action, client, slot)
{
    // Note: Calling the dynmenu handler when action is MenuAction_Cancel isn't
    //       necessarry because MenuAction_End is guaranteed to be called.
    //       Otherwise it would result in a double call when pressing "back"
    //       or "exit" buttons.
    
    new Handle:dynMenu;
    
    switch (action)
    {
        case MenuAction_Cancel:
        {
            // Get dynmenu handle.
            dynMenu = DynMenuLib_FindMenuByMenulib(menu);
            
            // Unset current action. This terminates a loop menu or multiselect
            // session so the menu won't be redisplayed in end action.
            DynMenuLib_SetHandlerAction(dynMenu, MenuAction:-1);
        }
        case MenuAction_End:
        {
            // Forward abort action to dynmenu handler.
            
            // Get dynmenu handle.
            dynMenu = DynMenuLib_FindMenuByMenulib(menu);
            
            // Get menu data.
            new menuData[DynMenuData];
            DynMenuLib_GetMenuData(dynMenu, menuData);
            
            // Check if a loop menu or a multiselect session is active.
            if ((menuData[DynMenu_Type] == DynMenu_Multiselect
                || menuData[DynMenu_Mode] == DynMenu_Loop)
                && menuData[DynMenu_HandlerAction] == MenuAction_Select)
            {
                // Still active, menu is being redisplayed. Don't send abort
                // action.
                return;
            }
            
            // Get menu handler.
            new DynMenuLib_Handler:handler = menuData[DynMenu_Handler];
            
            // Set current action.
            DynMenuLib_SetHandlerAction(dynMenu, action);
            
            // Call handler.
            Call_StartFunction(GetMyHandle(), handler);
            Call_PushCell(dynMenu);                 // menu
            Call_PushCell(client);                  // client
            Call_PushCell(_:DynMenuAction_Close);   // action
            Call_PushCell(0);                       // value (undefined in this action)
            Call_Finish();
            
            // Update dynmenu handle in case menu was deleted in callback above.
            dynMenu = DynMenuLib_FindMenuByMenulib(menu);
            
            // Check if the menu still exists.
            if (dynMenu != INVALID_HANDLE)
            {
                // Unset current action.
                DynMenuLib_SetHandlerAction(dynMenu, MenuAction:-1);
            }
        }
    }
}

/**
 * Menulib callback handler for list menus.
 *
 * Called when a certain button in a menu is pressed.  The menu action is always
 * MenuAction_Select.
 * 
 * @param menu      The menulib menu handle.
 * @param client    The client index.
 * @param slot      The menu slot selected. (starting from 0)
 */
public DynMenuLib_ListHandler(Handle:menu, client, slot)
{
    // Get dynamic menu.
    new Handle:dynMenu = DynMenuLib_FindMenuByMenulib(menu);
    
    // Get menu data.
    new menuData[DynMenuData];
    DynMenuLib_GetMenuData(dynMenu, menuData);
    
    // Get menu handler.
    new DynMenuLib_Handler:handler = menuData[DynMenu_Handler];
    new bool:callHandler = true;
    
    // Resolve selected entry index.
    decl String:buttonInfo[ML_DATA_INFO];
    buttonInfo[0] = 0;
    MenuLib_BtnReadString(menu, slot, MenuBtn_Info, buttonInfo, sizeof(buttonInfo));
    new entry = StringToInt(buttonInfo);
    
    // Get value (entry index for string, array handle for multiselect).
    new any:value = 0;
    switch (menuData[DynMenu_Type])
    {
        case DynMenu_String:
        {
            // Set value in handler.
            value = entry;
        }
        case DynMenu_Multiselect:
        {
            // Handles bound testing, error messages and update of entry state.
            callHandler = DynMenuLib_MultiselectHandler(client, menuData, entry);
            value = menuData[DynMenu_CurrentValue];
        }
        default:
        {
            // This should never happen. List mode only has string and
            // multiselect types. Catch it anyways to stop callback and avoid
            // invalid values.
            ThrowError("Unexpected data type for internal list handler. This is a bug!");
        }
    }
    
    // Set current action. Used by DynMenuLib_GeneralHandler to find out when
    // abort action should be fired.
    DynMenuLib_SetHandlerAction(dynMenu, MenuAction_Select);
    
    // Call handler if selection was allowed.
    if (callHandler)
    {
        // Call handler.
        Call_StartFunction(GetMyHandle(), handler);
        Call_PushCell(dynMenu);                 // menu
        Call_PushCell(client);                  // client
        Call_PushCell(_:DynMenuAction_Select);  // action
        Call_PushCell(value);                   // value (entry index or array handle)
        Call_Finish();
    }
    
    // Update and re-display menu if using multiselect.
    if (menuData[DynMenu_Type] == DynMenu_Multiselect)
    {
        // Update menu.
        DynMenuLib_BuildListMenu(menuData, true);
        
        // Get start position.
        new start = GetMenuSelectionPosition();
        
        // Send menu again.
        DynMenuLib_SendMenuAtItem(dynMenu, client, start, menuData[DynMenu_FromMenu]);
    }
    
    // (Note: Don't unset current handler action here. It's done in end action.)
}

/**
 * Menulib callback handler for loop menus.
 *
 * Called when a certain button in a menu is pressed.  The menu action is always
 * MenuAction_Select.
 * 
 * @param menu      The menulib menu handle.
 * @param client    The client index.
 * @param slot      The menu slot selected. (starting from 0)
 */
public DynMenuLib_LoopHandler(Handle:menu, client, slot)
{
    // Get dynamic menu.
    new Handle:dynMenu = DynMenuLib_FindMenuByMenulib(menu);
    
    // Get menu data.
    new menuData[DynMenuData];
    DynMenuLib_GetMenuData(dynMenu, menuData);
    
    // Get menu handler.
    new DynMenuLib_Handler:handler = menuData[DynMenu_Handler];
    new bool:callHandler = true;
    
    // Get current value.
    new any:value = menuData[DynMenu_CurrentValue];
    
    // Get whether data type is float.
    new bool:floatType = (menuData[DynMenu_Type] == DynMenu_Float);
    
    // Get loop action. The slot will never be a exit/back button in this
    // handler, so it's safe to cast direclty.
    new DynMenuLoopAction:loopAction = DynMenuLoopAction:slot;
    
    // Get step amount.
    new any:step = DynMenuLib_GetStep(menuData, loopAction, floatType);
    
    // Calculate new value.
    switch (menuData[DynMenu_Type])
    {
        case DynMenu_Integer, DynMenu_String:
        {
            value += step;
        }
        case DynMenu_Float:
        {
            value = Float:value + Float:step;
        }
    }
    
    SetGlobalTransTarget(client);
    decl String:errMsg[128];
    errMsg[0] = 0;
    
    // Check bounds and update value.
    switch (menuData[DynMenu_Type])
    {
        case DynMenu_Integer, DynMenu_Float:
        {
            new bounds = DynMenuLib_NumericBoundsCheck(menuData, value, floatType);
            if (bounds == 1)
            {
                // Upper bound reached.
                Format(errMsg, sizeof(errMsg), "%t", "dynmenulib upper bound numeric");
                PrintToChat(client, errMsg);

                // TODO: Update value if changed to upper limit.
                callHandler = false;
            }
            else if (bounds == -1)
            {
                // Lower bound reached.
                Format(errMsg, sizeof(errMsg), "%t", "dynmenulib lower bound numeric");
                PrintToChat(client, errMsg);
                
                // TODO: Update value if changed to lower limit.
                callHandler = false;
            }
            else
            {
                // Inside bounds. Update value.
                menuData[DynMenu_CurrentValue] = value;
                DynMenuLib_SetCurrentValue(dynMenu, value);
            }
        }
        case DynMenu_String:
        {
            // String type doesn't have bounds check. Update value.
            menuData[DynMenu_CurrentValue] = value;
            DynMenuLib_SetCurrentValue(dynMenu, value);
        }
    }
    
    // Set current action. Used by DynMenuLib_GeneralHandler to find out when
    // abort action should be fired.
    DynMenuLib_SetHandlerAction(dynMenu, MenuAction_Select);
    
    // Only call handler if a value was changed.
    if (callHandler)
    {
        // Call handler.
        Call_StartFunction(GetMyHandle(), handler);
        Call_PushCell(dynMenu);                 // menu
        Call_PushCell(client);                  // client
        Call_PushCell(_:DynMenuAction_Select);  // action
        Call_PushCell(value);                   // value
        Call_Finish();
        
        // Update view position if using string type.
        if (menuData[DynMenu_Type] == DynMenu_String)
        {
            menuData[DynMenu_CurrentPosition] += step;
        }
    }
    
    // Update and re-display loop menu.
    DynMenuLib_BuildLoopMenu(menuData, true);
    DynMenuLib_SendMenu(dynMenu, client, menuData[DynMenu_FromMenu]);
}
