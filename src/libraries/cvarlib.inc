/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:          cvarlib.inc
 *  Type:          Library
 *  Description:   Cvar-related API.
 *
 *  Copyright (C) 2009-2011  Greyscale, Richard Helgeby
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

#if defined _cvarlib_included
 #endinput
#endif
#define _cvarlib_included

/**
 * Handle to trie with locked state for cvars that are watched.
 */
static Handle:CvarLib_IsChanging = INVALID_HANDLE;


/**************
 *   Public   *
 **************/

/**
 * Sets a cvar to the given value and then locks it.
 * 
 * @param cvar      The cvar to lock.
 * @param value     The value to lock cvar as.
 * @param replace   Silently replace existing lock, if any.
 *
 * @error           Invalid or corrupt handle.
 * @error           If the cvar is already locked. Set replace to true to ignore
 *                  this.
 */
stock CvarLib_LockCvar(const String:cvar[], const String:value[], bool:replace = false)
{
    // Create trie if not created.
    CvarLib_InitTrie();
    
    new Handle:hCvar = FindConVar(cvar);
    
    // Check if valid.
    if (hCvar == INVALID_HANDLE)
    {
        ThrowError("Invalid cvar name: %s", cvar);
    }
    
    decl String:key[32];
    CvarLib_GetKeyName(hCvar, key, sizeof(key));
    
    // Check if it's already locked.
    if (CvarLib_IsCvarLockedEx(hCvar))
    {
        if (replace)
        {
            // Silently replace it. Toggle cvar state so it will be ignored by
            // the existing lock.
            CvarLib_ToggleState(key);
            SetConVarString(hCvar, value);
            
            // That's it.
            return;
        }
        else
        {
            // Already locked and not replacing.
            ThrowError("Cvar %s is already locked. Set \"replace\" to true to ignore this.", cvar);
        }
    }
    
    // Add to trie. (Using cvar handle in hex as key string.)
    SetTrieValue(CvarLib_IsChanging, key, false);   // The cvar is not changing.
    
    // Set new value and lock it.
    SetConVarString(hCvar, value);
    HookConVarChange(hCvar, CvarLib_CvarLock);
}

/**
 * Unlocks a locked cvar.
 * 
 * @param cvar      The cvar to lock.
 *
 * @error           Invalid or corrupt handle.
 */
stock CvarLib_UnlockCvar(const String:cvar[])
{
    new Handle:hCvar = FindConVar(cvar);
    
    // Check if valid.
    if (hCvar == INVALID_HANDLE)
    {
        ThrowError("Invalid cvar name: %s", cvar);
    }
    
    UnhookConVarChange(hCvar, CvarLib_CvarLock);
    
    // Remove state from trie.
    decl String:key[32];
    CvarLib_GetKeyName(hCvar, key, sizeof(key));
    RemoveFromTrie(CvarLib_IsChanging, key);
    
    // Remove trie if empty.
    CvarLib_CleanTrie();
}

/**
 * Returns whether a cvar is locked or not.
 *
 * @param cvar      Name of cvar to check.
 *
 * @return          True if locked, false otherwise.
 */
stock CvarLib_IsCvarLocked(const String:cvar[])
{
    new Handle:hCvar = FindConVar(cvar);
    decl String:key[32];
    CvarLib_GetKeyName(hCvar, key, sizeof(key));
    
    new dummy;
    return GetTrieValue(CvarLib_IsChanging, key, dummy);
}


/***************
 *   Private   *
 ***************/

/**
 * Called when a console variable's value is changed.
 *
 * Note:    This is a recursive call; when the user change the cvar, and
 *          when this code reset it to the old value. The isChanging tells
 *          whether it's the user that's changing or not, so the recursion is
 *          stopped.
 * 
 * @param convar		Handle to the convar that was changed.
 * @param oldValue		String containing the value of the convar before it was changed.
 * @param newValue		String containing the new value of the convar.
 */
public CvarLib_CvarLock(Handle:convar, const String:oldValue[], const String:newValue[])
{
    // Swap state.
    decl String:key[32];
    Format(key, sizeof(key), "%x", convar);
    
    // Whether the cvar must be reset or not.
    new bool:isChanging = CvarLib_ToggleState(key);
    
    decl String:name[255];
    GetConVarName(convar, name, sizeof(name));
    
    // Only reset value when the user is changing it.
    if (isChanging)
    {
        SetConVarString(convar, oldValue);
    }
}

/**
 * Prepares the trie.
 */
static stock CvarLib_InitTrie()
{
    // Create trie if it doesn't exist.
    if (CvarLib_IsChanging == INVALID_HANDLE)
    {
        CvarLib_IsChanging = CreateTrie();
    }
}

/**
 * Removes the trie if it's no longer used.
 */
static stock CvarLib_CleanTrie()
{
    // Remove trie if it's empty.
    if (CvarLib_IsChanging != INVALID_HANDLE && GetTrieSize(CvarLib_IsChanging) == 0)
    {
        CloseHandle(CvarLib_IsChanging);
        CvarLib_IsChanging = INVALID_HANDLE;
    }
}

/**
 * Swaps the boolean state of a cvar in the trie.
 *
 * @return  New state.
 */
static stock bool:CvarLib_ToggleState(const String:key[])
{
    new bool:cvarState;
    GetTrieValue(CvarLib_IsChanging, key, cvarState);
    cvarState = !cvarState;
    SetTrieValue(CvarLib_IsChanging, key, cvarState);
    return cvarState;
}

/**
 * Gets the key name used to identify the cvar in the cvar library.
 *
 * @param cvar      Cvar to identify.
 * @param dest      Destination string buffer.
 * @param destlen   Size of buffer.
 *
 * @return          Number of cells written.
 */
static stock CvarLib_GetKeyName(Handle:cvar, String:dest[], destlen)
{
    return Format(dest, destlen, "%x", cvar);
}

/**
 * Returns whether a cvar is locked or not.
 *
 * @param cvar      Cvar to check.
 *
 * @return          True if locked, false otherwise.
 */
static stock CvarLib_IsCvarLockedEx(Handle:hCvar)
{
    decl String:key[32];
    CvarLib_GetKeyName(hCvar, key, sizeof(key));
    
    new dummy;
    return GetTrieValue(CvarLib_IsChanging, key, dummy);
}
