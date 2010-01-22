/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          credits.zr.inc
 *  Type:          Include
 *  Description:   Credits-related natives/forwards.
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
 * Give credits to a client.
 * 
 * @client  The client index.
 * @amount  The amount of credits to give.
 * 
 * @return  The new amount of credits.
 */
native ZR_GiveCredits(client, amount);

/**
 * Take credits from a client.
 * 
 * @client  The client index.
 * @amount  The amount of credits to take.
 * 
 * @return  The new amount of credits.
 */
native ZR_TakeCredits(client, amount);

/**
 * Set the amount of credits a client has.
 * 
 * @param client    The client index.
 * @param amount    The amount of credits.
 */
native ZR_SetCredits(client, amount);

/**
 * Get the amount of credits a client has.
 * 
 * @param client    The client index.
 * 
 * @param return    The amount of credits the client has.
 */
native ZR_GetCredits(client);

/**
 * Converts currencies.
 * 
 * @param value         The value of the currency you are converting from. (rounded to nearest whole number)
 * @param exchangerate  The exchange rate for this currency to another. (See defines at the top of the file)
 * 
 * @return              The value of the new currency. (rounded to nearest whole number)
 */
native ZR_ConvertCurrency(value, Float:exchangerate);

/**
 * Called when credits on a client are about to be modified.
 * Here you can modify any variable or stop the action entirely. 
 * 
 * @param client    The client index.
 * @param newvalue  The current amount of credits.
 */
forward Action:ZR_OnCreditsModifiedPre(&client, &newvalue);

/**
 * Called when credits on a client are modified.
 * 
 * @param client    The client index.
 * @param newvalue  The current amount of credits.
 * @param oldvalue  How many the client had before the change.
 */
forward ZR_OnCreditsModified(client, newvalue, oldvalue);
