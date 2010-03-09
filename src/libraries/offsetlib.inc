/*
 * ============================================================================
 *
 *  <Project Name>
 *
 *  File:          offsetlib.inc
 *  Type:          Library
 *  Description:   A library of offset wrappers.
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

/**
 * Gets an entity's color.
 * 
 * @param entity	The entity index.
 *  
 * @return          The alpha value of the client. (0-255)
 */
stock OffsLib_GetEntityColor(entity, color[4])
{
	static bool:gotconfig = false;
	static String:prop[32];
	
	if (!gotconfig)
	{
		new Handle:gc = LoadGameConfigFile("core.games");
		new bool:exists = GameConfGetKeyValue(gc, "m_clrRender", prop, sizeof(prop));
		CloseHandle(gc);
		
		if (!exists)
		{
			strcopy(prop, sizeof(prop), "m_clrRender");
		}
		
		gotconfig = true;
	}
	
	new offset = GetEntSendPropOffs(entity, prop);
	
	GetEntDataArray(entity, offset, color, sizeof(color), 1);
}

// **********************************************
//              CS:S Offset Stocks
// **********************************************

#if defined PROJECT_GAME_CSS

/**
 * Get the amount of cash a client has.
 * 
 * @param client    The client index.
 * 
 * @return          The amount of cash the client has.
 */
stock OffsLib_GetCash(client)
{
    new offsAccount = FindSendPropInfo("CCSPlayer", "m_iAccount");
    return GetEntData(client, offsAccount);
}

/**
 * Set the amount of cash a client has.
 * 
 * @param client    The client index.
 * @param amount    The amount of cash to set.
 */
stock OffsLib_SetCash(client, amount)
{
    new offsAccount = FindSendPropInfo("CCSPlayer", "m_iAccount");
    SetEntData(client, offsAccount, amount);
}

#endif
