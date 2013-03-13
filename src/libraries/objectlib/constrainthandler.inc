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
 * Internal use only!
 *
 * @return              True if validation passed.
 */
stock bool:ObjLib_HandleValueConstraints(Object:object,
                                         ObjectType:typeDescriptor,
                                         keyIndex,
                                         any:value,
                                         ObjLib_ErrorHandler:customErrorHandler = INVALID_FUNCTION)
{
    // Get constraints object.
    new Object:constraints = ObjLib_GetTypeConstraintsAt(ObjectType:typeDescriptor, keyIndex);
    
    if (constraints == INVALID_OBJECT)
    {
        // No constraints.
        return true;
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
        return false;
    }
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock bool:ObjLib_HandleArrayConstraints(Object:object,
                                         ObjectType:typeDescriptor,
                                         keyIndex,
                                         const any:value[],
                                         size,
                                         ObjLib_ErrorHandler:customErrorHandler = INVALID_FUNCTION)
{
    // Get constraints object.
    new Object:constraints = ObjLib_GetTypeConstraintsAt(ObjectType:typeDescriptor, keyIndex);
    
    if (constraints == INVALID_OBJECT)
    {
        // No constraints.
        return true;
    }
    
    // Get error handler.
    new ObjLib_ErrorHandler:errorHandler = ObjLib_GetErrorHandler(typeDescriptor, customErrorHandler);
    
    // Delegate work to a handler that matches the data type.
    new ObjectType:constraintsType = ObjLib_GetTypeDescriptor(constraints);
    if (constraintsType == ObjLib_CustomConstraints)
    {
        return ObjLib_HandleCustomConstraints(object, typeDescriptor, constraints, keyIndex, value, size, errorHandler);
    }
    else if (constraintsType == ObjLib_StringConstraints)
    {
        return ObjLib_HandleStringConstraints(object, typeDescriptor, constraints, keyIndex, value, size, errorHandler);
    }
    else if (constraintsType == ObjLib_ArrayConstraints)
    {
        return ObjLib_HandleArrayTypeConstr(object, typeDescriptor, constraints, keyIndex, value, size, errorHandler);
    }
    else
    {
        // Invalid constraints object type.
        ThrowError("[BUG] Invalid constraints object type. They are validated so this is a bug in objectlib.");
        return false;
    }
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock bool:ObjLib_HandleCustomConstraints(Object:object,
                                          ObjectType:typeDescriptor,
                                          Object:constraints,
                                          keyIndex,
                                          const any:value[],
                                          size,
                                          ObjLib_ErrorHandler:errorHandler)
{
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
    // functag public bool:ObjLib_KeyValidator(Object:object, const String:keyName[], ObjectDataType:dataType, const any:value[], size, ObjectType:typeDescriptor);
    new bool:result = false;
    Call_StartFunction(GetMyHandle(), callback);
    Call_PushCell(object);
    Call_PushString(keyName);
    Call_PushCell(dataType);
    Call_PushArray(value, size);
    Call_PushCell(size);
    Call_PushCell(typeDescriptor);
    Call_Finish(result);
    
    return result;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock bool:ObjLib_HandleCellConstraints(Object:object,
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
        return false;
    }
    
    // Validate upper limit.
    if (upperLimit && value > max)
    {
        ObjLib_HandleConstraintError("max", object, typeDescriptor, keyIndex, errorHandler);
        return false;
    }
    
    // Check if zero.
    if (nonzero && value == 0)
    {
        ObjLib_HandleConstraintError("nonzero", object, typeDescriptor, keyIndex, errorHandler);
        return false;
    }
    
    // Passed validation.
    return true;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock bool:ObjLib_HandleFloatConstraints(Object:object,
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
        return false;
    }
    
    // Validate upper limit.
    if (upperLimit && value > max)
    {
        ObjLib_HandleConstraintError("max", object, typeDescriptor, keyIndex, errorHandler);
        return false;
    }
    
    // Check if zero.
    if (nonzero && (value == 0 || FloatAbs(value) < zeroDelta))
    {
        ObjLib_HandleConstraintError("nonzero", object, typeDescriptor, keyIndex, errorHandler);
        return false;
    }
    
    // Passed validation.
    return true;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock bool:ObjLib_HandleHandleConstraints(Object:object,
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
        return false;
    }
    
    // Passed validation.
    return true;
}



/*____________________________________________________________________________*/

/** Internal use only! */
stock bool:ObjLib_HandleFunctionConstraints(Object:object,
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
        return false;
    }
    
    // Passed validation.
    return true;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock bool:ObjLib_HandleStringConstraints(Object:object,
                                        ObjectType:typeDescriptor,
                                        Object:constraints,
                                        keyIndex,
                                        const String:value[],
                                        size,
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
        return false;
    }
    
    // Validate lower limit.
    if (lowerLimit && valueLen < minLen)
    {
        ObjLib_HandleConstraintError("minLen", object, typeDescriptor, keyIndex, errorHandler);
        return false;
    }
    
    // Validate upper limit.
    if (upperLimit && valueLen > maxLen)
    {
        ObjLib_HandleConstraintError("maxLen", object, typeDescriptor, keyIndex, errorHandler);
        return false;
    }
    
    // Validate path.
    if (pathValidation && !DirExists(value))
    {
        ObjLib_HandleConstraintError("pathValidation", object, typeDescriptor, keyIndex, errorHandler);
        return false;
    }
    
    // Validate file.
    if (fileValidation && !FileExists(value, includeValveFS))
    {
        ObjLib_HandleConstraintError("fileValidation", object, typeDescriptor, keyIndex, errorHandler);
        return false;
    }
    
    // TODO: Check whitelisted and blacklisted characters.
    
    // Passed validation.
    return true;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock bool:ObjLib_HandleArrayTypeConstr(Object:object,
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
        return false;
    }
    
    // Validate upper limit.
    if (upperLimit && size > maxLen)
    {
        ObjLib_HandleConstraintError("maxLen", object, typeDescriptor, keyIndex, errorHandler);
        return false;
    }
    
    // Passed validation.
    return true;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock bool:ObjLib_HandleObjectConstraints(Object:object,
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
        return false;
    }
    
    // Check type.
    if (type != INVALID_OBJECT_TYPE)
    {
        new ObjectType:valueType = ObjLib_GetTypeDescriptor(value);
        if (valueType != type)
        {
            ObjLib_HandleConstraintError("type", object, typeDescriptor, keyIndex, errorHandler);
            return false;
        }
    }
    
    // Passed validation.
    return true;
}



/*____________________________________________________________________________*/

/** Internal use only! */
stock bool:ObjLib_HandleObjTypeConstraints(Object:object,
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
        return false;
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
                return false;
            }
            else
            {
                // Check data type.
                if (GetArrayCell(valueDataTypes, key) != GetArrayCell(dataTypes, i))
                {
                    Format(constraintName, sizeof(constraintName), "dataTypes[%d]", i);
                    ObjLib_HandleConstraintError(constraintName, object, typeDescriptor, keyIndex, errorHandler);
                    return false;
                }
            }
        }
    }
    
    // Passed validation.
    return true;
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
