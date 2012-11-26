/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           accessors.inc
 *  Type:           Library
 *  Description:    Accessor functions for objectlib.
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

/*****************************
 *   Object data accessors   *
 *****************************/

/**
 * Gets a value of type "any" from the specified key.
 *
 * @param object    Object to get value from.
 * @param keyName   Name of key. Case sensitive.
 *
 * @return          Value in the specified key.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock any:ObjLib_GetAny(Object:object, const String:keyName[])
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    return ObjLib_GetAnyAt(object, keyIndex);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "any" in the specified key.
 *
 * @param object    Object to update.
 * @param keyName   Name of key. Case sensitive.
 * @param value     Value to set.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetAny(Object:object, const String:keyName[], any:value)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    ObjLib_SetAnyAt(object, keyIndex, value);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "cell" from the specified key.
 *
 * @param object    Object to get value from.
 * @param keyName   Name of key. Case sensitive.
 *
 * @return          Value in the specified key.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock ObjLib_GetCell(Object:object, const String:keyName[])
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    return ObjLib_GetCellAt(object, keyIndex);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "cell" in the specified key.
 *
 * @param object    Object to update.
 * @param keyName   Name of key. Case sensitive.
 * @param value     Value to set.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetCell(Object:object, const String:keyName[], any:value)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    ObjLib_SetCellAt(object, keyIndex, value);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "bool" from the specified key.
 *
 * @param object    Object to get value from.
 * @param keyName   Name of key. Case sensitive.
 *
 * @return          Value in the specified key.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock bool:ObjLib_GetBool(Object:object, const String:keyName[])
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    return ObjLib_GetBoolAt(object, keyIndex);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "bool" in the specified key.
 *
 * @param object    Object to update.
 * @param keyName   Name of key. Case sensitive.
 * @param value     Value to set.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetBool(Object:object, const String:keyName[], bool:value)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    ObjLib_SetBoolAt(object, keyIndex, value);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "float" from the specified key.
 *
 * @param object    Object to get value from.
 * @param keyName   Name of key. Case sensitive.
 *
 * @return          Value in the specified key.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock Float:ObjLib_GetFloat(Object:object, const String:keyName[])
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    return ObjLib_GetFloatAt(object, keyIndex);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "float" in the specified key.
 *
 * @param object    Object to update.
 * @param keyName   Name of key. Case sensitive.
 * @param value     Value to set.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetFloat(Object:object, const String:keyName[], Float:value)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    ObjLib_SetFloatAt(object, keyIndex, value);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "handle" from the specified key.
 *
 * @param object    Object to get value from.
 * @param keyName   Name of key. Case sensitive.
 *
 * @return          Value in the specified key.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock Handle:ObjLib_GetHandle(Object:object, const String:keyName[])
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    return ObjLib_GetHandleAt(object, keyIndex);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "handle" in the specified key.
 *
 * @param object    Object to update.
 * @param keyName   Name of key. Case sensitive.
 * @param value     Value to set.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetHandle(Object:object, const String:keyName[], Handle:value)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    ObjLib_SetHandleAt(object, keyIndex, value);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "function" from the specified key.
 *
 * @param object    Object to get value from.
 * @param keyName   Name of key. Case sensitive.
 *
 * @return          Value in the specified key.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock Function:ObjLib_GetFunction(Object:object, const String:keyName[])
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    return ObjLib_GetFunctionAt(object, keyIndex);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "function" in the specified key.
 *
 * @param object    Object to update.
 * @param keyName   Name of key. Case sensitive.
 * @param value     Value to set.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetFunction(Object:object, const String:keyName[], Function:value)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    ObjLib_SetFunctionAt(object, keyIndex, value);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "array" in the specified key.
 *
 * @param object    Object to get value from.
 * @param keyName   Name of key. Case sensitive.
 * @param buffer    Destination buffer.
 * @param maxlen    Size of buffer.
 *
 * @return          Number of cells copied.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock ObjLib_GetArray(Object:object, const String:keyName[], any:buffer[], maxlen)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    return ObjLib_GetArrayAt(object, keyIndex, buffer, maxlen);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "array" in the specified key.
 *
 * @param object    Object to update.
 * @param keyName   Name of key. Case sensitive.
 * @param values    Array to set.
 * @param maxlen    Size of array.
 *
 * @return          Number of cells copied.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetArray(Object:object, const String:keyName[], const any:values[], maxlen)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    return ObjLib_SetArrayAt(object, keyIndex, values, maxlen);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "string" in the specified key.
 *
 * @param object    Object to get value from.
 * @param keyName   Name of key. Case sensitive.
 * @param buffer    Destination buffer.
 * @param maxlen    Size of buffer.
 *
 * @return          Number of characters copied.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock ObjLib_GetString(Object:object, const String:keyName[], String:buffer[], maxlen)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    return ObjLib_GetStringAt(object, keyIndex, buffer, maxlen);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "string" in the specified key.
 *
 * @param object    Object to update.
 * @param keyName   Name of key. Case sensitive.
 * @param value     String to set.
 *
 * @return          Number of characters copied.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetString(Object:object, const String:keyName[], const String:value[])
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    return ObjLib_SetStringAt(object, keyIndex, value);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "object" from the specified key.
 *
 * @param object    Object to get value from.
 * @param keyName   Name of key. Case sensitive.
 *
 * @return          Value in the specified key.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock Object:ObjLib_GetObject(Object:object, const String:keyName[])
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    return ObjLib_GetObjectAt(object, keyIndex);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "object" in the specified key.
 *
 * @param object    Object to update.
 * @param keyName   Name of key. Case sensitive.
 * @param value     Value to set.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetObject(Object:object, const String:keyName[], Object:value)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    ObjLib_SetObjectAt(object, keyIndex, value);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "object type" from the specified key.
 *
 * @param object    Object to get value from.
 * @param keyName   Name of key. Case sensitive.
 *
 * @return          Value in the specified key.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock ObjectType:ObjLib_GetObjectType(Object:object, const String:keyName[])
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    return ObjLib_GetObjectTypeAt(object, keyIndex);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "object type" in the specified key.
 *
 * @param object    Object to update.
 * @param keyName   Name of key. Case sensitive.
 * @param value     Value to set.
 *
 * @error           Invalid object, key name or key type mismatch.
 */
stock ObjLib_SetObjectType(Object:object, const String:keyName[], ObjectType:value)
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    ObjLib_SetObjectTypeAt(object, keyIndex, value);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "any" at the specified key index.
 *
 * @param object    Object to get value from.
 * @param index     Index of key.
 *
 * @return          Value in the specified key.
 *
 * @error           Invalid object, key index or key type mismatch.
 */
stock any:ObjLib_GetAnyAt(Object:object, index)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_Any);
    
    // Null check.
    ObjLib_NullCheck(object, index);
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return any:GetArrayCell(data, index);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "any" at the specified key index.
 *
 * @param object    Object to update.
 * @param index     Index of key.
 * @param value     Value to set.
 */
stock ObjLib_SetAnyAt(Object:object, index, any:value)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_Any);
    
    // Set value.
    new Handle:data = ObjLib_GetObjectData(object);
    SetArrayCell(data, index, value);
    ObjLib_SetKeyNull(object, index, false);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "cell" at the specified key index.
 *
 * @param object    Object to get value from.
 * @param index     Index of key.
 *
 * @return          Value in the specified key.
 *
 * @error           Invalid object, key index or key type mismatch.
 */
stock ObjLib_GetCellAt(Object:object, index)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_Cell);
    
    // Null check.
    ObjLib_NullCheck(object, index);
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return GetArrayCell(data, index);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "cell" at the specified key index.
 *
 * @param object    Object to update.
 * @param index     Index of key.
 * @param value     Value to set.
 */
stock ObjLib_SetCellAt(Object:object, index, value)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_Cell);
    
    // Set value.
    new Handle:data = ObjLib_GetObjectData(object);
    SetArrayCell(data, index, value);
    ObjLib_SetKeyNull(object, index, false);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "bool" at the specified key index.
 *
 * @param object    Object to get value from.
 * @param index     Index of key.
 *
 * @return          Value in the specified key.
 *
 * @error           Invalid object, key index or key type mismatch.
 */
stock bool:ObjLib_GetBoolAt(Object:object, index)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_Bool);
    
    // Null check.
    ObjLib_NullCheck(object, index);
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return bool:GetArrayCell(data, index);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "bool" at the specified key index.
 *
 * @param object    Object to update.
 * @param index     Index of key.
 * @param value     Value to set.
 */
stock ObjLib_SetBoolAt(Object:object, index, bool:value)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_Bool);
    
    // Set value.
    new Handle:data = ObjLib_GetObjectData(object);
    SetArrayCell(data, index, value);
    ObjLib_SetKeyNull(object, index, false);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "float" at the specified key index.
 *
 * @param object    Object to get value from.
 * @param index     Index of key.
 *
 * @return          Value in the specified key.
 *
 * @error           Invalid object, key index or key type mismatch.
 */
stock Float:ObjLib_GetFloatAt(Object:object, index)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_Float);
    
    // Null check.
    ObjLib_NullCheck(object, index);
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return Float:GetArrayCell(data, index);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "float" at the specified key index.
 *
 * @param object    Object to update.
 * @param index     Index of key.
 * @param value     Value to set.
 */
stock ObjLib_SetFloatAt(Object:object, index, Float:value)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_Float);
    
    // Set value.
    new Handle:data = ObjLib_GetObjectData(object);
    SetArrayCell(data, index, value);
    ObjLib_SetKeyNull(object, index, false);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "handle" at the specified key index.
 *
 * @param object    Object to get value from.
 * @param index     Index of key.
 *
 * @return          Value in the specified key.
 *
 * @error           Invalid object, key index or key type mismatch.
 */
stock Handle:ObjLib_GetHandleAt(Object:object, index)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_Handle);
    
    // Null check.
    ObjLib_NullCheck(object, index);
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return Handle:GetArrayCell(data, index);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "handle" at the specified key index.
 *
 * @param object    Object to update.
 * @param index     Index of key.
 * @param value     Value to set.
 */
stock ObjLib_SetHandleAt(Object:object, index, Handle:value)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_Handle);
    
    // Set value.
    new Handle:data = ObjLib_GetObjectData(object);
    SetArrayCell(data, index, value);
    ObjLib_SetKeyNull(object, index, false);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "function" at the specified key index.
 *
 * @param object    Object to get value from.
 * @param index     Index of key.
 *
 * @return          Value in the specified key.
 *
 * @error           Invalid object, key index or key type mismatch.
 */
stock Function:ObjLib_GetFunctionAt(Object:object, index)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_Function);
    
    // Null check.
    ObjLib_NullCheck(object, index);
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return Function:GetArrayCell(data, index);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "function" at the specified key index.
 *
 * @param object    Object to update.
 * @param index     Index of key.
 * @param value     Value to set.
 */
stock ObjLib_SetFunctionAt(Object:object, index, Function:value)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_Function);
    
    // Set value.
    new Handle:data = ObjLib_GetObjectData(object);
    SetArrayCell(data, index, value);
    ObjLib_SetKeyNull(object, index, false);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "array" at the specified key index.
 *
 * @param object    Object to get value from.
 * @param index     Index of key.
 * @param buffer    Destination buffer.
 * @param maxlen    Size of buffer.
 *
 * @return          Number of cells copied.
 *
 * @error           Invalid object, key index or key type mismatch.
 */
stock ObjLib_GetArrayAt(Object:object, index, any:buffer[], maxlen)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_Array);
    
    // Null check.
    ObjLib_NullCheck(object, index);
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return GetArrayArray(data, index, buffer, maxlen);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "array" at the specified key index.
 *
 * @param object    Object to update.
 * @param index     Index of key.
 * @param values    Array to set.
 * @param maxlen    Size of array.
 *
 * @return          Number of cells copied.
 */
stock ObjLib_SetArrayAt(Object:object, index, const any:values[], maxlen)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_Array);
    
    // Set value.
    new Handle:data = ObjLib_GetObjectData(object);
    ObjLib_SetKeyNull(object, index, false);
    return SetArrayArray(data, index, values, maxlen);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "string" at the specified key index.
 *
 * @param object    Object to get value from.
 * @param index     Index of key.
 * @param buffer    Destination buffer.
 * @param maxlen    Size of buffer.
 *
 * @return          Number of characters copied.
 *
 * @error           Invalid object, key index or key type mismatch.
 */
stock ObjLib_GetStringAt(Object:object, index, String:buffer[], maxlen)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_String);
    
    // Null check.
    ObjLib_NullCheck(object, index);
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return GetArrayString(data, index, buffer, maxlen);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "string" at the specified key index.
 *
 * @param object    Object to update.
 * @param index     Index of key.
 * @param value     String to set.
 *
 * @return          Number of characters copied.
 */
stock ObjLib_SetStringAt(Object:object, index, const String:value[])
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_String);
    
    // Set value.
    new Handle:data = ObjLib_GetObjectData(object);
    ObjLib_SetKeyNull(object, index, false);
    return SetArrayString(data, index, value);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "object" at the specified key index.
 *
 * @param object    Object to get value from.
 * @param index     Index of key.
 *
 * @return          Value in the specified key.
 *
 * @error           Invalid object, key index or key type mismatch.
 */
stock Object:ObjLib_GetObjectAt(Object:object, index)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_Object);
    
    // Null check.
    ObjLib_NullCheck(object, index);
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return Object:GetArrayCell(data, index);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "object" at the specified key index.
 *
 * @param object    Object to update.
 * @param index     Index of key.
 * @param value     Value to set.
 */
stock ObjLib_SetObjectAt(Object:object, index, Object:value)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_Object);
    
    // Set value.
    new Handle:data = ObjLib_GetObjectData(object);
    SetArrayCell(data, index, value);
    ObjLib_SetKeyNull(object, index, false);
}

/*____________________________________________________________________________*/

/**
 * Gets a value of type "object type" at the specified key index.
 *
 * @param object    Object to get value from.
 * @param index     Index of key.
 *
 * @return          Value in the specified key.
 *
 * @error           Invalid object, key index or key type mismatch.
 */
stock ObjectType:ObjLib_GetObjectTypeAt(Object:object, index)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_ObjectType);
    
    // Null check.
    ObjLib_NullCheck(object, index);
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return ObjectType:GetArrayCell(data, index);
}

/*____________________________________________________________________________*/

/**
 * Sets a value of type "object type" at the specified key index.
 *
 * @param object    Object to update.
 * @param index     Index of key.
 * @param value     Value to set.
 */
stock ObjLib_SetObjectTypeAt(Object:object, index, ObjectType:value)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Validate type.
    ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, index, ObjDataType_ObjectType);
    
    // Set value.
    new Handle:data = ObjLib_GetObjectData(object);
    SetArrayCell(data, index, value);
    ObjLib_SetKeyNull(object, index, false);
}

/*____________________________________________________________________________*/

/***************************
 *   Meta data accessors   *
 ***************************/

/**
 * Returns whether the specified object is mutable.
 *
 * @return      True if mutable, false otherwise.
 */
stock bool:ObjLib_IsMutable(Object:object)
{
    return ObjLib_IsTypeMutable(ObjLib_GetTypeDescriptor(object));
}

/*____________________________________________________________________________*/

/**
 * Gets an object's type descriptor.
 *
 * @param object    Object to get type from.
 *
 * @return          Object type descriptor reference.
 */
stock ObjectType:ObjLib_GetTypeDescriptor(Object:object)
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    return ObjectType:GetArrayCell(Handle:object, _:Object_MetaData);
}

/*____________________________________________________________________________*/

/**
 * Gets the index for the specified key in an object.
 *
 * @param object    Object to get index from.
 * @param keyName   Name of key. Case sensitive.
 *
 * @return          Key index if successful, or -1 if it doesn't exist.
 */
stock ObjLib_GetKeyIndex(Object:object, const String:keyName[])
{
    // Validate object.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    new Handle:nameIndex = ObjLib_GetTypeNameIndex(typeDescriptor);
    
    new index = -1;
    if (GetTrieValue(nameIndex, keyName, index))
    {
        // Key found.
        return index;
    }
    
    // Key not found.
    return -1;
}

/*____________________________________________________________________________*/

/**
 * Returns whether a key is null at the specified key index.
 *
 * @param object    Object to inspect.
 * @param keyIndex  Key index.
 *
 * @return          True if null, false otherwise.
 */
stock bool:ObjLib_IsKeyNullAt(Object:object, keyIndex)
{
    new Handle:nullKey = ObjLib_GetObjectNullKey(object);
    return bool:GetArrayCell(nullKey, keyIndex);
}

/*____________________________________________________________________________*/

/**
 * Returns whether a key is null at the specified key index.
 *
 * @param object    Object to inspect.
 * @param keyName   Key name.
 *
 * @return          True if null, false otherwise.
 */
stock bool:ObjLib_IsKeyNull(Object:object, const String:keyName[])
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    return ObjLib_IsKeyNullAt(object, keyIndex);
}

/*____________________________________________________________________________*/

/**
 * Gets the list of key names in an object type descriptor.
 *
 * @param typeDescriptor    Object type to inspect.
 *
 * @return      Handle to ADT array with key name strings. Do not close this
 *              handle. Use ObjLib_DeleteType instead.
 */
stock Handle:ObjLib_GetTypeKeys(ObjectType:typeDescriptor)
{
    return Handle:GetArrayCell(Handle:typeDescriptor, _:ObjectType_Keys);
}

/*____________________________________________________________________________*/

/**
 * Gets the list of data types for each key in an object type descriptor.
 *
 * @param typeDescriptor    Type descriptor to inspect.
 *
 * @return      Handle to ADT array of ObjectDataType entries.
 */
stock Handle:ObjLib_GetTypeDataTypes(ObjectType:typeDescriptor)
{
    return Handle:GetArrayCell(Handle:typeDescriptor, _:ObjectType_DataTypes);
}

/**
 * Returns whether the specified object type descriptor is mutable (not locked).
 *
 * @param typeDescriptor    Object type to inspect.
 *
 * @return                  True if mutable, false otherwise.
 */
stock bool:ObjLib_IsTypeMutable(ObjectType:typeDescriptor)
{
    return GetArrayCell(Handle:typeDescriptor, _:ObjectType_Locked) == 0;
}

/*____________________________________________________________________________*/

/**
 * Returns the name of a data type.
 *
 * @param dataType  Data type value.
 * @param buffer    Name buffer.
 * @param maxlen    Size of buffer.
 *
 * @return          Number of cells written.
 */
stock ObjLib_DataTypeToString(ObjectDataType:dataType, String:buffer[], maxlen)
{
    switch (dataType)
    {
        case ObjDataType_Any:           return strcopy(buffer, maxlen, "Any");
        case ObjDataType_Cell:          return strcopy(buffer, maxlen, "Cell");
        case ObjDataType_Bool:          return strcopy(buffer, maxlen, "Bool");
        case ObjDataType_Float:         return strcopy(buffer, maxlen, "Handle");
        case ObjDataType_Handle:        return strcopy(buffer, maxlen, "Float");
        case ObjDataType_Function:      return strcopy(buffer, maxlen, "Function");
        case ObjDataType_Array:         return strcopy(buffer, maxlen, "Array");
        case ObjDataType_String:        return strcopy(buffer, maxlen, "String");
        case ObjDataType_Object:        return strcopy(buffer, maxlen, "Object");
        case ObjDataType_ObjectType:    return strcopy(buffer, maxlen, "ObjectType");
    }
    
    return 0;
}

/*____________________________________________________________________________*/

/**
 * Returns the number of keys declared in the specified type descriptor.
 *
 * @param typeDescriptor    Object type to inspect.
 *
 * @return                  Number of keys.
 */
stock ObjLib_GetNumKeys(ObjectType:typeDescriptor)
{
    new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
    return GetArraySize(keys);
}


/******************************************************************************
 *                             INTERNAL USE ONLY                              *
 ******************************************************************************/

/** Internal use only! */
stock ObjLib_SetTypeLocked(ObjectType:typeDescriptor, bool:locked)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_Locked, locked);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock Object:ObjLib_GetTypeParentObject(ObjectType:typeDescriptor)
{
    return Object:GetArrayCell(Handle:typeDescriptor, _:ObjectType_ParentObject);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetTypeParentObject(ObjectType:typeDescriptor, Object:parent)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_ParentObject, parent);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_GetTypeKeySize(ObjectType:typeDescriptor)
{
    return GetArrayCell(Handle:typeDescriptor, _:ObjectType_KeySize);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetTypeKeySize(ObjectType:typeDescriptor, keySize)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_KeySize, keySize);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_GetTypeBlockSize(ObjectType:typeDescriptor)
{
    return GetArrayCell(Handle:typeDescriptor, _:ObjectType_BlockSize);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetTypeBlockSize(ObjectType:typeDescriptor, blockSize)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_BlockSize, blockSize);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetTypeKeys(ObjectType:typeDescriptor, Handle:keys)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_Keys, keys);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock Handle:ObjLib_GetTypeNameIndex(ObjectType:typeDescriptor)
{
    return Handle:GetArrayCell(Handle:typeDescriptor, _:ObjectType_NameIndex);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetTypeNameIndex(ObjectType:typeDescriptor, Handle:nameIndex)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_NameIndex, nameIndex);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetTypeDataTypes(ObjectType:typeDescriptor, Handle:dataTypes)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_DataTypes, dataTypes);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock Handle:ObjLib_GetTypeConstraints(ObjectType:typeDescriptor)
{
    ThrowError("Not implemented");
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetTypeConstraints(ObjectType:typeDescriptor, Handle:constraints)
{
    ThrowError("Not implemented");
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock Handle:ObjLib_SetTypeDescriptor(Object:object, ObjectType:typeDescriptor)
{
    SetArrayCell(Handle:object, _:Object_MetaData, typeDescriptor);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock Handle:ObjLib_GetObjectData(Object:object)
{
    return Handle:GetArrayCell(Handle:object, _:Object_Data);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock Handle:ObjLib_SetObjectData(Object:object, Handle:data)
{
    SetArrayCell(Handle:object, _:Object_Data, data);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock Handle:ObjLib_GetObjectNullKey(Object:object)
{
    return Handle:GetArrayCell(Handle:object, _:Object_NullKey);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock Handle:ObjLib_SetObjectNullKey(Object:object, Handle:data)
{
    SetArrayCell(Handle:object, _:Object_NullKey, data);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_SetKeyNull(Object:object, keyIndex, bool:null)
{
    new Handle:nullKey = ObjLib_GetObjectNullKey(object);
    SetArrayCell(nullKey, keyIndex, null);
}