/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           constrainthandler.inc
 *  Type:           Library
 *  Description:    Handles validation constraints for objectlib.
 *
 *  Copyright (C) 2012  Richard Helgeby, Greyscale
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
 * @section Flags for storing whether a special constraint handler is working.
 *          This is used to prevent re-entry in these constraint handlers if the
 *          handler itself override the value stored in objects.
 *
 *          This only applies to handlers that may override values or have user
 *          callbacks.
 */
static ObjLib_InCustomHandler = false;
new ObjLib_InLookupHandler = false;
/**
 * @endsection
 */

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Helper for accessor functions. It will call constraint handlers and set value
 * if valid and not overridden. This code was repeated in a lot of accessors
 * and moved here (except for array and string data types).
 */
stock ObjLib_ApplyValueConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        index,
        any:value,
        ObjLib_ErrorHandler:errorHandler)
{
    switch (ObjLib_HandleValueConstraints(object, typeDescriptor, index, value, errorHandler))
    {
        case ObjConstraintResult_Invalid, ObjConstraintResult_Overridden:
        {
            return;
        }
        case ObjConstraintResult_Valid:
        {
            // Set value.
            new Handle:data = ObjLib_GetObjectData(object);
            SetArrayCell(data, index, value);
            ObjLib_SetKeyNull(object, index, false);
        }
        default:
        {
            ThrowError("[BUG] Unexpected constraint result. This is a bug in objectlib.");
        }
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Calls the constraint handler matching the key's data type.
 *
 * @return              True if validation passed constraints.
 */
stock ObjectConstraintResult:ObjLib_HandleValueConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        keyIndex,
        any:value,
        ObjLib_ErrorHandler:customErrorHandler = INVALID_FUNCTION)
{
    // Get constraints object.
    new Object:constraints = ObjLib_GetTypeConstraintsAt(typeDescriptor, keyIndex);
    
    if (constraints == INVALID_OBJECT)
    {
        // No constraints.
        return ObjConstraintResult_Valid;
    }
    
    // Get error handler.
    new ObjLib_ErrorHandler:errorHandler = ObjLib_GetErrorHandler(typeDescriptor, customErrorHandler);
    
    // Delegate work to a handler that matches the data type.
    new ObjectType:constraintsType = ObjLib_GetTypeDescriptor(constraints);
    if (constraintsType == ObjLib_CustomConstraints)
    {
        new any:valueBuffer[1];
        valueBuffer[0] = value;
        
        return ObjLib_HandleCustomConstraints(object, typeDescriptor, constraints, keyIndex, valueBuffer, sizeof(valueBuffer), errorHandler);
    }
    else if (constraintsType == ObjLib_LookupConstraints)
    {
        // Check if sub constraints are available.
        new Object:subConstraints = ObjLib_GetObject(constraints, "subConstraints");
        if (subConstraints != INVALID_OBJECT)
        {
            // Recursive re-entry for handling raw data constraints. (Recursion
            // is stopped by preventing lookup contraint objects in
            // subConstraints when building the constraint object.)
            return ObjLib_HandleValueConstraints(object, typeDescriptor, keyIndex, value, customErrorHandler);
        }
        else
        {
            // No sub constraints.
            return ObjConstraintResult_Valid;
        }
    }
    else if (constraintsType == ObjLib_CellConstraints)
    {
        return ObjLib_HandleCellConstraints(object, typeDescriptor, constraints, keyIndex, value, errorHandler);
    }
    else if (constraintsType == ObjLib_FloatConstraints)
    {
        return ObjLib_HandleFloatConstraints(object, typeDescriptor, constraints, keyIndex, value, errorHandler);
    }
    else if (constraintsType == ObjLib_HandleConstraints)
    {
        return ObjLib_HandleHandleConstraints(object, typeDescriptor, constraints, keyIndex, value, errorHandler);
    }
    else if (constraintsType == ObjLib_FunctionConstraints)
    {
        return ObjLib_HandleFunctionConstraints(object, typeDescriptor, constraints, keyIndex, value, errorHandler);
    }
    else if (constraintsType == ObjLib_ObjectConstraints)
    {
        return ObjLib_HandleObjectConstraints(object, typeDescriptor, constraints, keyIndex, value, errorHandler);
    }
    else if (constraintsType == ObjLib_ObjectTypeConstraints)
    {
        return ObjLib_HandleObjTypeConstraints(object, typeDescriptor, constraints, keyIndex, value, errorHandler);
    }
    else
    {
        // Invalid constraints object type.
        ThrowError("[BUG] Invalid constraints object type. They are validated so this is a bug in objectlib.");
        return ObjConstraintResult_Invalid;
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Calls the constraint handler matching the key's data type.
 *
 * @return          True if value passed constraints.
 */
stock ObjectConstraintResult:ObjLib_HandleArrayConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        keyIndex,
        const any:value[],
        size,
        ObjLib_ErrorHandler:customErrorHandler = INVALID_FUNCTION,
        bool:isLookup = false)
{
    // Get constraints object.
    new Object:constraints = ObjLib_GetTypeConstraintsAt(typeDescriptor, keyIndex);
    
    if (constraints == INVALID_OBJECT)
    {
        // No constraints.
        return ObjConstraintResult_Valid;
    }
    
    // Get error handler.
    new ObjLib_ErrorHandler:errorHandler = ObjLib_GetErrorHandler(typeDescriptor, customErrorHandler);
    
    // Delegate work to a handler that matches the data type.
    new ObjectType:constraintsType = ObjLib_GetTypeDescriptor(constraints);
    if (constraintsType == ObjLib_CustomConstraints)
    {
        return ObjLib_HandleCustomConstraints(object, typeDescriptor, constraints, keyIndex, value, size, errorHandler);
    }
    else if (constraintsType == ObjLib_LookupConstraints)
    {
        // Check if this is a special lookup case.
        if (isLookup)
        {
            // Call lookup constraint handler.
            return ObjLib_HandleLookupConstraints(object, typeDescriptor, constraints, keyIndex, value, errorHandler);
        }
        else
        {
            // Not a lookup case, or user is using something else than
            // SetString. Check if sub constraints are available.
            new Object:subConstraints = ObjLib_GetObject(constraints, "subConstraints");
            if (subConstraints != INVALID_OBJECT)
            {
                // Recursive re-entry for handling raw data constraints. (Recursion
                // is stopped by preventing lookup contraint objects in
                // subConstraints when building the constraint object.)
                return ObjLib_HandleArrayConstraints(object, typeDescriptor, keyIndex, value, size, customErrorHandler);
            }
            else
            {
                // No sub constraints.
                return ObjConstraintResult_Valid;
            }
        }
    }
    else if (constraintsType == ObjLib_StringConstraints)
    {
        return ObjLib_HandleStringConstraints(object, typeDescriptor, constraints, keyIndex, value, errorHandler);
    }
    else if (constraintsType == ObjLib_ArrayConstraints)
    {
        return ObjLib_HandleArrayTypeConstr(object, typeDescriptor, constraints, keyIndex, value, size, errorHandler);
    }
    else
    {
        // Invalid constraints object type.
        ThrowError("[BUG] Invalid constraints object type. They are validated so this is a bug in objectlib.");
        return ObjConstraintResult_Invalid;
    }
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleCustomConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        const any:value[],
        size,
        ObjLib_ErrorHandler:errorHandler)
{
    // Disable constraints if this handler is already working.
    if (ObjLib_InCustomHandler)
    {
        return ObjConstraintResult_Valid;
    }
    ObjLib_InCustomHandler = true;
    
    // Get callback.
    new ObjLib_KeyValidator:callback = ObjLib_KeyValidator:ObjLib_GetFunction(constraints, "callback");
    
    // Get Key name.
    new String:keyName[OBJECT_KEY_NAME_LEN];
    new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
    GetArrayString(keys, keyIndex, keyName, sizeof(keyName));
    
    // Get data type.
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    new ObjectDataType:dataType = ObjectDataType:GetArrayCell(dataTypes, keyIndex);
    
    // Call custom validator.
    // functag public ObjectConstraintResult:ObjLib_KeyValidator(Object:object, ObjectType:typeDescriptor, const String:keyName[], ObjectDataType:dataType, const any:values[], size);
    new ObjectConstraintResult:result = ObjConstraintResult_Invalid;
    Call_StartFunction(GetMyHandle(), callback);
    Call_PushCell(object);
    Call_PushCell(typeDescriptor);
    Call_PushString(keyName);
    Call_PushCell(dataType);
    Call_PushArray(value, size);
    Call_PushCell(size);
    Call_Finish(result);
    
    ObjLib_InCustomHandler = false;
    return result;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleLookupConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        const String:lookup[],
        ObjLib_ErrorHandler:errorHandler)
{
    // Disable constraints if this handler is already working.
    if (ObjLib_InLookupHandler)
    {
        return ObjConstraintResult_Valid;
    }
    ObjLib_InLookupHandler = true;
    
    // Get raw data type for key.
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    new ObjectDataType:dataType = ObjectDataType:GetArrayCell(dataTypes, keyIndex);
    
    // Get constraints.
    new ObjectLookupMethod:method = ObjectLookupMethod:ObjLib_GetCell(constraints, "method");
    new Handle:entries = ObjLib_GetHandle(constraints, "entries");
    new Handle:values = ObjLib_GetHandle(constraints, "values");
    new ObjLibLookupCallback:callback = ObjLib_GetFunction(constraints, "callback");
    
    // Buffer to store replacement value.
    decl any:buffer[OBJLIB_MAX_STRING_LEN];
    buffer[0] = 0;
    new numWritten = -1;
    
    // Delegate lookup to separate helpers according to lookup method.
    switch (method)
    {
        case ObjLookupMethod_Array:
        {
            numWritten = ObjLib_HandleArrayLookup(dataType, entries, values, lookup, buffer, sizeof(buffer));
        }
        case ObjLookupMethod_Trie:
        {
            numWritten = ObjLib_HandleTrieLookup(dataType, entries, lookup, buffer, sizeof(buffer));
        }
        case ObjLookupMethod_Callback:
        {
            // Get key name.
            decl String:keyName[OBJECT_KEY_NAME_LEN];
            keyName[0] = 0;
            GetArrayString(ObjLib_GetTypeKeys(typeDescriptor), keyIndex, keyName, sizeof(keyName));
            
            numWritten = ObjLib_HandleCallbackLookup(object, dataType, callback, keyName, lookup, buffer, sizeof(buffer));
        }
    }
    
    // Check if failed.
    if (numWritten < 0)
    {
        // Throw constraint error. Format a nice message matching the constraint
        // error handler. ("Value violated constraint: <buffer>")
        Format(buffer, sizeof(buffer), "Lookup. Invalid lookup entry (\"%s\").", lookup);
        ObjLib_HandleConstraintError(lookup, object, typeDescriptor, keyIndex, errorHandler);
        
        ObjLib_InLookupHandler = false;
        return ObjConstraintResult_Invalid;
    }
    
    // Set value (implies subConstraints if available, automatic).
    switch (dataType)
    {
        case ObjDataType_Any:
        {
            ObjLib_SetAnyAt(object, keyIndex, buffer[0], errorHandler);
        }
        case ObjDataType_Cell:
        {
            ObjLib_SetCellAt(object, keyIndex, buffer[0], errorHandler);
        }
        case ObjDataType_Bool:
        {
            ObjLib_SetBoolAt(object, keyIndex, buffer[0], errorHandler);
        }
        case ObjDataType_Float:
        {
            ObjLib_SetFloatAt(object, keyIndex, buffer[0], errorHandler);
        }
        case ObjDataType_Handle:
        {
            ObjLib_SetHandleAt(object, keyIndex, buffer[0], errorHandler);
        }
        case ObjDataType_Function:
        {
            ObjLib_SetFunctionAt(object, keyIndex, buffer[0], errorHandler);
        }
        case ObjDataType_Array:
        {
            ObjLib_SetArrayAt(object, keyIndex, buffer, sizeof(buffer), errorHandler);
        }
        case ObjDataType_String:
        {
            ObjLib_SetStringAt(object, keyIndex, buffer, errorHandler);
        }
        case ObjDataType_Object:
        {
            ObjLib_SetObjectAt(object, keyIndex, buffer[0], errorHandler);
        }
        case ObjDataType_ObjectType:
        {
            ObjLib_SetObjectTypeAt(object, keyIndex, buffer[0], errorHandler);
        }
        default:
        {
            // Unexpected type.
            ThrowError("[BUG] Unexpected data type. This is a bug in objectlib.");
        }
    }
    
    // Value is overridden and stored.
    ObjLib_InLookupHandler = false;
    return ObjConstraintResult_Overridden;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Helper for lookup constraint handler. Finds a lookup entry in an array and
 * copies the replacement value according to the data type specified.
 *
 * @param dataType      Data type of replacement values (match the object key's
 *                      data type).
 * @param entries       ADT array with list of string lookup entries.
 * @param values        ADT array with list of replacement values, matching
 *                      the data type.
 * @param lookup        Lookup entry to search for.
 * @param buffer        Buffer to store lookup result. Values are stored at the
 *                      first index. Strings and arrays stored as usual.
 * @param maxlen        Size of the buffer. Strings and array replacements are
 *                      truncated if the buffer is too small.
 *
 * @return              Number of cells or characters copied to buffer
 *                      (depending on data type), or -1 if lookup entry wasn't
 *                      found.
 */
stock ObjLib_HandleArrayLookup(
        ObjectDataType:dataType,
        Handle:entries,
        Handle:values,
        const String:lookup[],
        any:buffer[],
        maxlen)
{
    // Validate buffer size.
    if (maxlen <= 0)
    {
        ThrowError("Buffer size cannot be zero.");
        return -1;
    }
    
    // Entry buffer.
    decl String:entry[OBJLIB_MAX_STRING_LEN];
    entry[0] = 0;
    
    // Search the entry array for the lookup string.
    new entryIndex = FindStringInArray(entries, lookup);
    
    // Check if found.
    if (entryIndex >= 0)
    {
        // Get entry.
        GetArrayString(entries, entryIndex, entry, sizeof(entry));
        
        // Copy replacement value to result buffer.
        if (dataType == ObjDataType_Array)
        {
            return GetArrayArray(values, entryIndex, buffer, maxlen);
        }
        else if (dataType == ObjDataType_String)
        {
            return GetArrayString(values, entryIndex, buffer, maxlen);
        }
        else
        {
            buffer[0] = GetArrayCell(values, entryIndex);
            return 1;
        }
    }
    
    // Not found.
    return -1;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Helper for lookup constraint handler. Looks up an entry in a trie and copies
 * replacement value according to the data type specified.
 *
 * @param dataType      Data type of replacement values (match the object key's
 *                      data type).
 * @param entries       ADT trie with strings mapped to replacement values
 *                      (matching the data type).
 * @param lookup        Lookup entry to search for.
 * @param buffer        Buffer to store lookup result. Values are stored at the
 *                      first index. Strings and arrays stored as usual.
 * @param maxlen        Size of the buffer. Strings and array replacements are
 *                      truncated if the buffer is too small.
 *
 * @return              Number of cells or characters copied to buffer
 *                      (depending on data type), or -1 if lookup entry wasn't
 *                      found.
 */
stock ObjLib_HandleTrieLookup(
        ObjectDataType:dataType,
        Handle:entries,
        const String:lookup[],
        any:buffer[],
        maxlen)
{
    // Validate buffer size.
    if (maxlen <= 0)
    {
        ThrowError("Buffer size cannot be zero.");
        return -1;
    }
    
    // Entry buffer.
    decl String:entry[OBJLIB_MAX_STRING_LEN];
    entry[0] = 0;
    
    new bool:found = false;
    new numWritten = 0;
    
    // Get replacement value and copy to result buffer.
    if (dataType == ObjDataType_Array)
    {
        found = GetTrieArray(entries, lookup, buffer, maxlen, numWritten);
    }
    else if (dataType == ObjDataType_String)
    {
        found = GetTrieString(entries, lookup, buffer, maxlen, numWritten);
    }
    else
    {
        found = GetTrieValue(entries, lookup, buffer[0]);
        numWritten = 1;
    }
    
    return found ? numWritten : -1;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Helper for lookup constraint handler. Looks up an entry through a callback
 * and copies replacement value according to the data type specified.
 *
 * @param object        Object being validated.
 * @param dataType      Data type of replacement values (match the object key's
 *                      data type).
 * @param callback      Lookup callback to use.
 * @param lookup        Lookup entry to search for.
 * @param buffer        Buffer to store lookup result. Values are stored at the
 *                      first index. Strings and arrays stored as usual.
 * @param maxlen        Size of the buffer. Strings and array replacements are
 *                      truncated if the buffer is too small.
 *
 * @return              Number of cells or characters copied to buffer
 *                      (depending on data type), or -1 if lookup entry wasn't
 *                      found.
 */
stock ObjLib_HandleCallbackLookup(
        Object:object,
        ObjectDataType:dataType,
        ObjLibLookupCallback:callback,
        const String:keyName[],
        const String:lookup[],
        any:buffer[],
        maxlen)
{
    // Prepare call to the callback.
    Call_StartFunction(GetMyHandle(), callback);
    
    /*
    Callback signatures
    
    Cell:
    bool:public(Object:object, const String:keyName[], const String:lookup[], &any:replacement)
    
    Array:
    public(Object:object, const String:keyName[], const String:lookup[], any:replacement[], maxlen)
    
    String:
    public(Object:object, const String:keyName[], const String:lookup[], String:replacement[], maxlen)
    */

    // Push common parameters.    
    Call_PushCell(object);
    Call_PushString(keyName);
    Call_PushString(lookup);
    
    // Push parameters according to the data type (cell, array, string).
    if (dataType == ObjDataType_Array)
    {
        Call_PushArrayEx(buffer, maxlen, SM_PARAM_COPYBACK);
        Call_PushCell(maxlen);
    }
    else if (dataType == ObjDataType_String)
    {
        Call_PushStringEx(buffer, maxlen, SM_PARAM_STRING_UTF8, SM_PARAM_COPYBACK);
        Call_PushCell(maxlen);
    }
    else
    {
        Call_PushCellRef(buffer[0]);
    }
    
    // Call callback.
    new result;
    Call_Finish(result);
    
    // Convert and return result from callback.
    if (dataType == ObjDataType_Array || dataType == ObjDataType_String)
    {
        // Returned number of cells/characters copied.
        return result;
    }
    else
    {
        // Returned true if valid.
        return bool:result ? 1 : -1;
    }
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleCellConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        value,
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new min = ObjLib_GetCell(constraints, "min");
    new max = ObjLib_GetCell(constraints, "max");
    new bool:lowerLimit = ObjLib_GetBool(constraints, "lowerLimit");
    new bool:upperLimit = ObjLib_GetBool(constraints, "upperLimit");
    new bool:nonzero = ObjLib_GetBool(constraints, "nonzero");
    
    // Validate lower limit.
    if (lowerLimit && value < min)
    {
        ObjLib_HandleConstraintError("min", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate upper limit.
    if (upperLimit && value > max)
    {
        ObjLib_HandleConstraintError("max", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Check if zero.
    if (nonzero && value == 0)
    {
        ObjLib_HandleConstraintError("nonzero", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleFloatConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        Float:value,
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new Float:min = ObjLib_GetFloat(constraints, "min");
    new Float:max = ObjLib_GetFloat(constraints, "max");
    new bool:lowerLimit = ObjLib_GetBool(constraints, "lowerLimit");
    new bool:upperLimit = ObjLib_GetBool(constraints, "upperLimit");
    new bool:nonzero = ObjLib_GetBool(constraints, "nonzero");
    new Float:zeroDelta = ObjLib_GetFloat(constraints, "zeroDelta");
    
    // Validate lower limit.
    if (lowerLimit && value < min)
    {
        ObjLib_HandleConstraintError("min", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate upper limit.
    if (upperLimit && value > max)
    {
        ObjLib_HandleConstraintError("max", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Check if zero.
    if (nonzero && (value == 0 || FloatAbs(value) < zeroDelta))
    {
        ObjLib_HandleConstraintError("nonzero", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleHandleConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        Handle:value,
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new bool:nonzero = ObjLib_GetBool(constraints, "nonzero");
    
    // Check if zero.
    if (nonzero && value != INVALID_HANDLE)
    {
        ObjLib_HandleConstraintError("nonzero", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}



/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleFunctionConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        Function:value,
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new bool:nonzero = ObjLib_GetBool(constraints, "nonzero");
    
    // Check if nonzero.
    if (nonzero && value != INVALID_FUNCTION)
    {
        ObjLib_HandleConstraintError("nonzero", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleStringConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        const String:value[],
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new bool:nonempty = ObjLib_GetBool(constraints, "nonempty");
    new bool:lowerLimit = ObjLib_GetBool(constraints, "lowerLimit");
    new bool:upperLimit = ObjLib_GetBool(constraints, "upperLimit");
    new minLen = ObjLib_GetCell(constraints, "minLen");
    new maxLen = ObjLib_GetCell(constraints, "maxLen");
    new bool:pathValidation = ObjLib_GetBool(constraints, "pathValidation");
    new bool:fileValidation = ObjLib_GetBool(constraints, "fileValidation");
    new bool:includeValveFS = ObjLib_GetBool(constraints, "includeValveFS");
    //new bool:whitelist = ObjLib_GetBool(constraints, "whitelist");
    //new bool:blacklist = ObjLib_GetBool(constraints, "blacklist");
    
    // TODO
    /*decl String:whitelistChars[OBJECTLIB_WHITELIST_LEN];
    decl String:blacklistChars[OBJECTLIB_WHITELIST_LEN];
    ObjLib_GetString(constraint, "whitelistChars", whitelistChars, sizeof(whitelistChars));
    ObjLib_GetString(constraint, "blacklistChars", blacklistChars, sizeof(blacklistChars));*/
    
    new valueLen = strlen(value);
    
    // Check if empty.
    if (nonempty && valueLen == 0)
    {
        ObjLib_HandleConstraintError("nonempty", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate lower limit.
    if (lowerLimit && valueLen < minLen)
    {
        ObjLib_HandleConstraintError("minLen", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate upper limit.
    if (upperLimit && valueLen > maxLen)
    {
        ObjLib_HandleConstraintError("maxLen", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate path.
    if (pathValidation && !DirExists(value))
    {
        ObjLib_HandleConstraintError("pathValidation", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate file.
    if (fileValidation && !FileExists(value, includeValveFS))
    {
        ObjLib_HandleConstraintError("fileValidation", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // TODO: Check whitelisted and blacklisted characters.
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleArrayTypeConstr(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        const any:value[],
        size,
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new bool:lowerLimit = ObjLib_GetBool(constraints, "lowerLimit");
    new bool:upperLimit = ObjLib_GetBool(constraints, "upperLimit");
    new minLen = ObjLib_GetCell(constraints, "minLen");
    new maxLen = ObjLib_GetCell(constraints, "maxLen");
    
    // Validate lower limit.
    if (lowerLimit && size < minLen)
    {
        ObjLib_HandleConstraintError("minLen", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Validate upper limit.
    if (upperLimit && size > maxLen)
    {
        ObjLib_HandleConstraintError("maxLen", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleObjectConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        Object:value,
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new bool:nonzero = ObjLib_GetBool(constraints, "nonzero");
    new ObjectType:type = ObjLib_GetObjectType(constraints, "type");
    
    // Check if zero.
    if (nonzero && value == INVALID_OBJECT)
    {
        ObjLib_HandleConstraintError("nonzero", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Check type.
    if (type != INVALID_OBJECT_TYPE)
    {
        new ObjectType:valueType = ObjLib_GetTypeDescriptor(value);
        if (valueType != type)
        {
            ObjLib_HandleConstraintError("type", object, typeDescriptor, keyIndex, errorHandler);
            return ObjConstraintResult_Invalid;
        }
    }
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}



/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjectConstraintResult:ObjLib_HandleObjTypeConstraints(
        Object:object,
        ObjectType:typeDescriptor,
        Object:constraints,
        keyIndex,
        ObjectType:value,
        ObjLib_ErrorHandler:errorHandler)
{
    // Get constraints.
    new bool:nonzero = ObjLib_GetBool(constraints, "nonzero");
    new bool:requireKeys = ObjLib_GetBool(constraints, "requireKeys");
    new Handle:keys = ObjLib_GetHandle(constraints, "keys");
    new Handle:dataTypes = ObjLib_GetHandle(constraints, "dataTypes");
    
    decl String:constraintName[16];
    constraintName[0] = 0;
    
    // Check if zero.
    if (nonzero && value == INVALID_OBJECT_TYPE)
    {
        ObjLib_HandleConstraintError("nonzero", object, typeDescriptor, keyIndex, errorHandler);
        return ObjConstraintResult_Invalid;
    }
    
    // Check keys.
    if (requireKeys)
    {
        // Get data types.
        new Handle:valueDataTypes = ObjLib_GetTypeDataTypes(value);
        
        // Loop through keys.
        new len = GetArraySize(keys);
        for (new i = 0; i < len; i++)
        {
            // Get key name.
            decl String:keyName[OBJECT_KEY_NAME_LEN];
            GetArrayString(keys, i, keyName, sizeof(keyName));
            
            // Check if it exists.
            new key = ObjLib_GetKeyIndexFromType(typeDescriptor, keyName);
            if (key < 0)
            {
                ObjLib_HandleConstraintError("requireKeys", object, typeDescriptor, keyIndex, errorHandler);
                return ObjConstraintResult_Invalid;
            }
            else
            {
                // Check data type.
                if (GetArrayCell(valueDataTypes, key) != GetArrayCell(dataTypes, i))
                {
                    Format(constraintName, sizeof(constraintName), "dataTypes[%d]", i);
                    ObjLib_HandleConstraintError(constraintName, object, typeDescriptor, keyIndex, errorHandler);
                    return ObjConstraintResult_Invalid;
                }
            }
        }
    }
    
    // Passed validation.
    return ObjConstraintResult_Valid;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock bool:ObjLib_HandleConstraintError(const String:constraintName[], Object:object, ObjectType:typeDescriptor, keyIndex, ObjLib_ErrorHandler:errorHandler)
{
    // Get Key name.
    new String:keyName[OBJECT_KEY_NAME_LEN];
    new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
    GetArrayString(keys, keyIndex, keyName, sizeof(keyName));
    
    return ObjLib_HandleError(typeDescriptor, object, ObjLibError_ValidationError, errorHandler, _, _, "Cannot set value in key (\"%s\"). Value violated constraint: %s", keyName, constraintName);
}
