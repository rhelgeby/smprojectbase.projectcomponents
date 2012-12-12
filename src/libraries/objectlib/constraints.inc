/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           constraints.inc
 *  Type:           Library
 *  Description:    Validation constraints for objectlib.
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
 * @section Internal use only!
 *          Global constraint type descriptors. Created automatically.
 */
new ObjectType:ObjLib_CellConstraints       = INVALID_OBJECT_TYPE;
new ObjectType:ObjLib_FloatConstraints      = INVALID_OBJECT_TYPE;
new ObjectType:ObjLib_HandleConstraints     = INVALID_OBJECT_TYPE;
new ObjectType:ObjLib_FunctionConstraints   = INVALID_OBJECT_TYPE;
new ObjectType:ObjLib_StringConstraints     = INVALID_OBJECT_TYPE;
new ObjectType:ObjLib_ArrayConstraints      = INVALID_OBJECT_TYPE;
new ObjectType:ObjLib_ObjectConstraints     = INVALID_OBJECT_TYPE;
new ObjectType:ObjLib_ObjectTypeConstraints = INVALID_OBJECT_TYPE;
new ObjectType:ObjLib_CustomConstraints     = INVALID_OBJECT_TYPE;
new bool:ObjLib_ConstraintTypesBuilt        = false;
/**
 * @endsection
 */

/*____________________________________________________________________________*/


/***************************
 *   Constraint builders   *
 ***************************/

/**
 * Creates a cell constraint object. All parameters are optional and defaults to
 * no limit.
 *
 * @param nonzero       Cannot be zero.
 * @param lowerLimit    Enable lower limit.
 * @param upperLimit    Enable upper limit.
 * @param min           Lower limit. Cannot be higher than max.
 * @param max           Upper limit. Cannot be lower than min.
 *
 * @return              Constraint object. Use this once when adding a key.
 *                      Object must be deleted if not used anywhere.
 */
stock Object:ObjLib_GetCellConstraints(bool:nonzero = false,
                                       bool:lowerLimit = false,
                                       bool:upperLimit = false,
                                       min = 0,
                                       max = 0)
{
    // Make sure constraint types exist.
    ObjLib_BuildConstraintTypes();
    
    // Validate min and max.
    if (min > max || max < min)
    {
        ThrowError("Invalid min (%d) or max (%d) value. They cannot overlap eachother.", min, max);
    }
    
    new Object:constraint = ObjLib_CreateObject(ObjLib_CellConstraints, false);
    
    ObjLib_SetCell(constraint, "min", min);
    ObjLib_SetCell(constraint, "max", max);
    ObjLib_SetBool(constraint, "lowerLimit", lowerLimit);
    ObjLib_SetBool(constraint, "upperLimit", upperLimit);
    ObjLib_SetBool(constraint, "nonzero", nonzero);
    
    return constraint;
}

/*____________________________________________________________________________*/

/**
 * Creates a float constraint object. All parameters are optional and defaults
 * to no limit.
 *
 * @param nonzero       Cannot be zero.
 * @param lowerLimit    Enable lower limit.
 * @param upperLimit    Enable upper limit.
 * @param min           Lower limit. Cannot be higher than max.
 * @param max           Upper limit. Cannot be lower than min.
 * @param zeroDelta     Maximum difference allowed when comparing against zero.
 *
 * @return              Constraint object. Use this once when adding a key.
 *                      Object must be deleted if not used anywhere.
 */
stock Object:ObjLib_GetFloatConstraints(bool:nonzero = false,
                                        bool:lowerLimit = false,
                                        bool:upperLimit = false,
                                        Float:min = 0.0,
                                        Float:max = 0.0,
                                        Float:zeroDelta = 0.0001)
{
    // Make sure constraint types exist.
    ObjLib_BuildConstraintTypes();
    
    // Validate min and max.
    if (min > max || max < min)
    {
        ThrowError("Invalid min (%d) or max (%d) value. They cannot overlap eachother.", min, max);
    }
    
    new Object:constraint = ObjLib_CreateObject(ObjLib_FloatConstraints, false);
    
    ObjLib_SetFloat(constraint, "min", min);
    ObjLib_SetFloat(constraint, "max", max);
    ObjLib_SetBool(constraint, "lowerLimit", lowerLimit);
    ObjLib_SetBool(constraint, "upperLimit", upperLimit);
    ObjLib_SetFloat(constraint, "zeroDelta", zeroDelta);
    ObjLib_SetBool(constraint, "nonzero", nonzero);
    
    return constraint;
}

/*____________________________________________________________________________*/

/**
 * Creates a handle constraint object. All parameters are optional and defaults
 * to no limit.
 *
 * @param nonzero       Cannot be zero (INVALID_HANDLE).
 *
 * @return              Constraint object. Use this once when adding a key.
 *                      Object must be deleted if not used anywhere.
 */
stock Object:ObjLib_GetHandleConstraints(bool:nonzero = false)
{
    // Make sure constraint types exist.
    ObjLib_BuildConstraintTypes();
    
    new Object:constraint = ObjLib_CreateObject(ObjLib_HandleConstraints, false);
    
    ObjLib_SetBool(constraint, "nonzero", nonzero);
    
    return constraint;
}

/*____________________________________________________________________________*/

/**
 * Creates a function constraint object. All parameters are optional and
 * defaults to no limit.
 *
 * @param nonzero       Cannot be zero (INVALID_FUNCTION).
 *
 * @return              Constraint object. Use this once when adding a key.
 *                      Object must be deleted if not used anywhere.
 */
stock Object:ObjLib_GetFunctionConstraints(bool:nonzero = false)
{
    // Make sure constraint types exist.
    ObjLib_BuildConstraintTypes();
    
    new Object:constraint = ObjLib_CreateObject(ObjLib_FunctionConstraints, false);
    
    ObjLib_SetBool(constraint, "nonzero", nonzero);
    
    return constraint;
}

/*____________________________________________________________________________*/

/**
 * Creates a string constraint object. All parameters are optional and defaults
 * to no limit.
 *
 * @param nonempty          Cannot be empty.
 * @param lowerLimit        Enable minimum length limit.
 * @param upperLimit        Enable maximum lenght limit.
 * @param min               Minimum length. Cannot be higher than max.
 * @param max               Maximum length. Cannot be lower than min.
 * @param pathValidation    Validate as directory path. Cannot be used with fileValidation.
 * @param fileValidation    Validate as file path. Cannot be used with pathValidation.
 * @param includeValveFS    Include files in Valve's file system when using fileValidation.
 * @param whitelist         Enable whitelisting of characters.
 * @param blacklist         Enable blacklisting of characters.
 * @param whitelistChars    Whitelisted characters.
 * @param blacklistChars    Blacklisted characters.
 *
 * @return                  Constraint object. Use this once when adding a key.
 *                          Object must be deleted if not used anywhere.
 */
stock Object:ObjLib_GetStringConstraints(bool:nonempty = false,
                                         bool:lowerLimit = false,
                                         bool:upperLimit = false,
                                         minLen = 0,
                                         maxLen = 32,
                                         bool:pathValidation = false,
                                         bool:fileValidation = false,
                                         bool:includeValveFS = false,
                                         bool:whitelist = false,
                                         bool:blacklist = false,
                                         const String:whitelistChars[] = "",
                                         const String:blacklistChars[] = "")
{
    // Make sure constraint types exist.
    ObjLib_BuildConstraintTypes();
    
    // Validate min and max.
    if (minLen > maxLen || maxLen < minLen)
    {
        ThrowError("Invalid min (%d) or max (%d) value. They cannot overlap eachother.", minLen, maxLen);
    }
    
    new Object:constraint = ObjLib_CreateObject(ObjLib_StringConstraints, false);
    
    ObjLib_SetBool(constraint, "nonempty", nonempty);
    ObjLib_SetBool(constraint, "lowerLimit", lowerLimit);
    ObjLib_SetBool(constraint, "upperLimit", upperLimit);
    ObjLib_SetCell(constraint, "minLen", minLen);
    ObjLib_SetCell(constraint, "maxLen", maxLen);
    ObjLib_SetBool(constraint, "pathValidation", pathValidation);
    ObjLib_SetBool(constraint, "fileValidation", fileValidation);
    ObjLib_SetBool(constraint, "includeValveFS", includeValveFS);
    ObjLib_SetBool(constraint, "whitelist", whitelist);
    ObjLib_SetString(constraint, "whitelistChars", whitelistChars);
    ObjLib_SetString(constraint, "blacklistChars", blacklistChars);
    
    return constraint;
}

/*____________________________________________________________________________*/

/**
 * Creates a array constraint object. All parameters are optional and defaults
 * to no limit.
 *
 * @param lowerLimit    Enable minimum length limit.
 * @param upperLimit    Enable maximum lenght limit.
 * @param min           Minimum length. Cannot be higher than max.
 * @param max           Maximum length. Cannot be lower than min.
 *
 * @return              Constraint object. Use this once when adding a key.
 *                      Object must be deleted if not used anywhere.
 */
stock Object:ObjLib_GetArrayConstraints(bool:nonempty = false,
                                        bool:lowerLimit = false,
                                        bool:upperLimit = false,
                                        minLen = 0,
                                        maxLen = 32)
{
    // Make sure constraint types exist.
    ObjLib_BuildConstraintTypes();
    
    // Validate min and max.
    if (minLen > maxLen || maxLen < minLen)
    {
        ThrowError("Invalid min (%d) or max (%d) value. They cannot overlap eachother.", minLen, maxLen);
    }
    
    new Object:constraint = ObjLib_CreateObject(ObjLib_ArrayConstraints, false);
    
    ObjLib_SetBool(constraint, "lowerLimit", lowerLimit);
    ObjLib_SetBool(constraint, "upperLimit", upperLimit);
    ObjLib_SetCell(constraint, "minLen", minLen);
    ObjLib_SetCell(constraint, "maxLen", maxLen);
    
    return constraint;
}

/*____________________________________________________________________________*/

/**
 * Creates a object constraint object. All parameters are optional and defaults
 * to no limit.
 *
 * @param nonzero       Cannot be zero (INVALID_OBJECT).
 * @param type          Must be a certain type. Use INVALID_OBJECT_TYPE to skip
 *                      type validation.
 *
 * @return              Constraint object. Use this once when adding a key.
 *                      Object must be deleted if not used anywhere.
 */
stock Object:ObjLib_GetObjectConstraints(bool:nonzero = false,
                                         ObjectType:type = INVALID_OBJECT_TYPE)
{
    // Make sure constraint types exist.
    ObjLib_BuildConstraintTypes();
    
    new Object:constraint = ObjLib_CreateObject(ObjLib_ObjectConstraints, false);
    
    ObjLib_SetBool(constraint, "nonzero", nonzero);
    ObjLib_SetObjectType(constraint, "type", type);
    
    return constraint;
}

/*____________________________________________________________________________*/

/**
 * Creates a object type constraint object. All parameters are optional and defaults
 * to no limit.
 *
 * @param nonzero       Cannot be zero (INVALID_OBJECT).
 * @param type          Must be a certain type. Use INVALID_OBJECT_TYPE to skip
 *                      type validation.
 *
 * @return              Constraint object. Use this once when adding a key.
 *                      Object must be deleted if not used anywhere.
 */
stock Object:ObjLib_GetObjTypeConstraints(bool:nonzero = false,
                                          bool:requireKeys = false,
                                          Handle:keys = INVALID_HANDLE,
                                          Handle:dataTypes = INVALID_HANDLE)
{
    // Make sure constraint types exist.
    ObjLib_BuildConstraintTypes();
    
    // Validate lists.
    if (requireKeys)
    {
        if (keys == INVALID_HANDLE)
        {
            ThrowError("Missing list of key names.");
        }
        if (dataTypes == INVALID_HANDLE)
        {
            ThrowError("Missing list of key data types.");
        }
        
        // Match length.
        new keyLength = GetArraySize(keys);
        new typeLength = GetArraySize(dataTypes);
        if (keyLength == 0 || keyLength != typeLength)
        {
            ThrowError("List of keys is empty or list doesn't match dataType's length.");
        }
    }
    
    new Object:constraint = ObjLib_CreateObject(ObjLib_ObjectTypeConstraints, false);
    
    ObjLib_SetBool(constraint, "nonzero", nonzero);
    ObjLib_SetBool(constraint, "requireKeys", requireKeys);
    ObjLib_SetHandle(constraint, "keys", keys);
    ObjLib_SetHandle(constraint, "dataTypes", dataTypes);
    
    return constraint;
}

/*____________________________________________________________________________*/

/**
 * Creates a custom constraint object. All parameters are optional and defaults
 * to no limit.
 *
 * @param callback      Callback to custom validation handler.
 *
 * @return              Constraint object. Use this once when adding a key.
 *                      Object must be deleted if not used anywhere.
 */
stock Object:ObjLib_GetCustomConstraints(ObjLib_KeyValidator:callback)
{
    // Make sure constraint types exist.
    ObjLib_BuildConstraintTypes();
    
    // Validate callback.
    if (callback == INVALID_FUNCTION)
    {
        ThrowError("Invalid callback function (%x).", callback);
    }
    
    new Object:constraint = ObjLib_CreateObject(ObjLib_CustomConstraints, false);
    
    ObjLib_SetFunction(constraint, "callback", callback);
    
    return constraint;
}

/*____________________________________________________________________________*/

/**
 * Returns whether the specified object is a constraints object.
 *
 * @param constraints   Object to check.
 *
 * @return              True if a constraints object, false otherwise.
 */
stock bool:ObjLib_IsConstraintObject(Object:constraints)
{
    ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(constraints);
    return ObjLib_IsConstraintType(typeDescriptor);
}

/*____________________________________________________________________________*/

/**
 * Returns whether the specified type is a constraint type.
 *
 * @param typeDescriptor    Object type to check.
 *
 * @return                  True if a constraint type, false otherwise.
 */
stock bool:ObjLib_IsConstraintType(ObjectType:typeDescriptor)
{
    if (typeDescriptor == ObjLib_CellConstraints
        || typeDescriptor == ObjLib_FloatConstraints
        || typeDescriptor == ObjLib_HandleConstraints
        || typeDescriptor == ObjLib_FunctionConstraints
        || typeDescriptor == ObjLib_StringConstraints
        || typeDescriptor == ObjLib_ArrayConstraints
        || typeDescriptor == ObjLib_ObjectConstraints
        || typeDescriptor == ObjLib_ObjectTypeConstraints
        || typeDescriptor == ObjLib_CustomConstraints)
    {
        return true;
    }
    
    return false;
}


/******************************************************************************
 *                             INTERNAL USE ONLY                              *
 ******************************************************************************/

/* Internal use only! */
stock ObjLib_BuildConstraintTypes()
{
    if (ObjLib_ConstraintTypesBuilt)
    {
        return;
    }
    
    // Build types.
    ObjLib_CellConstraints          = ObjLib_GetCellConstraintType();
    ObjLib_FloatConstraints         = ObjLib_GetFloatConstraintType();
    ObjLib_HandleConstraints        = ObjLib_GetHandleConstraintType();
    ObjLib_FunctionConstraints      = ObjLib_GetFunctionConstraintType();
    ObjLib_StringConstraints        = ObjLib_GetStringConstraintType();
    ObjLib_ArrayConstraints         = ObjLib_GetArrayConstraintType();
    ObjLib_ObjectConstraints        = ObjLib_GetObjectConstraintType();
    ObjLib_ObjectTypeConstraints    = ObjLib_GetObjTypeConstraintType();
    ObjLib_CustomConstraints        = ObjLib_GetCustomConstraintType();
    
    ObjLib_ConstraintTypesBuilt = true;
}

/*____________________________________________________________________________*/

/* Internal use only! */
stock ObjectType:ObjLib_GetCellConstraintType()
{
    new ObjectType:constraints = ObjLib_CreateType();
    
    ObjLib_AddKey(constraints, "min", ObjDataType_Cell);
    ObjLib_AddKey(constraints, "max", ObjDataType_Cell);
    ObjLib_AddKey(constraints, "lowerLimit", ObjDataType_Bool);
    ObjLib_AddKey(constraints, "upperLimit", ObjDataType_Bool);
    ObjLib_AddKey(constraints, "nonzero", ObjDataType_Bool);
    
    return constraints;
}

/*____________________________________________________________________________*/

/* Internal use only! */
stock ObjectType:ObjLib_GetFloatConstraintType()
{
    new ObjectType:constraints = ObjLib_CreateType();
    
    ObjLib_AddKey(constraints, "min", ObjDataType_Cell);
    ObjLib_AddKey(constraints, "max", ObjDataType_Cell);
    ObjLib_AddKey(constraints, "lowerLimit", ObjDataType_Bool);
    ObjLib_AddKey(constraints, "upperLimit", ObjDataType_Bool);
    ObjLib_AddKey(constraints, "deltaLimit", ObjDataType_Float);
    ObjLib_AddKey(constraints, "nonzero", ObjDataType_Bool);
    
    return constraints;
}

/*____________________________________________________________________________*/

/* Internal use only! */
stock ObjectType:ObjLib_GetHandleConstraintType()
{
    new ObjectType:constraints = ObjLib_CreateType();
    
    ObjLib_AddKey(constraints, "nonzero", ObjDataType_Bool);
    
    return constraints;
}

/*____________________________________________________________________________*/

/* Internal use only! */
stock ObjectType:ObjLib_GetFunctionConstraintType()
{
    new ObjectType:constraints = ObjLib_CreateType();
    
    ObjLib_AddKey(constraints, "nonzero", ObjDataType_Bool);
    
    return constraints;
}

/*____________________________________________________________________________*/

/* Internal use only! */
stock ObjectType:ObjLib_GetStringConstraintType()
{
    // Reserve enough space for whitelist/blacklist.
    new blockSize = ByteCountToCells(OBJECTLIB_WHITELIST_LEN);
    
    new ObjectType:constraints = ObjLib_CreateType(blockSize);
    
    ObjLib_AddKey(constraints, "minLen", ObjDataType_Cell);
    ObjLib_AddKey(constraints, "maxLen", ObjDataType_Cell);             // Cannot be larger than the space reserved in the object array.
    ObjLib_AddKey(constraints, "lowerLimit", ObjDataType_Bool);
    ObjLib_AddKey(constraints, "upperLimit", ObjDataType_Bool);
    ObjLib_AddKey(constraints, "nonempty", ObjDataType_Bool);
    ObjLib_AddKey(constraints, "pathValidation", ObjDataType_Bool);
    ObjLib_AddKey(constraints, "fileValidation", ObjDataType_Bool);
    ObjLib_AddKey(constraints, "includeValveFS", ObjDataType_Bool);     // Check if file exist in valve's file system.
    ObjLib_AddKey(constraints, "whitelistChars", ObjDataType_String);
    ObjLib_AddKey(constraints, "blacklistChars", ObjDataType_String);
    ObjLib_AddKey(constraints, "whitelist", ObjDataType_Bool);
    ObjLib_AddKey(constraints, "blacklist", ObjDataType_Bool);
    
    return constraints;
}

/*____________________________________________________________________________*/

/* Internal use only! */
stock ObjectType:ObjLib_GetArrayConstraintType()
{
    new ObjectType:constraints = ObjLib_CreateType();
    
    ObjLib_AddKey(constraints, "minLen", ObjDataType_Cell);
    ObjLib_AddKey(constraints, "maxLen", ObjDataType_Cell);             // Cannot be larger than the space reserved in the object array.
    ObjLib_AddKey(constraints, "lowerLimit", ObjDataType_Bool);
    ObjLib_AddKey(constraints, "upperLimit", ObjDataType_Bool);
    
    return constraints;
}

/*____________________________________________________________________________*/

/* Internal use only! */
stock ObjectType:ObjLib_GetObjectConstraintType()
{
    new ObjectType:constraints = ObjLib_CreateType();
    
    ObjLib_AddKey(constraints, "nonzero", ObjDataType_Cell);
    ObjLib_AddKey(constraints, "type", ObjDataType_ObjectType);
    
    return constraints;
}

/*____________________________________________________________________________*/

/* Internal use only! */
stock ObjectType:ObjLib_GetObjTypeConstraintType()
{
    new ObjectType:constraints = ObjLib_CreateType();
    
    ObjLib_AddKey(constraints, "nonzero", ObjDataType_Cell);
    ObjLib_AddKey(constraints, "keys", ObjDataType_Handle);
    ObjLib_AddKey(constraints, "dataTypes", ObjDataType_Handle);
    ObjLib_AddKey(constraints, "requireKeys", ObjDataType_Bool);
    
    return constraints;
}

/*____________________________________________________________________________*/

/* Internal use only! */
stock ObjectType:ObjLib_GetCustomConstraintType()
{
    new ObjectType:constraints = ObjLib_CreateType();
    
    ObjLib_AddKey(constraints, "callback", ObjDataType_Function);
    
    return constraints;
}

/*____________________________________________________________________________*/

/* Internal use only! */
stock ObjLib_ValidateConstraintOrFail(Object:constraints, ObjectDataType:dataType = ObjDataType_Any)
{
    if (constraints == INVALID_OBJECT)
    {
        // INVALID_OBJECT is considered valid (no constraints enabled).
        return;
    }
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(constraints);
    new ObjectType:constraintType = ObjLib_DataTypeToConstraintType(dataType);
    
    // Check constraint type against data type.
    if (typeDescriptor != ObjLib_CustomConstraints      // Custom constraints allowed on all data types.
        && constraintType == INVALID_OBJECT_TYPE)
    {
        new String:dataTypeName[32];
        ObjLib_DataTypeToString(dataType, dataTypeName, sizeof(dataTypeName));
        ThrowError("Invalid constraints object (%x). Not a constraint type or constraints not matching data type (%s).", constraints, dataTypeName);
    }
}

/*____________________________________________________________________________*/

/* Internal use only! */
stock ObjectType:ObjLib_DataTypeToConstraintType(ObjectDataType:dataType)
{
    switch (dataType)
    {
        case ObjDataType_Any:           return ObjLib_CustomConstraints;    // Only custom constraints allowed.
        case ObjDataType_Cell:          return ObjLib_CellConstraints;
        case ObjDataType_Bool:          return ObjLib_CustomConstraints;    // Only custom constraints allowed.
        case ObjDataType_Float:         return ObjLib_FloatConstraints;
        case ObjDataType_Handle:        return ObjLib_HandleConstraints;
        case ObjDataType_Function:      return ObjLib_FunctionConstraints;
        case ObjDataType_Array:         return ObjLib_ArrayConstraints;
        case ObjDataType_String:        return ObjLib_StringConstraints;
        case ObjDataType_Object:        return ObjLib_ObjectConstraints;
        case ObjDataType_ObjectType:    return ObjLib_ObjectTypeConstraints;
        
        default:
        {
            return INVALID_OBJECT_TYPE;
        }
    }
    
    return INVALID_OBJECT_TYPE;
}
