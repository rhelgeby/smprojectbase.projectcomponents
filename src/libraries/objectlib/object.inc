/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           object.inc
 *  Type:           Library
 *  Description:    Object functions for objectlib.
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
 * Creates an empty object based on the type template.
 *
 * There are two types of objects, mutable and immutable.
 *
 * Mutable objects store their own copy of the type descriptor when created so
 * that keys can be added or removed. They use a bit more memory because of
 * this, but are flexible.
 *
 * Immutable objects can have its contents modified, but not its type. Keys
 * cannot be added or removed. The type template is used directly and shared
 * between immutable objects. Memory usage is more efficient than mutable
 * objects.
 *
 * Warning:
 * Creating an immutable object will automatically lock the type template
 * because it's now used directly. If you don't want to lock it, clone it with
 * ObjLib_CloneType and use the cloned type template instead.
 *
 * @param typeTemplate      Object type to use.
 * @param mutableObject     (Optional) Whether the object is mutable (add/remove
 *                          keys). Default is true.
 *
 * @return                  Reference to new object. Must be deleted with
 *                          ObjLib_DeleteObject when no longer in use.
 */
stock Object:ObjLib_CreateObject(ObjectType:typeTemplate, bool:mutableObject = true)
{
    new Object:object = Object:CreateArray(_, OBJECT_DATA_LEN);
    new blockSize = ObjLib_GetTypeBlockSize(typeTemplate);
    new keyCount = ObjLib_GetNumKeys(typeTemplate);
    
    // Create array for storing object data.
    new Handle:data = CreateArray(blockSize);
    new Handle:nullKey = CreateArray(_, keyCount);
    
    // Initialize null key flags. All keys are null.
    for (new i = 0; i < keyCount; i++)
    {
        PushArrayCell(nullKey, 1);
    }
    
    // Get type descriptor. Mutable objects use their own clone.
    new ObjectType:type;
    if (mutableObject)
    {
        // Clone type descriptor and make it mutable.
        type = ObjLib_CloneType(typeTemplate, false);
        
        // Attach type to cloned object.
        ObjLib_SetTypeParentObject(type, object);
    }
    else
    {
        // Use the type template directly.
        type = typeTemplate;
        
        // Lock type template for further modification.
        ObjLib_SetTypeLocked(type, true);
    }
    
    ObjLib_SetObjectData(object,        data);
    ObjLib_SetObjectNullKey(object,     nullKey);
    ObjLib_SetTypeDescriptor(object,    type);
    
    return object;
}

/*____________________________________________________________________________*/

/**
 * Deletes an object and its data.
 *
 * @param object            Object to delete.
 * @param resetReference    (Optional) Reset object to INVALID_OBJECT when
 *                          deleted. Default is true.
 */
stock ObjLib_DeleteObject(&Object:object, resetReference = true)
{
    // Validate.
    ObjLib_ValidateObject(object);
    
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    new Object:parent = ObjLib_GetTypeParentObject(typeDescriptor);
    if (parent != INVALID_OBJECT)
    {
        // Check if the type descriptor actually belong to this object. (Potential bug).
        if (parent != object)
        {
            ThrowError("[BUG] This mutable object (%x) has a type descriptor (%x) that isn't owned by this object. This is a bug!", object, typeDescriptor);
        }
        
        // Unattatch type descriptor from object so it can be deleted.
        ObjLib_SetTypeParentObject(typeDescriptor, INVALID_OBJECT);
        
        // Delete type descriptor.
        ObjLib_DeleteType(typeDescriptor);
    }
    
    // Delete data array.
    CloseHandle(ObjLib_GetObjectData(object));
    
    // Delete object.
    CloseHandle(Handle:object);
    
    // Reset reference if enabled.
    if (resetReference)
    {
        object = INVALID_OBJECT;
    }
}

/*____________________________________________________________________________*/

/**
 * Creates a clone of an object and its data.
 *
 * Note: If the object data contain reference values, only the references
 *       themself will be cloned, not the data they refer to. See the
 *       cloneHandles parameter for more info.
 *
 * Warning: Creating an immutable object from a mutable object will also create
 *          a clone of the source type descriptor. Delete this type descriptor 
 *          when it's no longer in use. It can be retrieved from the cloned
 *          object with ObjLib_GetTypeDescriptor.
 *
 * @param objectTemplate    Source object.
 * @param mutableObject     (Optional) Make new object mutable. Default is true.
 * @param cloneHandles      (Optional) Use CloneHandle on value entries with the
 *                          handle data type. The handles will still refer to
 *                          the same data, but through a new cloned handle.
 *                          Default is false.
 *
 * @return                  Reference to new cloned object.
 */
stock Object:ObjLib_CloneObject(Object:objectTemplate, bool:mutableObject = true, bool:cloneHandles = false)
{
    // Validate object.
    ObjLib_ValidateObject(objectTemplate);
    
    // Create new object array.
    new Object:object = Object:CreateArray(_, OBJECT_DATA_LEN);
    
    // Clone data array.
    new Handle:data = CloneArray(ObjLib_GetObjectData(objectTemplate));
    ObjLib_SetObjectData(object, data);
    
    // Clone null key array.
    new Handle:nullKey = CloneArray(ObjLib_GetObjectNullKey(Object:object));
    ObjLib_SetObjectNullKey(object, nullKey);
    
    // Clone type.
    new ObjectType:typeTemplate = ObjLib_GetTypeDescriptor(objectTemplate);
    new bool:mutableSource = ObjLib_IsMutable(objectTemplate);
    
    /*
    The if block below decides how the type should be cloned and attached.
    
    m = mutable
    i = immutable
    c = clone type
    cl = clone and lock
    s = use same type
    X = invalid
    
                source-i    dest-i  source-m    dest-m
    source-i    X           s       X           c
    dest-i      s           X       cl          X
    source-m    X           cl      X           c
    dest-m      c           X       c           X
    */
    if (!mutableSource && !mutableObject)
    {
        // Template and destination object is immutable. Use the same type
        // descriptor.
        ObjLib_SetTypeDescriptor(object, typeTemplate);
    }
    else
    {
        // Either source or destination is mutable. Clone type, as unlocked.
        new ObjectType:type = ObjLib_CloneType(typeTemplate, false);
        
        if (mutableObject)
        {
            // Destination is mutable. Attatch cloned type.
            ObjLib_SetTypeParentObject(type, object);
            ObjLib_SetTypeDescriptor(object, type);
        }
        else
        {
            // Destination is immutable. Link cloned type.
            ObjLib_SetTypeDescriptor(object, type);

            // Lock type.
            ObjLib_SetTypeLocked(type, true);
        }
    }
    
    // Clone handles if enabled.
    if (cloneHandles)
    {
        new len = GetArraySize(data);
        new Handle:dataTypes = ObjLib_GetTypeDataTypes(ObjLib_GetTypeDescriptor(object));
        
        // Loop through data values.
        for (new i = 0; i < len; i++)
        {
            new ObjectDataType:dataType = ObjectDataType:GetArrayCell(dataTypes, i);
            if (dataType == ObjDataType_Handle)
            {
                // Get original handle.
                new Handle:value = Handle:GetArrayCell(data, i);
                
                // Replace with cloned handle.
                value = CloneHandle(value);
                SetArrayCell(data, i, value);
            }
        }
    }
    
    return object;
}

/*____________________________________________________________________________*/

/**
 * Returns whether the object reference is valid.
 *
 * @param object    Object reference to validate.
 *
 * @return          True if valid, false otherwise.
 */
stock bool:ObjLib_IsValidObject(Object:object)
{
    return object != INVALID_OBJECT;
}

/*____________________________________________________________________________*/

/**
 * Returns whether the object is the specitifed type.
 *
 * Note: This is simple type checking. It compares the objects type descriptor
 *       reference against the specified reference.
 *
 * @param object            Object reference to validate.
 * @param typeDescriptor    Type to match against.
 *
 * @return                  True if types match, false otherwise.
 */
stock bool:ObjLib_TypeOf(Object:object, ObjectType:typeDescriptor)
{
    return typeDescriptor == ObjLib_GetTypeDescriptor(object);
}

/*____________________________________________________________________________*/

/**
 * Adds a new key to an object, if not locked.
 *
 * @param object        Object to remove key from.
 * @param keyName       Name of key to remove. Case sensitive.
 * @param dataType      Data type of key.
 * @param constraints   Key constraints. Constraint type must match data type.
 *                      Use INVALID_OBJECT to disable constraints.
 *
 * @error               Immutable object.
 */
stock bool:ObjLib_AddObjectKey(Object:object, const String:keyName[], ObjectDataType:dataType = ObjDataType_Any, Object:constraints = INVALID_OBJECT)
{
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    ObjLib_AddKey(typeDescriptor, keyName, dataType, constraints);
}

/*____________________________________________________________________________*/

/**
 * Removes a key from an object, if not locked.
 *
 * Warning: Data associated with this key will also be deleted.
 *
 * @param object    Object to remove key from.
 * @param keyName   Name of key to remove. Case sensitive.
 *
 * @error           Immutable object.
 */
stock ObjLib_RemoveObjectKey(Object:object, const String:keyName[])
{
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    ObjLib_RemoveKey(typeDescriptor, keyName);
}

/*____________________________________________________________________________*/

/**
 * Resets the value in the specified key (marked as null).
 *
 * @param object    Object to update.
 * @param keyName   Key name.
 */
stock ObjLib_ClearKey(Object:object, const String:keyName[])
{
    new keyIndex = ObjLib_GetKeyIndexOrFail(object, keyName);
    ObjLib_ClearKeyAt(object, keyIndex);
}

/*____________________________________________________________________________*/

/**
 * Resets the value in the specified key (marked as null).
 *
 * @param object    Object to update.
 * @param keyIndex  Key index.
 */
stock ObjLib_ClearKeyAt(Object:object, keyIndex)
{
    ObjLib_SetKeyNull(object, keyIndex, true);
}


/******************************************************************************
 *                             INTERNAL USE ONLY                              *
 ******************************************************************************/

/** Internal use only! */
stock ObjLib_ValidateObject(Object:object)
{
    if (!ObjLib_IsValidObject(object))
    {
        ThrowError("Invalid object (%x).", object);
    }
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_NullCheck(Object:object, keyIndex)
{
    if (ObjLib_IsKeyNullAt(object, keyIndex))
    {
        // Key is null. Throw error.
        
        // Get Key name.
        new String:keyName[OBJECT_KEY_NAME_LEN];
        new Handle:keys = ObjLib_GetTypeKeys(ObjLib_GetTypeDescriptor(object));
        GetArrayString(keys, keyIndex, keyName, sizeof(keyName));
        
        ThrowError("Key at index %d is null: %s", keyIndex, keyName);
    }
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_GetKeyIndexOrFail(Object:object, const String:keyName[])
{
    new keyIndex = ObjLib_GetKeyIndex(object, keyName);
    
    if (keyIndex < 0)
    {
        ThrowError("Invalid key name (%s).", keyName);
    }
    
    return keyIndex;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Creates a new initialized value entry at the end of the object data array.
 *
 * Note: Don't make new value entries without updating the type descriptor.
 *       These must match.
 *
 * @param object    Object where new value entry is added.
 * @param dataType  Data type of initialized value.
 */
stock ObjLib_InitializeNewValue(Object:object, ObjectDataType:dataType)
{
    new Handle:data = ObjLib_GetObjectData(object);
    
    switch (dataType)
    {
        case ObjDataType_Any:
        {
            PushArrayCell(data, 0);
        }
        case ObjDataType_Cell:
        {
            PushArrayCell(data, 0);
        }
        case ObjDataType_Bool:
        {
            PushArrayCell(data, 0);
        }
        case ObjDataType_Float:
        {
            PushArrayCell(data, 0.0);
        }
        case ObjDataType_Handle:
        {
            PushArrayCell(data, INVALID_HANDLE);
        }
        case ObjDataType_Function:
        {
            PushArrayCell(data, INVALID_FUNCTION);
        }
        case ObjDataType_Array:
        {
            new dummyArray[1];
            PushArrayArray(data, dummyArray, sizeof(dummyArray));
        }
        case ObjDataType_String:
        {
            PushArrayString(data, "");
        }
        case ObjDataType_Object:
        {
            PushArrayCell(data, INVALID_OBJECT);
        }
        case ObjDataType_ObjectType:
        {
            PushArrayCell(data, INVALID_OBJECT_TYPE);
        }
        default:
        {
            ThrowError("Unexpected data type. This is a bug in objectlib.");
        }
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Removes a value entry from an object.
 *
 * Note: Don't make new value entries without updating the type descriptor.
 *
 * @param object        Object to remove value from.
 * @param entryIndex    Data index of value entry.
 */
stock ObjLib_RemoveEntry(Object:object, entryIndex)
{
    new Handle:data = ObjLib_GetObjectData(object);
    RemoveFromArray(data, entryIndex);
}
