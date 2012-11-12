/*
 * ============================================================================
 *
 *  PawnUnit
 *
 *  File:          assert.inc
 *  Type:          Include
 *  Description:   PawnUnit assertion functions.
 *                 
 *                 Use these with the Assert macro in combination with assertion
 *                 functions below so that it will return an error code if the
 *                 assert failed, without poisoning the test code.
 *
 *  Copyright (C) 2012  Richard Helgeby
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

#if defined _pawnunit_assert_included
 #endinput
#endif
#define _pawnunit_assert_included

/**
 * General assert macro that returns Test_Failed if logically false.
 *
 * @param 1     Logical expression. (Like assert functions below.)
 */
#define Assert(%1)          if (!(%1)) return Test_Failed

/**
 * General assert macro that returns Test_Failed if logically false, and stores
 * an error message in the global message buffer.
 *
 * @param 1     Logical expression. (Like assert functions below.)
 * @param 2     Message formatting rules string with parameters. (As in the 
 *              Format function)
 */
#define AssertMsg(%1, %2)   if (!(%1)) {Format(MessageBuffer, sizeof(MessageBuffer), %2); return Test_Failed;}

/**
 * General macro that returns Test_Failed and stores an error message in the
 * global message buffer.
 *
 * @param 1     Message formatting rules string with parameters. (As in the 
 *              Format function)
 */
#define FailTest(%1)        Format(MessageBuffer, sizeof(MessageBuffer), %1); return Test_Failed


#define FORMAT_MESSAGE(%1)  Format(MessageBuffer, sizeof(MessageBuffer), %1)

// Strings.
#define NOT_TRUE                    "Value was not true"
#define NOT_FALSE                   "Value was not false"

#define CELL_EXPECTED_BUT_WAS       "Expected %d, but was %d"
#define FLOAT_EXPECTED_BUT_WAS      "Expected %f, but was %f"
#define STRING_EXPECTED_BUT_WAS     "Expected \"%s\", but was \"%s\""

#define CELL_TOO_LOW                "Expected below %d, but was %d"
#define FLOAT_TOO_LOW               "Expected below %f, but was %f"

#define CELL_TOO_HIGH               "Expected above %d, but was %d"
#define FLOAT_TOO_HIGH              "Expected above %f, but was %f"

#define HANDLE_NOT_VALID            "Handle not valid"
#define ACTION_EXPECTED_BUT_WAS     "Expected action value %d, but was %d"

#define CLIENT_NOT_VALID            "Client index is not valid. Actual value: %d"
#define CLIENT_NOT_IN_GAME          "Client %d is not in game"
#define CLIENT_NOT_CONNECTED        "Client %d is not connected"
#define CLIENT_NOT_ALIVE            "Client %d is not alive"


/*********************
 *   General logic   *
 *********************/

/**
 * Returns whether the value is considered as logically true.
 *
 * @param actual    Actual value.
 * @param msg       Optional. Custom error message.
 *
 * @return          True if logically true, false otherwise.
 */
stock bool:True(any:actual, const String:msg[] = NOT_TRUE)
{
    if (!actual)
    {
        FORMAT_MESSAGE(msg);
        return false;
    }
    return true;
}

/**
 * Returns whether the value is considered as logically false.
 *
 * @param actual    Actual value.
 * @param msg       Optional. Custom error message.
 *
 * @return          True if logically false, false otherwise.
 */
stock bool:False(any:actual, const String:msg[] = NOT_FALSE)
{
    if (actual)
    {
        FORMAT_MESSAGE(msg);
        return false;
    }
    return true;
}

/**
 * Returns whether the two cell values are equal.
 *
 * @param expected  Expected value.
 * @param actual    Actual value.
 * @param msg       Optional. Custom error message.
 *
 * @return          True if equal, false otherwise.
 */
stock bool:CellEquals(expected, actual, const String:msg[] = CELL_EXPECTED_BUT_WAS)
{
    if (actual != expected)
    {
        FORMAT_MESSAGE(msg, expected, actual);
        return false;
    }
    return true;
}

/**
 * Returns whether the two float values are equal.
 *
 * @param expected  Expected value.
 * @param actual    Actual value.
 * @param delta     Optional. Maximum difference for which expected and actual
 *                  value are considered equal. Default is 0.0001.
 * @param msg       Optional. Custom error message.
 *
 * @return          True if equal, false otherwise.
 */
stock bool:FloatEquals(Float:expected, Float:actual, Float:delta = 0.0001, const String:msg[] = FLOAT_EXPECTED_BUT_WAS)
{
    if (actual != expected && (FloatAbs(expected - actual) > delta))
    {
        FORMAT_MESSAGE(msg, expected, actual);
        return false;
    }
    return true;
}

/**
 * Returns whether the two string values are equal.
 *
 * @param expected          Expected value.
 * @param actual            Actual value.
 * @param caseSensitive     Optional. Whether comparision is case sensitive.
 *                          Default is true. 
 * @param msg               Optional. Custom error message.
 *
 * @return                  True if equal, false otherwise.
 */
stock bool:StringEquals(const String:expected[], const String:actual, bool:caseSensitive = true, const String:msg[] = STRING_EXPECTED_BUT_WAS)
{
    if (!StrEqual(expected, actual, caseSensitive))
    {
        FORMAT_MESSAGE(msg, expected, actual);
        return false;
    }
    return true;
}

/**
 * Returns whether the actual value is less than the min value (exclusive).
 *
 * @param min       Minimum value (exclusive).
 * @param actual    Actual value.
 * @param msg       Optional. Custom error message.
 *
 * @return          True if equal, false otherwise.
 */
stock bool:CellLessThan(min, actual, const String:msg[] = CELL_TOO_LOW)
{
    if (actual > min)
    {
        FORMAT_MESSAGE(msg, min, actual);
        return false;
    }
    return true;
}

/**
 * Returns whether the actual value is less than the min value (exclusive).
 *
 * @param min       Minimum value (exclusive).
 * @param actual    Actual value.
 * @param msg       Optional. Custom error message.
 *
 * @return          True if equal, false otherwise.
 */
stock bool:FloatLessThan(Float:min, Float:actual, const String:msg[] = FLOAT_TOO_LOW)
{
    if (actual > min)
    {
        FORMAT_MESSAGE(msg, min, actual);
        return false;
    }
    return true;
}

/**
 * Returns whether the actual value is greater than the max value (exclusive).
 *
 * @param max       Maximum value (exclusive).
 * @param actual    Actual value.
 * @param msg       Optional. Custom error message.
 *
 * @return          True if equal, false otherwise.
 */
stock bool:CellGreaterThan(max, actual, const String:msg[] = CELL_TOO_HIGH)
{
    if (actual < max)
    {
        FORMAT_MESSAGE(msg, min, actual);
        return false;
    }
    return true;
}

/**
 * Returns whether the actual value is greater than the min value (exclusive).
 *
 * @param max       Maximum value (exclusive).
 * @param actual    Actual value.
 * @param msg       Optional. Custom error message.
 *
 * @return          True if equal, false otherwise.
 */
stock bool:FloatGreaterThan(Float:max, Float:actual, const String:msg[] = FLOAT_TOO_HIGH)
{
    if (actual < max)
    {
        FORMAT_MESSAGE(msg, min, actual);
        return false;
    }
    return true;
}


/*****************
 *   SourceMod   *
 *****************/

/**
 * Returns whether a handle is valid (not INVALID_HANDLE or lower).
 *
 * @param actual    Actual value.
 * @param msg       Optional. Custom error message.
 *
 * @return          True if valid, false otherwise.
 */
stock bool:ValidHandle(Handle:actual, const String:msg[] = HANDLE_NOT_VALID)
{
    if (actual <= INVALID_HANDLE)
    {
        FORMAT_MESSAGE(msg, actual);
        return false;
    }
    return true;
}

/**
 * Returns whether a client index is valid.
 *
 * @param actual            Actual value.
 * @param includeServer     Optional. Consider the server index as a valid
 *                          client. Default is false.
 * @param msg               Optional. Custom error message.
 *
 * @return                  True if valid, false otherwise.
 */
stock bool:ValidClientIndex(actual, bool:includeServer = false, const String:msg[] = CLIENT_NOT_VALID)
{
    if (actual > MAXPLAYERS - 1 ||          // Upper limit.
        (actual < 0 && includeServer) ||    // Lower limit with server index.
        (actual < 1 && !includeServer))     // Lower limit without server index.
    {
        FORMAT_MESSAGE(msg, actual);
        return false;
    }
    return true;
}

/**
 * Returns whether the action values are equal.
 *
 * @param expected  Expected value.
 * @param actual    Actual value.
 * @param msg       Optional. Custom error message.
 *
 * @return          True if equal, false otherwise.
 */
stock bool:ActionEquals(Action:expected, Action:actual, const String:msg[] = ACTION_EXPECTED_BUT_WAS)
{
    if (actual != expected)
    {
        FORMAT_MESSAGE(msg, _:expected, _:actual);
        return false;
    }
    return true;
}

/**
 * Returns whether a client is in game.
 *
 * @param client    Client index.
 * @param msg       Optional. Custom error message.
 *
 * @return          True if in game, false otherwise.
 */
stock bool:ClientInGame(client, const String:msg[] = CLIENT_NOT_IN_GAME)
{
    if (!IsClientInGame(client))
    {
        FORMAT_MESSAGE(msg, client);
        return false;
    }
    return true;
}

/**
 * Returns whether a client is connected.
 *
 * @param client    Client index.
 * @param msg       Optional. Custom error message.
 *
 * @return          True if connected, false otherwise.
 */
stock bool:ClientConnected(client, const String:msg[] = CLIENT_NOT_CONNECTED)
{
    if (!IsClientConnected(client))
    {
        FORMAT_MESSAGE(msg, client);
        return false;
    }
    return true;
}

/**
 * Returns whether a player is alive.
 *
 * @param client    Client index.
 * @param msg       Optional. Custom error message.
 *
 * @return          True if alive, false otherwise.
 */
stock bool:PlayerAlive(client, const String:msg[] = CLIENT_NOT_ALIVE)
{
    if (!IsPlayerAlive(client))
    {
        FORMAT_MESSAGE(msg, client);
        return false;
    }
    return true;
}
