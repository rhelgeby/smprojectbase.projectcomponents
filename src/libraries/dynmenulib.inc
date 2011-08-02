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
    DynMenuAction_Select,   /** User selected/changed a value. */
    DynMenuAction_Abort     /** Selection aborted (or menu was closed). */
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
    DynMenuLib_Handler:DynMenu_Handler,    /** Function to call when the user performs an action. */
    
    any:DynMenu_LowerLimit,         /** Numeric: Lowest value   | Multiselect: Min no. of entries selected | Others: No effect */
    any:DynMenu_UpperLimit,         /** Numeric: Highest value  | Multiselect: Max no. of entries selected | Others: No effect */
    
    any:DynMenu_SmallStep,          /** Numeric: Smallest change | String/Multiselect: Smallest index change */
    any:DynMenu_LargeStep,          /** Numeric: Largest change  | String/Multiselect: Largest index cange */
    
    any:DynMenu_CurrentValue,       /** Numeric: Current value | String: Current entry index | Multiselect: Handle to boolean array for selected entries. */
    DynMenu_CurrentPosition,        /** Current item position. Used for redisplaying menu at the last page. */
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

#define DYNMENULIB_LOWER_LIMIT          8
#define DYNMENULIB_UPPER_LIMIT          9

#define DYNMENULIB_SMALL_STEP           10
#define DYNMENULIB_LARGE_STEP           11

#define DYNMENULIB_CURRENT_VALUE        12
#define DYNMENULIB_CURRENT_POSITION     13
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
 * @return              Handle to dynamic menu. Close this handle when the menu
 *                      is no longer used.
 *
 * @error               Menu identifier already in use, or invalid data.
 */
stock Handle:DynMenuLib_CreateMenu(menuData[DynMenuData])
{
    /****************************
     *   Validation and fixes   *
     ****************************/
    
    // TODO: A bit ugly and repeated validation code because of multipurpose
    //       parameters. There are room for improvements, but first step is to
    //       make it work.
    
    // Validate mode and type.
    if ((menuData[DynMenu_Type] == DynMenu_Integer || menuData[DynMenu_Type] == DynMenu_Float)
        && menuData[DynMenu_Mode] == DynMenu_List)
    {
        // Numeric type supports loop mode only.
        ThrowError("Cannot use list display mode for numeric types. Numeric types only support loop mode.");
    }
    
    // Validate entries if string or multiselect.
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
                Util_SwapFloat(lower, upper);
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
                    || small < menuData[DynMenu_LowerLimit])
                {
                    ThrowError("Small step size out of bounds: %f", small);
                }
                if (large == 0
                    || large > menuData[DynMenu_UpperLimit])
                {
                    ThrowError("Large step size out of bounds: %f", large);
                }
                
                // Swap if necessarry.
                if (small > large)
                {
                    Util_SwapFloat(small, large);
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
    
    // Validate that initial value matches entry size.
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
                // Not specified. Prepare boolean list with no entries selected.
                initialValue = CreateArray(1, numEntries);
                for (new i = 0; i < numEntries; i++)
                {
                    PushArrayCell(initialValue, false);
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
 * @param initialValue  Numeric: Initial value
 *                      String: Entry index selected
 *                      Multiselect: Handle to boolean array (matching size)
 *                      with selected entries set to true. Or use INVALID_HANDLE
 *                      to select none.
 * @param fromMenu      Optional. Handle to previous menulib menu (for the
 *                      navigation stack). This menu is displayed if the user
 *                      press "back".
 *
 * @return              Handle to dynamic menu. Close this handle when the
 *                      menu is no longer used.
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
    menuData[DynMenu_MenuLib] = INVALID_HANDLE;     // No menu in menulib created yet.
    menuData[DynMenu_FromMenu] = fromMenu;
    
    menuData[DynMenu_Type] = type;
    menuData[DynMenu_Mode] = mode;
    menuData[DynMenu_Entries] = entries;
    menuData[DynMenu_Handler] = handler;
    
    menuData[DynMenu_LowerLimit] = lowerLimit;
    menuData[DynMenu_UpperLimit] = upperLimit;
    
    menuData[DynMenu_SmallStep] = smallStep;
    menuData[DynMenu_LargeStep] = largeStep;
    
    menuData[DynMenu_CurrentValue] = initialValue;      // Current value is initial value.
    menuData[DynMenu_CurrentPosition] = 0;              // Start at first page.
    
    return DynMenuLib_CreateMenu(menuData);
}

/**
 * Deletes a dynamic menu.
 *
 * @param menu          Dynamic menu handle.
 * @param recursive     Optional. Recursive deletion. Destroys entry list if
 *                      available. Default is false (don't destroy).
 */
stock DynMenuLib_DeleteMenu(Handle:menu, bool:recursive = false)
{
    // Get menu data.
    new menuData[DynMenuData];
    DynMenuLib_GetMenuData(menu, menuData);
    
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
    MenuLib_DeleteMenu(menuLibMenu);
    
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
            MenuLib_BtnWriteString(menuLibMenu, entry, MenuBtn_Label, sizeof(buttonLabel), entryBuffer);
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
    
    // Numeric: Current value | String: Current entry index | Multiselect: Handle to boolean array for selected entries.
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
        case DynMenu_String, DynMenu_Multiselect:
        {
            // Current value (entry string).
            GetArrayString(menuData[DynMenu_Entries], currentValue, strValue, sizeof(strValue));
            
            // Add check box if multiselect.
            if (menuData[DynMenu_Type] == DynMenu_Multiselect)
            {
                // Get whether the current entry is selected, and store the
                // corresponding character. When using multiselect the current value
                // is a array handle to a boolean list.
                new selectedChar = bool:GetArrayCell(menuData[DynMenu_Entries], currentValue) ? DYNMENULIB_SELECTED_CHAR : ' ';
                
                // Prefix with check box.
                Format(strValue, sizeof(strValue), "[%c] %s", selectedChar, strValue);
            }
            
            // Navigation control labels (only when building, not updating).
            if (!update)
            {
                Format(increaseLarge, sizeof(increaseLarge), "%t", "dynmenulib jump", largeStep);
                Format(increaseSmall, sizeof(increaseSmall), "%t", "dynmenulib jump", smallStep);
            }
        }
    }
    
    // Update title with prompt and current value.
    switch (menuData[DynMenu_Type])
    {
        case DynMenu_Integer, DynMenu_Float:
        {
            // Display prompt and current value.
            Format(titleBuffer, sizeof(titleBuffer), "%s\n%t %s", menuData[DynMenu_Prompt], "dynmenulib current value", strValue);
        }
        case DynMenu_String, DynMenu_Multiselect:
        {
            // Display prompt, number of selected entries and current entry.
            new selectedCount = DynMenuLib_GetSelectedCount(currentValue);
            Format(titleBuffer, sizeof(titleBuffer), "%s\n%t %s\n%t %s", menuData[DynMenu_Prompt], "dynmenulib selected", selectedCount, "dynmenulib current value", strValue);
        }
    }
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


// Callback handlers (for menulib)
// -------------------------------

/**
 * Menulib callback.
 * Called when for all MenuAction actions except voting.  See menus.inc.
 * 
 * @param menu      The menulib menu handle.
 * @param action    Action client is doing in menu.  May be any action except voting ones.  MenuAction_Select can be handled in other callbacks.
 * @param client    The client index.
 * @param slot      The menu slot selected. (starting from 0)
 */
public DynMenuLib_GeneralHandler(Handle:menu, MenuAction:action, client, slot)
{
    switch (action)
    {
        case MenuAction_Cancel, MenuAction_End:
        {
            // Forward abort action to dynmenu handler.
            
            // Get dynmenu handle.
            new Handle:dynMenu = DynMenuLib_FindMenuByMenulib(menu);
            
            // Get menu data.
            new menuData[DynMenuData];
            DynMenuLib_GetMenuData(dynMenu, menuData);
            
            // Get menu handler.
            new DynMenuLib_Handler:handler = menuData[DynMenu_Handler];
            
            // Call handler.
            Call_StartFunction(GetMyHandle(), handler);
            Call_PushCell(dynMenu);                 // menu
            Call_PushCell(client);                  // client
            Call_PushCell(_:DynMenuAction_Abort);   // action
            Call_PushCell(0);                       // value (undefined in this action)
            Call_Finish();
        }
    }
}

/**
 * Menulib callback handler for list menus.
 *
 * Called when a certain button in a menu is pressed.  The menu action is always MenuAction_Select.
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
    
    // Resolve selected entry index.
    decl String:buttonInfo[ML_DATA_INFO];
    buttonInfo[0] = 0;
    MenuLib_BtnReadString(menu, slot, MenuBtn_Info, buttonInfo, sizeof(buttonInfo));
    new entry = StringToInt(buttonInfo);
    
    // Get value (entry index for string, array handle for multiselect).
    new value = 0;
    switch (menuData[DynMenu_Type])
    {
        case DynMenu_String:
        {
            // Set value in handler.
            value = entry;
        }
        case DynMenu_Multiselect:
        {
            // Get array with selected entries.
            new Handle:selectedEntries = menuData[DynMenu_CurrentValue];
            
            // Toggle whether current entry is selected.
            new bool:selected = GetArrayCell(selectedEntries, entry);
            SetArrayCell(selectedEntries, entry, !selected);
            
            // Set value in handler.
            value = _:selectedEntries;
        }
        default:
        {
            // This should never happen. List mode only has string and
            // multiselect types. Catch it anyways to stop callback and avoid
            // invalid values.
            ThrowError("Unexpected data type for internal list handler. This is a bug!");
        }
    }
    
    // Call handler.
    Call_StartFunction(GetMyHandle(), handler);
    Call_PushCell(dynMenu);                 // menu
    Call_PushCell(client);                  // client
    Call_PushCell(_:DynMenuAction_Select);  // action
    Call_PushCell(value);                   // value (entry index or array handle)
    Call_Finish();
    
    // Update and re-display menu if using multiselect.
    if (menuData[DynMenu_Type] == DynMenu_Multiselect)
    {
        // Update menu.
        DynMenuLib_BuildListMenu(menuData, true);
        
        // Get start position.
        new start = GetMenuSelectionPosition();
        
        // Send menu again.
        new Handle:fromMenu = menuData[DynMenu_FromMenu];
        DynMenuLib_SendMenuAtItem(dynMenu, client, start, fromMenu);
    }
}

/**
 * Menulib callback handler for loop menus.
 *
 * Called when a certain button in a menu is pressed.  The menu action is always MenuAction_Select.
 * 
 * @param menu      The menulib menu handle.
 * @param client    The client index.
 * @param slot      The menu slot selected. (starting from 0)
 */
public DynMenuLib_LoopHandler(Handle:menu, client, slot)
{
    
}
