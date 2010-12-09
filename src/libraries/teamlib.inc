/*
 * ============================================================================
 *
 *  <Project Name>
 *
 *  File:          teamlib.inc
 *  Type:          Library
 *  Description:   Virtual team managing API.
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

// Include libraries.
#include "project/libraries/sdktoolslib"

/**
 * Team defines.
 */
#if defined PROJECT_GAME_CSS
  #define TEAM_0 CS_TEAM_NON
  #define TEAM_1 CS_TEAM_SPECTATOR
  #define TEAM_2 CS_TEAM_T
  #define TEAM_3 CS_TEAM_CT
#endif

#if defined PROJECT_GAME_TF2
  #define TEAM_0 _:TFTeam_Unassigned
  #define TEAM_1 _:TFTeam_Spectator
  #define TEAM_2 _:TFTeam_Red
  #define TEAM_3 _:TFTeam_Blue
#endif

/**
 * Virtual teams.
 */
enum VTeam
{
    VTeam_Team1,            /** A virtual team. */
    VTeam_Team2             /** Another virtual team. */
}

/**
 * Global array to store each client's virtual team.
 */
new VTeam:g_TLibClientTeam[MAXPLAYERS + 1];

/**
 * Function to return the client's virtual team.
 */
stock VTeam:TLib_GetClientTeam(client) { return g_TLibClientTeam[client]; }

/**
 * Check client's virtual team via boolean return type.
 */
stock bool:TLib_IsClientTeam1(client) { return (g_TLibClientTeam[client] == VTeam_Team1); }
stock bool:TLib_IsClientTeam2(client) { return (g_TLibClientTeam[client] == VTeam_Team2); }

/**
 * Set a client's team.
 * 
 * @param client    The client index.
 * @param team      The team from enum VTeam to set.
 */
stock TLib_SetClientTeam(client, VTeam:team) { g_TLibClientTeam[client] = team; }

/**
 * Count clients on a specific virtual team.
 * 
 * @param team  The team to count clients of.
 * @param alive Only count client if they're alive. 
 * 
 * @return      The number of clients on this team.  
 */
stock TLib_CountTeam(VTeam:team, bool:alive)
{
    new count;
    for (new client = 1; client <= MaxClients; client++)
    {
        if (!IsClientInGame(client))
            continue;
        
        // Skip dead players if they must be alive.
        if (alive && !IsPlayerAlive(client))
            continue;
        
        if (team == g_TLibClientTeam[client])
            count++;
    }
    
    return count;
}

/**
 * Stores the game team index for a given virtual team.
 */
new g_iTLibGameTeamIndex[VTeam];

/**
 * Gets the game team index for a virtual team.
 */
stock TLib_GetGameTeamIndex(VTeam:team) { return g_iTLibGameTeamIndex[team]; }

/**
 * Set the game team index for a virtual team.
 * 
 * @param team          Team to set.
 * @param teamindex     Team index to set.
 */
stock TLib_SetGameTeamIndex(VTeam:team, teamindex) { g_iTLibGameTeamIndex[team] = teamindex; }

/**
 * Convert a game team string into a game team index.
 * 
 * @param teamstring    The game team string to convert to a game team index.
 * 
 * @return              The game team index, -1 if no team index is associated with the team string.
 */
stock TLib_GTeamStringToIndex(const String:teamstring[])
{
    #if defined PROJECT_GAME_CSS
        if (StrEqual(teamstring, "spec", false) || StrEqual(teamstring, "spectators", false))
            return CS_TEAM_SPECTATOR;
        else if (StrEqual(teamstring, "t", false) || StrEqual(teamstring, "terrorists", false))
            return CS_TEAM_T;
        else if (StrEqual(teamstring, "ct", false) || StrEqual(teamstring, "counter-terrorists", false))
            return CS_TEAM_CT;
        
        return -1;
    #else
        ThrowError("TLib_GTeamStringToIndex doesn't support this game");
        return -1;
    #endif
}

#if defined PROJECT_GAME_CSS

/**
 * Set a client's game team to the one bound to their virtual team.
 * 
 * @param client    The client index.
 */
stock TLib_UpdateClientTeam(client)
{
    new VTeam:vteam = TLib_GetClientTeam(client);
    new gteam = GetClientTeam(client);
    
    // Check if the client is already on the game team before trying to switch them to it.
    if (gteam != g_iTLibGameTeamIndex[vteam])
    {
        #if defined PROJECT_GAME_CSS
            CS_SwitchTeam(client, g_iTLibGameTeamIndex[vteam]);
        #endif
    }
}

/**
 * Set all clients' game teams to the one bound to their virtual team.
 */
stock TLib_UpdateClientTeams()
{
    for (new client = 1; client <= MaxClients; client++)
    {
        if (!IsClientInGame(client))
            continue;
        
        TLib_UpdateClientTeam(client);
    }
}

#endif
