/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:          cvarlib.inc
 *  Type:          Library
 *  Description:   Cvar-related API.
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

#if defined _cvarlib_included
 #endinput
#endif
#define _cvarlib_included

/**
 * Sets a cvar to the given value and then locks it.
 * 
 * @param cvar  The cvar to lock.
 * @param value The value to lock cvar as.
 */
stock CvarLib_LockCvar(const String:cvar[], const String:value[])
{
    new Handle:hCvar = FindConVar(cvar);
    SetConVarString(hCvar, value);
    HookConVarChange(hCvar, CvarLib_CvarLock);
}

/**
 * Unlocks a locked cvar.
 * 
 * @param cvar  The cvar to lock.
 */
stock CvarLib_UnlockCvar(const String:cvar[])
{
    UnhookConVarChange(FindConVar(cvar), CvarLib_CvarLock);
}

/**
 * Called when a console variable's value is changed.
 * Note: This can only possibly be called in pairs.  (User change cvar, then changing back to old) 
 * 
 * @param convar		Handle to the convar that was changed.
 * @param oldValue		String containing the value of the convar before it was changed.
 * @param newValue		String containing the new value of the convar.
 */
new bool:g_CvarLibLock = false;
public CvarLib_CvarLock(Handle:convar, const String:oldValue[], const String:newValue[])
{
    // We know that when this is true, the user is changing, and when its false, the callback is resetting to its locked value.
    g_CvarLibLock = !g_CvarLibLock;
    if (g_CvarLibLock)
        SetConVarString(convar, oldValue);
}
