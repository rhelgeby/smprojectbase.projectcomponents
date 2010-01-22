/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          tools.zr.inc
 *  Type:          Include
 *  Description:   Tool natives in ZR.
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
 * @section m_takedamage datamap property values.
 */
#define TOOLS_TAKEDAMAGE_ENABLE 2   /** Allows entity to take damage. */
#define TOOLS_TAKEDAMAGE_DISABLE 0  /** Blocks all damage done to entity. */
/**
 * @endsection
 */

/**
 * NVG properties on a client.
 */
enum ToolsNVGProps
{
    NVGProp_Owner,  /** Controls whether the client has nightvision or not. */
    NVGProp_Enabled /** Controls whether the client's nightvision is enabled. (must own nightvision to have effect) */
}

/**
 * Score properties on a client.
 */
enum ToolsScoreProps
{
    ScoreProp_Kills,    /** Controls how many kills the client has. (score) */
    ScoreProp_Deaths    /** Controls how many deaths the client has. */
}

/**
 * Set a client's velocity.
 * 
 * @param client        The client index.
 * @param vecVelocity   Velocity to set on client.
 * @param stack         If modifying velocity, then true will stack new velocity onto the client's
 *                      current velocity, false will reset it.
 */
native ZR_SetClientVelocity(client, const Float:vecVelocity[3], bool:stack = true);

/**
 * Get a client's velocity.
 *  
 * @param client        The client index.
 * @param vecVelocity   Array to store velocity vector in.
 */
native ZR_GetClientVelocity(client, Float:vecVelocity[3]);

/**
 * Set a client's lagged movement value.
 *  
 * @param client    The client index.
 * @param value     LMV value. (1.0 = default, 2.0 = double)
 */
native ZR_SetClientLMV(client, Float:value);

/**
 * Get a client's lagged movement value.
 *  
 * @param client    The client index.
 *  
 * @return          LMV value. (1.0 = default, 2.0 = double)
 */
native Float:ZR_GetClientLMV(client);

/**
 * Set nightvision properties on a client.
 *  
 * @param client    The client index.
 * @param prop      The prop to set: See ToolsNVGProps (top of file)
 * @param value     New value of the property of nightvision.
 */
native ZR_SetClientNVG(client, ToolsNVGProps:prop, bool:value);

/**
 * Get nightvision properties on a client.
 *  
 * @param client    The client index.
 * @param prop      The prop to get: See ToolsNVGProps (top of file)
 * 
 * @return          The value of the property of nightvision on client.
 */
native bool:ZR_GetClientNVG(client, ToolsNVGProps:prop);

/**
 * Set a client's default field of vision.
 *  
 * @param client    The client index.
 * @param FOV       The field of vision of the client.
 */
native ZR_SetClientDefaultFOV(client, FOV);

/**
 * Set a client's score or deaths.
 * 
 * @param client    The client index.
 * @param prop      The prop to get: See ToolsScoreProps (top of file)
 * @param value     New value of the property of their score.
 */
native ZR_SetClientScore(client, ToolsNVGProps:prop, bool:value);

/**
 * Get a client's score or deaths.
 * 
 * @param client    The client index.
 * @param prop      The prop to get: See ToolsScoreProps (top of file)
 * 
 * @return          The value of the property of the client's score.
 */
native ZR_GetClientScore(client, ToolsNVGProps:prop);

/**
 * Set a client's (not entity) alpha value.
 * Note: Using this will forward the event to the rest of ZR. (Ex, weapon alpha will be updated as well if you use this) 
 * 
 * @param client    The client index.
 * @param alpha     The alpha value to set client's alpha to. (0-255)
 * @param weapons   Apply alpha to all client's weapons.
 */
native ZR_SetClientAlpha(client, alpha);

/**
 * Sets an entity's color. (This is the "get" version of SetEntityRenderColor)
 * 
 * @param entity	The entity index.
 *  
 * @return          The alpha value of the client. (0-255)
 */
native ZR_GetEntityAlpha(entity);

/**
 * Add a flag to an entity.
 * 
 * @param entity    The entity index.
 * @param flag      The flag to add. (See entity_prop_stocks.inc for flags)
 */
native ZR_AddEntityFlag(entity, flag);

/**
 * Remove a flag from an entity.
 * 
 * @param entity    The entity index.
 * @param flag      The flag to remove. (See entity_prop_stocks.inc for flags)
 */
native ZR_RemoveEntityFlag(entity, flag);
