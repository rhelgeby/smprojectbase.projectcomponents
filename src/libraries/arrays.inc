/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:          arrays.inc
 *  Type:          Utilities
 *  Description:   Place for various array and string utility functions.
 *
 *  Copyright (C) 2009-2012  Greyscale, Richard Helgeby
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

#if defined _arrays_included
 #endinput
#endif
#define _arrays_included

/**
 * Character used by Array_GetStringMarker to build a marker string.
 */
#define STRING_MARKER_CHAR        '^'

/**
 * Checks if a vector exists in a vector array.
 * 
 * @param vec       Vector to search for in array
 * @param vecs      Array to search in.
 * @param numvecs   Number of vectors in the array.
 * 
 * @return          Index in array that the vector is in, -1 if not found.
 */
stock Array_IsVecInArray(Float:vec[3], Float:vecs[][3], numvecs)
{
    for (new vindex; vindex < numvecs; vindex++)
    {
        if (vec[0] == vecs[vindex][0]
            && vec[1] == vecs[vindex][1]
            && vec[2] == vecs[vindex][2])
        {
            return vindex;
        }
    }
    
    return -1;
}

/**
 * Adds a marker (^) at the specified position on a new line below.
 *
 * Note: Does not work with multiline strings as source.
 *
 * @param pos       Position of marker (zero based).
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 * @param marker    Optional Marker character. Defaults to STRING_MARKER_CHAR
 *                  defined in this file.
 * 
 * @return          Number of cells written.
 */
stock Array_GetStringMarker(pos, String:buffer[], maxlen, marker = STRING_MARKER_CHAR)
{
    // Check if the marker is outside the buffer space.
    if (pos > maxlen - 2)
    {
        // Outside buffer, stop here. The marker won't be visible anyways.
        return 0;
    }
    
    // Write spaces before marker.
    for (new i = 0; i < pos; i++)
    {
        buffer[i] = ' ';
    }
    
    // Write marker.
    buffer[pos] = marker;
    
    // Terminate string.
    buffer[pos + 1] = 0;
    
    // +1 for the terminator, +1 to shift zero-based number.
    return pos + 2;
}

/**
 * Counts number of a certain character in a string value.
 *
 * @praam value     Source string.
 * @param c         Character type to count.
 *
 * @return          Number of characters found.
 */
stock Array_CountChars(const String:value[], c)
{
    new count = 0;
    new pos = 0;
    
    while (value[pos] != 0)
    {
        if (value[pos] == c)
        {
            count++;
        }
        pos++;
    }
    
    return count;
}

/**
 * Counts number of a certain character in a string value.
 *
 * @praam value     Source string.
 * @param c         String with a single character type to count.
 *
 * @return          Number of characters found.
 */
stock Array_CountCharsEx(const String:value[], const String:character[])
{
    return Array_CountChars(value, character[0]);
}

/**
 * Merges several dynamic arrays into a new one. Duplicates are not removed.
 *
 * @param arrays        List of arrays to merge (handles).
 * @param numArrays     Number of arrays in the list.
 *
 * @return              Handle to new array with all elements merged.
 */
stock Handle:Array_Merge(const Handle:arrays[], numArrays)
{
    new Handle:merged = CreateArray();
    
    // Iterate through arrays.
    for (new i = 0; i < numArrays; i++)
    {
        new Handle:array = arrays[i];
        new len = GetArraySize(array);
        
        // Add elements in the current array to the new array.
        for (new j = 0; j < len; j++)
        {
            PushArrayCell(merged, GetArrayCell(array, j));
        }
    }
    
    return merged;
}

/**
 * Removes duplicate integers in the array.
 *
 * Note: The array is sorted for optimal removal.
 *
 * @param array     Array to remove duplicate integers from.
 * @param sorted    Optional. Specifies whether the array is sorted or not.
 *                  Arrays who already are sorted doesn't need to be sorted
 *                  again by this function. Default is false.
 */
stock Array_RemoveDuplicateInt(Handle:array, bool:sorted = false)
{
    // Sort array if not sorted.
    if (!sorted)
    {
        // Sort the array so duplicate entries will be next to eachother.
        SortADTArray(array, Sort_Ascending, Sort_Integer);
    }
    
    new len = GetArraySize(array);
    if (len < 2)
    {
        // Arrays with one or zero elements can't have duplicates.
        return;
    }
    
    new currentVal;
    new lastVal = GetArrayCell(len - 1);
    
    // Iterate backwards through elements and remove duplicates. Elements are
    // removed at the end first so that minimal amount of elements must be
    // shifted.
    for (new i = len - 2; i >= 0; i--)
    {
        currentVal = GetArrayCell(i);
        if (lastVal == currentVal)
        {
            // Remove last duplicate (the one after this).
            RemoveFromArray(array, i + 1);
        }
        lastVal = currentVal;
    }
}

/**
 * Converts the contents of a cell array to a string.
 *
 * Output format: {1, 2, 3, ...}
 *
 * @param values    Source array.
 * @param len       Size of source array.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 *
 * @return          Number of cells written.
 */
stock Array_CellsToString(const values[], len, String:buffer[], maxlen)
{
    decl String:valuebuffer[16];
    valuebuffer[0] = 0;
    
    // Make sure buffer is empty.
    buffer[0] = 0;
    
    new writeCount = 0;
    
    writeCount = StrCat(buffer, maxlen, "{");
    
    // Loop throug values.
    for (new i = 0; i < len; i++)
    {
        IntToString(values[i], valuebuffer, sizeof(valuebuffer));
        writeCount += StrCat(buffer, maxlen, valuebuffer);
        
        // If not the last value.
        if (i < len - 1)
        {
            writeCount += StrCat(buffer, maxlen, ", ");
        }
    }
    
    writeCount += StrCat(buffer, maxlen, "}");
    
    return writeCount;
}

/**
 * Converts the contents of a float array to a string.
 *
 * Output format: {1.0, 2.0, 3.0, ...}
 *
 * @param values    Source array.
 * @param len       Size of source array.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 */
stock Array_FloatsToString(const Float:values[], len, String:buffer[], maxlen)
{
    decl String:valuebuffer[16];
    valuebuffer[0] = 0;
    
    // Make sure buffer is empty.
    buffer[0] = 0;
    
    StrCat(buffer, maxlen, "{");
    
    // Loop throug values.
    for (new i = 0; i < len; i++)
    {
        FloatToString(values[i], valuebuffer, sizeof(valuebuffer));
        StrCat(buffer, maxlen, valuebuffer);
        
        // If not the last value.
        if (i < len - 1)
        {
            StrCat(buffer, maxlen, ", ");
        }
    }
    
    StrCat(buffer, maxlen, "}");
}

/**
 * Gets and pops the last value from an ADT array.
 *
 * @param stackArray    Handle to ADT array that is used as a stack.
 * @param value         (Output) Value retrieved at last index in array. The
 *                      value is only valid if this function returns true.
 *
 * @return              True if a value was retrieved, false if array was empty.
 */
stock bool:Array_PopCell(Handle:stackArray, &any:value = 0)
{
    // Get last cell.
    if (Array_PeekCell(stackArray, value))
    {
        // Value retrieved, remove it from the array.
        new len = GetArraySize(stackArray);
        RemoveFromArray(stackArray, len - 1);
        return true;
    }
    
    // Stack was empty.
    return false;
}

/**
 * Gets the value at the last index from an ADT array.
 *
 * @param stackArray    Handle to ADT array that is used as a stack.
 * @param value         (Output) Value retrieved at last index in array. The
 *                      value is only valid if this function returns true.
 *
 * @return              True if a value was retrieved, false if array was empty.
 */
stock bool:Array_PeekCell(Handle:stackArray, &any:value = 0)
{
    new len = GetArraySize(stackArray);
    
    if (!len)
    {
        // Stack empty.
        return false;
    }
    
    // Get last cell.
    value = GetArrayCell(stackArray, len - 1);
    return true;
}
