/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          respawn.zr.inc
 *  Type:          Include
 *  Description:   Infect-related natives/forwards.
 *
 *  Copyright (C) 2009-10  Greyscale, Richard Helgeby
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

/**
 * Spawns a player into the round following rules set by cvars.
 * 
 * @param client            The client index.
 * @param zombie            True to spawn as zombie, false to spawn as human.
 * @param zombie_if_suicide If true, client will be respawned as a zombie if they died by suicide.
 */
native ZR_RespawnClient(client, bool:zombie = false, bool:zombie_if_suicide = false);

/**
 * Called right before ZR respawns a client.
 * Here you can modify any variable or stop the action entirely.
 * 
 * @param client            The client index.
 * @param zombie            True if the client is respawning as a zombie, false if not.
 * @param zombie_if_suicide True if the client will be respawned as a zombie if they suicided.
 */
forward ZR_OnClientRespawnPre(&client, &bool:zombie, &bool:zombie_if_suicide);

/**
 * Called after ZR respawns a client.
 * 
 * @param client            The client index.
 * @param zombie            True if the client has respawned as a zombie, false if not.
 * @param zombie_if_suicide True if the client has been respawned as a zombie if they suicided.
 */
forward ZR_OnClientRespawn(client, bool:zombie, bool:zombie_if_suicide);

/**
 * Override if the client died by a suicide.
 * Note: 'zombie_if_suicide' checks this value.
 * Note: This value is set by Zombie:Reloaded when a client dies.
 * 
 * @param client    The client index.
 * @param suicide   True to say the client suicided, false if they were killed by another client.
 */
native ZR_OverrideKilledByWorld(client, bool:suicide);

/**
 * Read the value of if the client died by a suicide.
 * Note: 'zombie_if_suicide' checks this value.
 * Note: This value is set by Zombie:Reloaded when a client dies.
 * 
 * @param client    The client index.
 * 
 * @return          True if the client died by suicide, false if they were killed by another client.
 */
native bool:ZR_ReadKilledByWorld(client);
