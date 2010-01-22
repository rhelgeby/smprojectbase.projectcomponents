/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          infect.zr.inc
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
 * Returns true if the client is infected, false if not.
 *
 * @param client    The client index.
 *  
 * @return			True if infected, false if not.
 * @error           Client must be alive.
 */
native bool:ZR_IsClientInfected(client);

/**
 * Returns true if the client is human, false if not.
 *
 * @param client    The client index.
 *  
 * @return			True if human, false if not.
 * @error           Client must be alive.
 */
native bool:ZR_IsClientHuman(client);

/**
 * Called when a client is about to become a zombie.
 * Here you can modify any variable or stop the infection entirely.
 * 
 * @param client            The client index.
 * @param attacker          The the infecter. (-1 if there is no infecter)
 * @param motherinfect      If the client is becoming a mother zombie.
 * @param respawnoverride   True if the respawn cvar is being overridden.
 * @param respawn           The value that respawn is being overridden with.
 * 
 * @return                  See "enum Action" in core.inc
 */
forward Action:ZR_OnClientInfectedPre(&client, &attacker, &bool:motherinfect, &bool:respawnoverride, &bool:respawn);

/**
 * Called after a client has become a zombie.
 * 
 * @param client            The client index.
 * @param attacker          The the infecter. (-1 if there is no infecter)
 * @param motherinfect      If the client is becoming a mother zombie.
 * @param respawnoverride   True if the respawn cvar is being overridden.
 * @param respawn           The value that respawn is being overridden with.
 */
forward ZR_OnClientInfected(client, attacker, bool:motherinfect, bool:respawnoverride, bool:respawn);

/**
 * Called when a client is about to become a human. (Through either zr_human or admin menu)
 * Here you can modify any variable or stop the action entirely.
 * 
 * @param client            The client index.
 * @param respawn           True if the client was respawned, false if not.
 * @param protect           True if the client spawn protected, false if not.
 * 
 * @return                  See "enum Action" in core.inc
 */
forward Action:ZR_OnClientHumanPre(&client, &bool:respawn, &bool:protect);

/**
 * Called after a client has become a human. (Through either zr_human or admin menu)
 * 
 * @param client            The client index.
 * @param respawn           True if the client was respawned, false if not.
 * @param protect           True if the client spawn protected, false if not.
 */
forward ZR_OnClientHuman(client, bool:respawn, bool:protect);

/**
 * Client has been killed.
 * Note:  This doesn't fire when a client is infected.
 * 
 * @param client            The client index.
 * @param attacker          The attacker index.
 * @param weapon            The weapon the attacker used to kill the client.
 * @param headshot          True if the kill was a headshot, false if regular kill.
 */
forward ZR_OnClientDeath(client, attacker, const String:weapon[], bool:headshot);
