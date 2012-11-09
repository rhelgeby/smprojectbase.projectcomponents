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
    INVALID_OBJECT = -1
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
    INVALID_OBJECT_TYPE = -1
}

/**
 * Data types that objects can store.
 */
enum ObjectDataType
{
    ObjDataType_Any,
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
    // Create array to store type descriptor data.
    new Handle:typeDescriptor = CreateArray(_, OBJECT_TYPE_DATA_LEN);
    
    new Handle:keys = CreateArray(keySzie);         // List of key names (for enumeration or reflection).
    new Handle:nameIndex = CreateTrie();            // ADT Trie with key names mapped to indexes.
    new Handle:dataTypes = CreateArray();           // List of data types of each value entry.
    
    PushArrayCell(typeDescriptor, 0);                   // ObjectType_Locked
    PushArrayCell(typeDescriptor, _:INVALID_OBJECT);    // ObjectType_ParentObject
    PushArrayCell(typeDescriptor, keySzie);             // ObjectType_KeySize
    PushArrayCell(typeDescriptor, blockSize);           // ObjectType_BlockSize
    PushArrayCell(typeDescriptor, keys);                // ObjectType_Keys
    PushArrayCell(typeDescriptor, nameIndex);           // ObjectType_NameIndex
    PushArrayCell(typeDescriptor, dataTypes);           // ObjectType_DataTypes
    PushArrayCell(typeDescriptor, INVALID_HANDLE);      // ObjectType_Constraints - not implemented yet.
    
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
    if (!ObjLib_IsValidObjectType(typeDescriptor))
    {
        ThrowError("Invalid object type descriptor.");
    }
    
    // Check if this type descriptor is attatched to a mutable object.
    if (ObjectType:GetArrayCell(typeDescriptor, _:ObjectType_ParentObject) != INVALID_OBJECT)
    {
        ThrowError("Can't directly delete type descriptor in mutable object. Use ObjLib_DeleteObject.");
    }
    
    // Delete data structures stored in the type descriptor.
    CloseHandle(Handle:GetArrayCell(typeDescriptor, _:ObjectType_Keys));
    CloseHandle(Handle:GetArrayCell(typeDescriptor, _:ObjectType_NameIndex));
    CloseHandle(Handle:GetArrayCell(typeDescriptor, _:ObjectType_DataTypes));
    
    // Delete constraints. These are optional.
    new Handle:constraints = Handle:GetArrayCell(typeDescriptor, _:ObjectType_Constraints);
    if (constraints != INVALID_HANDLE)
    {
        // TODO: Delete constraints.
    }
    
    // Delete type descriptor container.
    CloseHandle(typeDescriptor);
    
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
    new bool:locked = GetArrayCell(typeDescriptor, ObjectType_Locked);
    new keySize = GetArrayCell(typeDescriptor, ObjectType_KeySize);
    new blockSize = GetArrayCell(typeDescriptor, ObjectType_BlockSize);
    
    new Handle:keys = CloneArray(Handle:GetArrayCell(typeDescriptor, ObjectType_Keys));
    new Handle:nameIndex = ObjLib_CloneTrie(Handle:GetArrayCell(typeDescriptor, ObjectType_NameIndex), keys, keySize);
    new Handle:dataTypes = CloneArray(Handle:GetArrayCell(typeDescriptor, ObjectType_DataTypes));
    // TODO: clone constraints when implemented
    
    new Handle:newType = CreateArray(_, OBJECT_TYPE_DATA_LEN);
    
    PushArrayCell(newType, locked);             // ObjectType_Locked
    PushArrayCell(newType, _:INVALID_OBJECT);   // ObjectType_ParentObject
    PushArrayCell(newType, keySzie);            // ObjectType_KeySize
    PushArrayCell(newType, blockSize);          // ObjectType_BlockSize
    PushArrayCell(newType, keys);               // ObjectType_Keys
    PushArrayCell(newType, nameIndex);          // ObjectType_NameIndex
    PushArrayCell(newType, dataTypes);          // ObjectType_DataTypes
    PushArrayCell(newType, INVALID_HANDLE);     // ObjectType_Constraints - not implemented yet.
    
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
    if (!ObjLib_IsValidObjectType(typeDescriptor))
    {
        ThrowError("Invalid object type descriptor (%x).", typeDescriptor);
    }
    
    // Check if not mutable.
    if (!ObjLib_IsObjectTypeMutable(typeDescriptor))
    {
        ThrowError("This object type descriptor is immutable (%x).", typeDescriptor);
    }
    
    // Check if key exist.
    if (ObjLib_KeyExist(typeDescriptor, keyName))
    {
        ThrowError("Key already exist (\"%s\").", keyName);
    }
    
    new Handle:keys = Handle:GetArrayCell(typeDescriptor, _:ObjectType_Keys);
    new Handle:nameIndex = Handle:GetArrayCell(typeDescriptor, _:ObjectType_NameIndex);
    new Handle:dataTypes = Handle:GetArrayCell(typeDescriptor, _:ObjectType_DataTypes);
    
    new entryIndex = PushArrayString(keys, keyName);    // Keys
    SetTrieValue(nameIndex, keyName, entryIndex);       // Name index
    PushArrayCell(dataTypes, _:dataType);               // Data types
    
    // Update parent object if available.
    new Object:parent = Object:GetArrayCell(typeDescriptor, _:ObjectType_ParentObject);
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
    if (!ObjLib_IsValidObjectType(typeDescriptor))
    {
        ThrowError("Invalid object type descriptor (%x).", typeDescriptor);
    }
    
    // Check if not mutable.
    if (!ObjLib_IsObjectTypeMutable(typeDescriptor))
    {
        ThrowError("This object type descriptor is immutable (%x).", typeDescriptor);
    }
    
    // Check if key doesn't exist.
    if (!ObjLib_KeyExist(typeDescriptor, keyName))
    {
        ThrowError("Key doesn't exist (\"%s\").", keyName);
    }
    
    new Handle:keys = Handle:GetArrayCell(typeDescriptor, _:ObjectType_Keys);
    new Handle:nameIndex = Handle:GetArrayCell(typeDescriptor, _:ObjectType_NameIndex);
    new Handle:dataTypes = Handle:GetArrayCell(typeDescriptor, _:ObjectType_DataTypes);
    
    new entryIndex = GetTrieValue(nameIndex, keyName);
    RemoveFromArray(keys, entryIndex);                  // Keys
    RemoveFromArray(dataTypes, entryIndex);             // Data types
    RemoveFromTrie(nameIndex, keyName);                 // Name index
    
    // Update parent object if available.
    new Object:parent = Object:GetArrayCell(typeDescriptor, _:ObjectType_ParentObject);
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
    new Handle:nameIndex = Handle:GetArrayCell(typeDescriptor, _:ObjectType_NameIndex);
    new dummyBuffer;
    if (GetTrieValue(nameIndex, keyName, dummyBuffer)
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
 *
 * @return          True if successful, false if object was locked.
 */
stock bool:ObjLib_AddObjectKey(Object:object, const String:keyName[], ObjectDataType:dataType = ObjDataType_Any)
{
    new ObjectType:typeDescriptor = ObjLib_GetObjectType(object);
    return ObjLib_AddKey(typeDescriptor, keyName, dataType);
}

/**
 * Removes a key from an object, if not locked.
 *
 * Warning: Data associated with this key will also be deleted.
 *
 * @param object    Object to remove key from.
 * @param keyName   Name of key to remove. Case sensitive.
 *
 * @return          True if successful, false if object was locked.
 */
stock bool:ObjLib_RemoveObjectKey(Object:object, const String:keyName[])
{
    new ObjectType:typeDescriptor = ObjLib_GetObjectType(object);
    return ObjLib_RemoveKey(typeDescriptor, keyName);
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
    new blockSize = GetArrayCell(typeTemplate, ObjectType_BlockSize);
    
    // Create array for storing object data.
    new Handle:data = CreateArray(blockSize);
    
    // Get type descriptor. Mutable objects use their own clone.
    new ObjectType:type;
    if (mutableObject)
    {
        // Clone type descriptor and make it mutable.
        type = ObjLib_CloneType(typeTemplate, false);
    }
    else
    {
        // Use the type template directly.
        type = typeTemplate;
        
        // Lock type template for further modification.
        ObjLib_LockType(type);
    }
    
    PushArrayCell(object, data);        // Object_Data
    PushArrayCell(object, type);        // Object_MetaData
    
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
    if (!ObjLib_IsValidObject(object))
    {
        ThrowError("Invalid object (%x).", object);
    }
    
    new ObjectType:typeDescriptor = ObjLib_GetObjectType(object);
    new Object:parent = Object:GetArrayCell(typeDescriptor, _:ObjectType_ParentObject);
    if (parent != INVALID_OBJECT)
    {
        // Check if the type descriptor actually belong to this object. (Potential bug).
        if (parent != object)
        {
            ThrowError("[BUG] This mutable object (%x) has a type descriptor (%x) that isn't owned by this object. This is a bug!", object, typeDescriptor);
        }
        
        // Unattatch type descriptor from object so it can be deleted.
        SetArrayCell(typeDescriptor, _:ObjectType_ParentObject, INVALID_OBJECT);
        
        // Delete type descriptor.
        ObjLib_DeleteType(typeDescriptor);
    }
    
    // Delete data array.
    CloseHandle(Handle:GetArrayCell(object, _:Object_Data));
    
    // Delete object.
    CloseHandle(object);
    
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
 * Warmomg: Creating an immutable object from a mutable object will also create
 *          a clone of the type descriptor stored in typeDescriptor. Delete this
 *          type descriptor when it's no longer in use.
 *
 * @param objectTemplate    Source object.
 * @param mutableObject     (Optional) Make new object mutable. Default is true.
 * @param cloneHandles      (Optional) Use CloneHandle on value entries with the
 *                          handle data type. The handles will still refer to
 *                          the same data, but through a new cloned handle.
 *                          Default is false.
 * @param typeDescriptor    (Optional output) When mutableObject is false, the
 *                          template object's type descriptor will be cloned.
 *                          Its clone will be stored here. Must be deleted with
 *                          ObjLib_DeleteType when no longer in use.
 *
 * @return                  Reference to new cloned object.
 */
stock Object:ObjLib_CloneObject(Object:objectTemplate, bool:mutableObject = true, bool:cloneHandles = false, &ObjectType:typeDescriptor = INVALID_OBJECT_TYPE)
{
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



/********************************
 *   Object get/set functions   *
 *                              *
 * Note: Some are internal and  *
 * not intended for public use. *
 ********************************/

/**
 * Gets an object's type descriptor.
 *
 * @param object    Object to get type from.
 *
 * @return          Object type descriptor reference.
 */
stock ObjectType:ObjLib_GetObjectType(Object:object)
{
    return ObjectType:GetArrayCell(object, _:Object_MetaData);
}

/**
 * Gets an object's raw data array. Internal use only.
 *
 * @param object    Object to get data from.
 *
 * @return          Object raw data array.
 */
static stock Handle:ObjLib_GetObjectData(Object:object)
{
    return Handle:GetArrayCell(object, _:Object_Data);
}

/**
 * Returns whether the specified object is mutable.
 *
 * @return      True if mutable, false otherwise.
 */
stock bool:ObjLib_IsMutable(Object:object)
{
    return ObjLib_IsTypeMutable(ObjLib_GetObjectType(object));
}


/*************************************
 *   Object type get/set functions   *
 *                                   *
 * Note: Some are internal and not   *
 * intended for public use.          *
 *************************************/

/**
 * Returns whether the specified object type descriptor is mutable (not locked).
 *
 * @return      True if mutable, false otherwise.
 */
stock bool:ObjLib_IsTypeMutable(ObjectType:typeDescriptor)
{
    return GetArrayCell(typeDescriptor, _:ObjectType_Locked) == 0;
}


/*********************************
 *   Internal helper functions   *
 *********************************/

/**
 * Creates a new initialized value entry.
 *
 * Note: Don't make new value entries without updating the type descriptor.
 *
 * @param object    Object where new value entry is added.
 * @param dataType  Data type of initialized value.
 */
static stock ObjLib_InitializeNewValue(Object:object, ObjectDataType:dataType)
{
    new Handle:data = Handle:GetArrayCell(object, _:Object_Data);
    
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
            PushArrayCell(data, dummyArray);
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
            ThrowError("Unexpected data type.");
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
    new Handle:data = Handle:GetArrayCell(object, _:Object_Data);
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
    for (int i = 0; i < len; i++)
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
