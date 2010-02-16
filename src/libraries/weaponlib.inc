/*
 * ============================================================================
 *
 *  <Project Name>
 *
 *  File:          weaponlib.inc
 *  Type:          Library
 *  Description:   Weapon-related API.
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

#include <sdktools>

/**
 * The number of slots for weapons to be stored in.  (CS:S)
 */
#define WEPLIB_SLOT_COUNT 5

/**
 * Weapon slots. (CS:S)
 */
enum WepLib_Slots
{
    Slot_Invalid        = -1,   /** Invalid weapon (slot). */
    Slot_Primary        = 0,    /** Primary weapon slot. */
    Slot_Secondary      = 1,    /** Secondary weapon slot. */
    Slot_Melee          = 2,    /** Melee (knife) weapon slot. */
    Slot_Projectile     = 3,    /** Projectile (grenades, flashbangs, etc) weapon slot. */
    Slot_Explosive      = 4,    /** Explosive (c4) weapon slot. */
}

/**
 * Checks if a client has a specific weapon.
 * 
 * @param client    The client index.
 * @param weapon    The weapon classname.
 * 
 * @return          True if the client has the weapon, false if not.
 */
stock bool:WepLib_HasWeapon(client, const String:weapon[])
{
    // x = slot index
    new weaponindex;
    decl String:classname[64];
    for (new weaponslot = 0; weaponslot < WEPLIB_SLOT_COUNT; x++)
    {
        weaponindex = GetPlayerWeaponSlot(client, weaponslot);
        
        // If slot is empty, then stop.
        if (weaponindex == -1)
            continue;
        
        // If the weapon's classname matches, then return true.
        GetEdictClassname(weaponindex, classname, sizeof(classname));
        if (StrEqual(weapon, classname, false))
            return true;
    }
    
    return false;
}

/**
 * Return an array that contains all client's weapon indexes.
 * Note: This only returns the first grenade in the 4th slot.
 * 
 * @param client    The client index.
 * @param weapons   The weapon index array.  -1 if no weapon in slot.
 */
stock WepLib_GetWeapons(client, weapons[WepLib_Slots])
{
    for (new weaponslot = 0; weaponslot < WEPLIB_SLOT_COUNT; weaponslot++)
    {
        weapons[WepLib_Slots:weaponslot] = GetPlayerWeaponSlot(client, weaponslot);
    }
}

/**
 * Returns weapon index of the client's deployed weapon.
 * 
 * @param client    The client index.
 *  
 * @return          The weapon index of the deployed weapon.  -1 if no weapon is deployed.
 */
stock WepLib_GetDeployedWeaponIndex(client)
{
    // Return the client's active weapon.
    return GetEntPropEnt(client, Prop_Data, "m_hActiveWeapon");
}

/**
 * Returns slot of client's deployed weapon.
 * 
 * @param client    The client index.
 *  
 * @return          The slot number of deployed weapon.
 */
stock WepLib_Slots:WeaponsGetDeployedWeaponSlot(client)
{
    // Get client's deployed weapon.
    new deployedweapon = WeaponsGetDeployedWeaponIndex(client);
    
    // If client has no deployed weapon, then stop.
    if (deployedweapon == -1)
        return Slot_Invalid;
    
    for (new weaponslot = 0; weaponslot < WEPLIB_SLOT_COUNT; weaponslot++)
    {
        // If the weapon indexes match, then return it.
        if (weapons[weaponslot] == deployedweapon)
            return WepLib_Slots:weaponslot;
    }
    
    return Slot_Invalid;
}

/**
 * Used to explicitly remove projectile weapons from a client.
 * 
 * @param client    The client index.
 */
public WepLib_RemoveGrenades(client)
{
    // While GetPlayerWeaponSlot returns a valid projectile, remove it and then test again.
    new grenade = GetPlayerWeaponSlot(client, _:Slot_Projectile);
    while (grenade != -1)
    {
        RemovePlayerItem(client, grenade);
        grenade = GetPlayerWeaponSlot(client, _:Slot_Projectile);
    }
}

/**
 * Refresh a weapon by taking it and giving it back.
 * Sometimes changing models on a client screws up the weapon attachments/angles. 
 * 
 * @param client        The client index.
 * @param slot          The weapon slot to refresh. (see enum WepLib_Slots)
 */
stock WepLib_RefreshWeapon(client, WepLib_Slots:slot)
{
    new weaponindex = GetPlayerWeaponSlot(client, _:slot);
    if (weaponindex == -1)
        return;
    
    // Get the classname of the weapon to re-give.
    decl String:entityname[64];
    GetEdictClassname(weaponindex, entityname, sizeof(entityname));
    
    // Refresh weapon.
    RemovePlayerItem(client, weaponindex);
    GivePlayerItem(client, entityname);
}

/**
 * Remove all weapons.
 * 
 * @param client        The client index.
 */
stock WepLib_RemoveAllWeapons(client)
{
    new weaponindex;
    for (new weaponslot = 0; weaponslot < WEPLIB_SLOT_COUNT; weaponslot++)
    {
        weaponindex = GetPlayerWeaponSlot(client, weaponslot);
        if (weaponindex == -1)
            continue;
        
        RemovePlayerItem(client, weaponindex);
    }
    
    // Remove left-over projectiles.
    WeaponsRemoveClientGrenades(client, weaponsdrop);
    
    // Give zombie a new knife. (If you leave the old one there will be glitches with the knife positioning)
    GivePlayerItem(client, "weapon_knife");
}

/**
 * Refresh all weapons by taking them and giving them back.
 * 
 * @param client        The client index.
 */
stock WepLib_RefreshAllWeapons(client)
{
    new weaponindex;
    for (new weaponslot = 0; weaponslot < WEPLIB_SLOT_COUNT; weaponslot++)
    {
        weaponindex = GetPlayerWeaponSlot(client, weaponslot);
        if (weaponindex == -1)
            continue;
        
        WeaponsRefreshClientWeapon(client, WepLib_Slots:weaponindex);
    }
}

/**
 * Checks if a client is in a buyzone.
 * Game: CS:S
 * 
 * @param client    The client index.
 */
stock bool:WepLib_InBuyZone(client)
{
    // This offset will only be found once.
    static iBuyZoneOffset;
    if (iBuyZoneOffset = 0)
        iBuyZoneOffset = FindSendPropInfo("CCSPlayer", "m_bInBuyZone");
    
    // Return if client is in buyzone.
    return bool:GetEntData(client, iBuyZoneOffset);
}
