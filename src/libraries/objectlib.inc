/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           objectlib.inc
 *  Type:           Library
 *  Description:    Key/value dynamic object storage manager with validation,
 *                  type safety and support for importing from Valve's KeyValue
 *                  file format.
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

#if defined _objectlib_included
 #endinput
#endif
#define _objectlib_included

#include <sourcemod>

/**
 * Object tag.
 */
enum Object
{
    INVALID_OBJECT = 0
}

/**
 * Number of bytes reserved for key names.
 */
#define OBJECT_KEY_NAME_LEN     32

/**
 * Object type descriptor tag.
 */
enum ObjectType
{
    INVALID_OBJECT_TYPE = 0
}

/**
 * Data types that objects can store.
 */
enum ObjectDataType
{
    ObjDataType_Any,            /** Does not include arrays. */
    ObjDataType_Cell,
    ObjDataType_Bool,
    ObjDataType_Float,
    ObjDataType_Handle,
    ObjDataType_Function,
    ObjDataType_Array,
    ObjDataType_String,
    ObjDataType_Object,
    ObjDataType_ObjectType,
}

/**
 * Object entries. Used internally to name array indexes.
 *
 * Note: The order of these elements must match code in ObjLib_CreateObject and
 *       ObjLib_CloneObject.
 */
#define OBJECT_DATA_LEN         2   /** Number of elements below. */
enum ObjectData
{
    Object_Data = 0,    /** Data entry. Handle to raw data array. */
    Object_MetaData     /** Object meta data entry. Handle to object type descriptor. */
}

/**
 * Object type entries. Used internally to name array indexes.
 *
 * Note: The order of these elements must match code in ObjLib_CreateType and
 *       ObjLib_CloneType.
 */
#define OBJECT_TYPE_DATA_LEN    8   /** Number of elements below. */
enum ObjectTypeData
{
    ObjectType_Locked = 0,      /** Whether type descriptor is read only. */
    ObjectType_ParentObject,    /** Reference to parent object, if any. Used by mutable objects. */
    ObjectType_KeySize,         /** Block size of key name array. */
    ObjectType_BlockSize,       /** Block size of raw data array. */
    ObjectType_Keys,            /** Handle to array of key names (case sensitive). */
    ObjectType_NameIndex,       /** Handle to trie index of key names. */
    ObjectType_DataTypes,       /** Handle to array of data type for each value entry. */
    ObjectType_Constraints,     /** Handle to validation constraints info for each key. */
}

/**
 * Creates an empty object type descriptor.
 *
 * Note: You may use ByteCountToCells to calculate number of cells required to
 *       fit a certain string length for blockSize and keySize.
 *
 * @param blockSize     (Optional) Maximum number of cells reserved for each
 *                      value entry. Default is 1 cell.
 * @param keySize       (Optional) Maximum number of cells reserved for each
 *                      key name. Default is 8 cells (32 bytes/characters).
 *
 * @return              Reference to type descriptor. Must be deleted with
 *                      ObjLib_DeleteType when no longer in use.
 *
 *                      Note: Don't delete type descriptors if there are objects
 *                      using it. Delete objects first.
 */
stock ObjectType:ObjLib_CreateType(blockSize = 1, keySzie = 8)
{
    // Create array to store type descriptor data. Reserve space required.
    new Handle:typeDescriptor = CreateArray(_, OBJECT_TYPE_DATA_LEN);
    
    new Handle:keys = CreateArray(keySzie);         // List of key names (for enumeration or reflection).
    new Handle:nameIndex = CreateTrie();            // ADT Trie with key names mapped to indexes.
    new Handle:dataTypes = CreateArray();           // List of data types of each value entry.
    
    ObjLib_SetTypeLocked(ObjectType:typeDescriptor,         false);
    ObjLib_SetTypeParentObject(ObjectType:typeDescriptor,   INVALID_OBJECT);
    ObjLib_SetTypeKeySize(ObjectType:typeDescriptor,        keySzie);
    ObjLib_SetTypeBlockSize(ObjectType:typeDescriptor,      blockSize);
    ObjLib_SetTypeKeys(ObjectType:typeDescriptor,           keys);
    ObjLib_SetTypeNameIndex(ObjectType:typeDescriptor,      nameIndex);
    ObjLib_SetTypeDataTypes(ObjectType:typeDescriptor,      dataTypes);
    //ObjLib_SetTypeConstraints(ObjectType:typeDescriptor,  INVALID_HANDLE);     // Not implemented yet.
    
    /*
    PushArrayCell(typeDescriptor, 0);                   // ObjectType_Locked
    PushArrayCell(typeDescriptor, _:INVALID_OBJECT);    // ObjectType_ParentObject
    PushArrayCell(typeDescriptor, keySzie);             // ObjectType_KeySize
    PushArrayCell(typeDescriptor, blockSize);           // ObjectType_BlockSize
    PushArrayCell(typeDescriptor, keys);                // ObjectType_Keys
    PushArrayCell(typeDescriptor, nameIndex);           // ObjectType_NameIndex
    PushArrayCell(typeDescriptor, dataTypes);           // ObjectType_DataTypes
    PushArrayCell(typeDescriptor, INVALID_HANDLE);      // ObjectType_Constraints - not implemented yet.
    */
    
    return ObjectType:typeDescriptor;
}

/**
 * Deletes an object type descriptor.
 *
 * Warning: If there are objects refering to this type descriptor they may
 *          trigger an error when trying to use it. Delete objects first.
 *
 * @param typeDescriptor    Type descriptor to delete.
 * @param resetReference    (Optional) Reset typeDescriptor to
 *                          INVALID_OBJECT_TYPE when deleted. Default is true.
 */
stock ObjLib_DeleteType(&ObjectType:typeDescriptor, bool:resetReference = true)
{
    // Validate.
    ObjLib_ValidateObjectType(typeDescriptor);
    
    // Check if this type descriptor is attatched to a mutable object.
    if (ObjLib_GetTypeParentObject(typeDescriptor) != INVALID_OBJECT)
    {
        ThrowError("Can't directly delete type descriptor in mutable object. Use ObjLib_DeleteObject.");
    }
    
    // Delete data structures stored in the type descriptor.
    CloseHandle(ObjLib_GetTypeKeys(typeDescriptor));
    CloseHandle(ObjLib_GetTypeNameIndex(typeDescriptor));
    CloseHandle(ObjLib_GetTypeDataTypes(typeDescriptor));
    
    // TODO: Delete constraints. These are optional.
    /*new Handle:constraints = ObjLib_GetTypeConstraints(ObjectType:typeDescriptor);
    if (constraints != INVALID_HANDLE)
    {
        // Delete constraints.
    }*/
    
    // Delete type descriptor container.
    CloseHandle(Handle:typeDescriptor);
    
    // Reset reference if enabled.
    if (resetReference)
    {
        typeDescriptor = INVALID_OBJECT_TYPE;
    }
}

/**
 * Creates a new object type based on the specified type descriptor.
 *
 * Note: blockSize and keySize of the new cloned object type cannot be modified.
 * Note: The new object type will not be attatched to an object.
 *
 * @param typeDescriptor    Source object type template.
 * @param locked            (Optional) Whether the new object type is locked.
 *                          Default is false.
 *
 * @return                  New object type descriptor. Must be deleted with
 *                          ObjLib_DeleteType when no longer in use.
 */
stock ObjectType:ObjLib_CloneType(ObjectType:typeDescriptor, bool:locked = false)
{
    new keySize = ObjLib_GetTypeKeySize(typeDescriptor);
    new blockSize = ObjLib_GetTypeBlockSize(typeDescriptor);
    
    new Handle:keys = CloneArray(ObjLib_GetTypeKeys(typeDescriptor));
    new Handle:nameIndex = ObjLib_CloneTrie(ObjLib_GetTypeNameIndex(typeDescriptor), keys, keySize);
    new Handle:dataTypes = CloneArray(ObjLib_GetTypeDataTypes(typeDescriptor));
    // TODO: clone constraints when implemented
    
    new Handle:newType = CreateArray(_, OBJECT_TYPE_DATA_LEN);
    
    ObjLib_SetTypeLocked(ObjectType:newType,        locked);
    ObjLib_SetTypeParentObject(ObjectType:newType,  INVALID_OBJECT);
    ObjLib_SetTypeKeySize(ObjectType:newType,       keySize);
    ObjLib_SetTypeBlockSize(ObjectType:newType,     blockSize);
    ObjLib_SetTypeKeys(ObjectType:newType,          keys);
    ObjLib_SetTypeNameIndex(ObjectType:newType,     nameIndex);
    ObjLib_SetTypeDataTypes(ObjectType:newType,     dataTypes);
    //ObjLib_SetTypeConstraints(ObjectType:newType, INVALID_HANDLE);     // Not implemented yet.
    
    /*
    PushArrayCell(newType, locked);             // ObjectType_Locked
    PushArrayCell(newType, _:INVALID_OBJECT);   // ObjectType_ParentObject
    PushArrayCell(newType, keySzie);            // ObjectType_KeySize
    PushArrayCell(newType, blockSize);          // ObjectType_BlockSize
    PushArrayCell(newType, keys);               // ObjectType_Keys
    PushArrayCell(newType, nameIndex);          // ObjectType_NameIndex
    PushArrayCell(newType, dataTypes);          // ObjectType_DataTypes
    PushArrayCell(newType, INVALID_HANDLE);     // ObjectType_Constraints - not implemented yet.
    */
    
    return ObjectType:newType;
}

/**
 * Returns whether the object type descriptor is valid.
 *
 * @param typeDescriptor    Type descriptor to validate.
 *
 * @return                  True if valid, false otherwise.
 */
stock bool:ObjLib_IsValidObjectType(ObjectType:typeDescriptor)
{
    return typeDescriptor != INVALID_OBJECT_TYPE;
}

/**
 * Adds a new key to an object type descriptor.
 * 
 * @param typeDescriptor    Handle to type descriptor.
 * @param keyName           Name of the new key. Case sensitive.
 * @param dataType          Data type of key.
 */
stock ObjLib_AddKey(ObjectType:typeDescriptor, const String:keyName[], ObjectDataType:dataType = ObjDataType_Any)
{
    // Validate.
    ObjLib_ValidateObjectType(typeDescriptor);
    
    // Check if not mutable.
    if (!ObjLib_IsTypeMutable(typeDescriptor))
    {
        ThrowError("This object type descriptor is immutable (%x).", typeDescriptor);
    }
    
    // Check if key exist.
    if (ObjLib_KeyExist(typeDescriptor, keyName))
    {
        ThrowError("Key already exist (\"%s\").", keyName);
    }
    
    new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
    new Handle:nameIndex = ObjLib_GetTypeNameIndex(typeDescriptor);
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    
    new entryIndex = PushArrayString(keys, keyName);    // Keys
    SetTrieValue(nameIndex, keyName, entryIndex);       // Name index
    PushArrayCell(dataTypes, dataType);                 // Data types
    
    // Update parent object if available.
    new Object:parent = ObjLib_GetTypeParentObject(typeDescriptor);
    if (parent != INVALID_OBJECT)
    {
        // Add initialized data entry. This will push a new empty value at the
        // same index as the key.
        ObjLib_InitializeNewValue(parent, dataType);
    }
}

/**
 * Removes a key from an object type descriptor, if not locked.
 *
 * @param typeDescriptor    Object type descriptor to remove key from.
 * @param keyName           Name of key to remove. Case sensitive.
 *
 * @return                  True if successful, false if type descriptor was
 *                          locked.
 */
stock bool:ObjLib_RemoveKey(ObjectType:typeDescriptor, const String:keyName[])
{
    // Validate.
    ObjLib_ValidateObjectType(typeDescriptor);
    
    // Check if not mutable.
    if (!ObjLib_IsTypeMutable(typeDescriptor))
    {
        ThrowError("This object type descriptor is immutable (%x).", typeDescriptor);
    }
    
    // Check if key doesn't exist.
    if (!ObjLib_KeyExist(typeDescriptor, keyName))
    {
        ThrowError("Key doesn't exist (\"%s\").", keyName);
    }
    
    new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
    new Handle:nameIndex = ObjLib_GetTypeNameIndex(typeDescriptor);
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    
    new entryIndex = -1;
    GetTrieValue(nameIndex, keyName, entryIndex);
    RemoveFromArray(keys, entryIndex);                  // Keys
    RemoveFromArray(dataTypes, entryIndex);             // Data types
    RemoveFromTrie(nameIndex, keyName);                 // Name index
    
    // Update parent object if available.
    new Object:parent = ObjLib_GetTypeParentObject(typeDescriptor);
    if (parent != INVALID_OBJECT)
    {
        // Remove entry.
        ObjLib_RemoveEntry(parent, entryIndex);
    }
}

/**
 * Returns whether a key exist in an object type descriptor.
 *
 * @param typeDescriptor    Obect type descriptor to inspect.
 * @param keyName           Key to search for. Case sensitive.
 *
 * @return                  True if found, false otherwise.
 */
stock bool:ObjLib_KeyExist(ObjectType:typeDescriptor, const String:keyName[])
{
    new Handle:nameIndex = ObjLib_GetTypeNameIndex(typeDescriptor);
    new dummyBuffer;
    if (GetTrieValue(nameIndex, keyName, dummyBuffer))
    {
        // Key exist.
        return true;
    }
    return false;
}

/**
 * Adds a new key to an object, if not locked.
 *
 * @param object    Object to remove key from.
 * @param keyName   Name of key to remove. Case sensitive.
 * @param dataType  Data type of key.
 */
stock bool:ObjLib_AddObjectKey(Object:object, const String:keyName[], ObjectDataType:dataType = ObjDataType_Any)
{
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    ObjLib_AddKey(typeDescriptor, keyName, dataType);
}

/**
 * Removes a key from an object, if not locked.
 *
 * Warning: Data associated with this key will also be deleted.
 *
 * @param object    Object to remove key from.
 * @param keyName   Name of key to remove. Case sensitive.
 */
stock ObjLib_RemoveObjectKey(Object:object, const String:keyName[])
{
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    ObjLib_RemoveKey(typeDescriptor, keyName);
}

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
    new Handle:object = CreateArray(_, OBJECT_DATA_LEN);
    new blockSize = ObjLib_GetTypeBlockSize(typeTemplate);
    
    // Create array for storing object data.
    new Handle:data = CreateArray(blockSize);
    
    // Get type descriptor. Mutable objects use their own clone.
    new ObjectType:type;
    if (mutableObject)
    {
        // Clone type descriptor and make it mutable.
        type = ObjLib_CloneType(typeTemplate, false);
        
        // Attach type to cloned object.
        ObjLib_SetTypeParentObject(type, Object:object);
    }
    else
    {
        // Use the type template directly.
        type = typeTemplate;
        
        // Lock type template for further modification.
        ObjLib_SetTypeLocked(type, true);
    }
    
    ObjLib_SetObjectData(Object:object, data);
    ObjLib_SetTypeDescriptor(Object:object, type);
    
    /*
    PushArrayCell(object, data);        // Object_Data
    PushArrayCell(object, type);        // Object_MetaData
    */
    
    return Object:object;
}

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

/**
 * Creates a clone of an object and its data.
 *
 * Note: If the object data contain reference values, only the references
 *       themself will be cloned, not the data they refer to. See the
 *       cloneHandles parameter for more info.
 *
 * Warning: Creating an immutable object from a mutable object will also create
 *          a clone of the source type descriptor. Delete this type descriptor 
 *          when it's no longer in use. This can be retrieved from the cloned
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
    new Handle:object = CreateArray(_, OBJECT_DATA_LEN);
    
    // Clone data array.
    new Handle:data = CloneArray(ObjLib_GetObjectData(objectTemplate));
    ObjLib_SetObjectData(Object:object, data);
    
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
        ObjLib_SetTypeDescriptor(Object:object, typeTemplate);
    }
    else
    {
        // Either source or destination is mutable. Clone type, as unlocked.
        new ObjectType:type = ObjLib_CloneType(typeTemplate, false);
        
        if (mutableObject)
        {
            // Destination is mutable. Attatch cloned type.
            ObjLib_SetTypeParentObject(type, Object:object);
            ObjLib_SetTypeDescriptor(Object:object, type);
        }
        else
        {
            // Destination is immutable. Link cloned type.
            ObjLib_SetTypeDescriptor(Object:object, type);

            // Lock type.
            ObjLib_SetTypeLocked(type, true);
        }
    }
    
    // Clone handles if enabled.
    if (cloneHandles)
    {
        new len = GetArraySize(data);
        new Handle:dataTypes = ObjLib_GetTypeDataTypes(ObjLib_GetTypeDescriptor(Object:object));
        
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
    
    return Object:object;
}

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

/**
 * Returns whether the specified object is mutable.
 *
 * @return      True if mutable, false otherwise.
 */
stock bool:ObjLib_IsMutable(Object:object)
{
    return ObjLib_IsTypeMutable(ObjLib_GetTypeDescriptor(object));
}

/**************************************
 *   Object data accessor functions   *
 **************************************/

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return any:GetArrayCell(data, index);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    SetArrayCell(data, index, value);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return GetArrayCell(data, index);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    SetArrayCell(data, index, value);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return bool:GetArrayCell(data, index);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    SetArrayCell(data, index, value);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return Float:GetArrayCell(data, index);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    SetArrayCell(data, index, value);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return Handle:GetArrayCell(data, index);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    SetArrayCell(data, index, value);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return Function:GetArrayCell(data, index);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    SetArrayCell(data, index, value);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return GetArrayArray(data, index, buffer, maxlen);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return SetArrayArray(data, index, values, maxlen);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return GetArrayString(data, index, buffer, maxlen);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return SetArrayString(data, index, value);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return Object:GetArrayCell(data, index);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    SetArrayCell(data, index, value);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    return ObjectType:GetArrayCell(data, index);
}

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
    
    // Get value.
    new Handle:data = ObjLib_GetObjectData(object);
    SetArrayCell(data, index, value);
}


/********************************
 *   Object get/set functions   *
 ********************************/

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

/** Internal use only! */
static stock ObjLib_GetKeyIndexOrFail(Object:object, const String:keyName[])
{
    new keyIndex = ObjLib_GetKeyIndex(object, keyName);
    
    if (keyIndex < 0)
    {
        ThrowError("Invalid key name (%s).", keyName);
    }
    
    return keyIndex;
}

/** Internal use only! */
static stock Handle:ObjLib_SetTypeDescriptor(Object:object, ObjectType:typeDescriptor)
{
    SetArrayCell(Handle:object, _:Object_MetaData, typeDescriptor);
}

/** Internal use only! */
static stock Handle:ObjLib_GetObjectData(Object:object)
{
    return Handle:GetArrayCell(Handle:object, _:Object_Data);
}

/** Internal use only! */
static stock Handle:ObjLib_SetObjectData(Object:object, Handle:data)
{
    SetArrayCell(Handle:object, _:Object_Data, data);
}


/*************************************
 *   Object type get/set functions   *
 *************************************/

/**
 * Returns whether the specified object type descriptor is mutable (not locked).
 *
 * @return      True if mutable, false otherwise.
 */
stock bool:ObjLib_IsTypeMutable(ObjectType:typeDescriptor)
{
    return GetArrayCell(Handle:typeDescriptor, _:ObjectType_Locked) == 0;
}

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

/** Internal use only! */
static stock ObjLib_SetTypeLocked(ObjectType:typeDescriptor, bool:locked)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_Locked, locked);
}

/** Internal use only! */
static stock Object:ObjLib_GetTypeParentObject(ObjectType:typeDescriptor)
{
    return Object:GetArrayCell(Handle:typeDescriptor, _:ObjectType_ParentObject);
}

/** Internal use only! */
static stock ObjLib_SetTypeParentObject(ObjectType:typeDescriptor, Object:parent)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_ParentObject, parent);
}

/** Internal use only! */
static stock ObjLib_GetTypeKeySize(ObjectType:typeDescriptor)
{
    return GetArrayCell(Handle:typeDescriptor, _:ObjectType_KeySize);
}

/** Internal use only! */
static stock ObjLib_SetTypeKeySize(ObjectType:typeDescriptor, keySize)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_KeySize, keySize);
}

/** Internal use only! */
static stock ObjLib_GetTypeBlockSize(ObjectType:typeDescriptor)
{
    return GetArrayCell(Handle:typeDescriptor, _:ObjectType_BlockSize);
}

/** Internal use only! */
static stock ObjLib_SetTypeBlockSize(ObjectType:typeDescriptor, blockSize)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_BlockSize, blockSize);
}

/**
 * Gets the list of key names in an object type descriptor.
 *
 * @param typeDescriptor    Object type to inspect.
 *
 * @return      Handle to ADT array with strings of key names. Do not close this
 *              handle. Use ObjLib_DeleteType instead.
 */
stock Handle:ObjLib_GetTypeKeys(ObjectType:typeDescriptor)
{
    return Handle:GetArrayCell(Handle:typeDescriptor, _:ObjectType_Keys);
}

/** Internal use only! */
static stock ObjLib_SetTypeKeys(ObjectType:typeDescriptor, Handle:keys)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_Keys, keys);
}

/** Internal use only! */
static stock Handle:ObjLib_GetTypeNameIndex(ObjectType:typeDescriptor)
{
    return Handle:GetArrayCell(Handle:typeDescriptor, _:ObjectType_NameIndex);
}

/** Internal use only! */
static stock ObjLib_SetTypeNameIndex(ObjectType:typeDescriptor, Handle:nameIndex)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_NameIndex, nameIndex);
}

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

/** Internal use only! */
static stock ObjLib_SetTypeDataTypes(ObjectType:typeDescriptor, Handle:dataTypes)
{
    SetArrayCell(Handle:typeDescriptor, _:ObjectType_DataTypes, dataTypes);
}

/** Internal use only! */
static stock Handle:ObjLib_GetTypeConstraints(ObjectType:typeDescriptor)
{
    ThrowError("Not implemented");
}

/** Internal use only! */
static stock ObjLib_SetTypeConstraints(ObjectType:typeDescriptor, Handle:constraints)
{
    ThrowError("Not implemented");
}


/*********************************
 *   Internal helper functions   *
 *********************************/

/**
 * Creates a new initialized value entry at the end of the object data array.
 *
 * Note: Don't make new value entries without updating the type descriptor.
 *       These must match.
 *
 * @param object    Object where new value entry is added.
 * @param dataType  Data type of initialized value.
 */
static stock ObjLib_InitializeNewValue(Object:object, ObjectDataType:dataType)
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

/**
 * Removes a value entry from an object.
 *
 * Note: Don't make new value entries without updating the type descriptor.
 *
 * @param object        Object to remove value from.
 * @param entryIndex    Data index of value entry.
 */
static stock ObjLib_RemoveEntry(Object:object, entryIndex)
{
    new Handle:data = ObjLib_GetObjectData(object);
    RemoveFromArray(data, entryIndex);
}

/**
 * Creates a clone of a trie with the specified keys.
 *
 * @param trie      Handle to ADT trie.
 * @param keys      Handle to ADT array with strings of key names in the trie.
 * @param keySize   Max key name size (in bytes).
 *
 * @return          Handle to cloned trie. Must be closed with CloseHandle.
 */
static stock Handle:ObjLib_CloneTrie(Handle:trie, Handle:keys, keySize)
{
    new Handle:newTrie = CreateTrie();
    
    // Loop through each key.
    new len = GetArraySize(keys);
    for (new i = 0; i < len; i++)
    {
        // Get key.
        decl String:key[keySize];
        GetArrayString(keys, i, key, keySize);
        
        // Get source value.
        new value;
        GetTrieValue(trie, key, value);
        
        // Set new key.
        SetTrieValue(newTrie, key, value);
    }
    
    return newTrie;
}

/**
 * Validates a key type against an object's key type at the specified index.
 *
 * This function will throw an validation error on key type mismatch.
 *
 * @param typeDescriptor    Object type descriptor.
 * @param keyIndex          Index of key to validate against.
 * @param keyType           Type of key to validate.
 */
static stock ObjLib_KeyTypeCheck(ObjectType:typeDescriptor, keyIndex, ObjectDataType:keyType)
{
    // Validate key index.
    if (keyIndex < 0)
    {
        ThrowError("Invalid key index (%d).", keyIndex);
    }
    
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    new ObjectDataType:destType = ObjectDataType:GetArrayCell(dataTypes, keyIndex);
    
    if (keyType != destType)
    {
        // Key mismatch. Throw error.
        ObjLib_KeyTypeMismatchError(keyType, destType);
    }
}

static stock ObjLib_KeyTypeMismatchError(ObjectDataType:actual, ObjectDataType:expected)
{
    new String:actualName[16];
    new String:expectedName[16];
    
    ObjLib_DataTypeToString(actual, actualName, sizeof(actualName));
    ObjLib_DataTypeToString(expected, expectedName, sizeof(expectedName));
    
    ThrowError("Key type mismatch. Expected \"%s\", but \"%s\" was used.", expectedName, actualName);
}

static stock ObjLib_ValidateObject(Object:object)
{
    if (!ObjLib_IsValidObject(object))
    {
        ThrowError("Invalid object (%x).", object);
    }
}

static stock ObjLib_ValidateObjectType(ObjectType:typeDescriptor)
{
    if (!ObjLib_IsValidObjectType(typeDescriptor))
    {
        ThrowError("Invalid object type (%x).", typeDescriptor);
    }
}
