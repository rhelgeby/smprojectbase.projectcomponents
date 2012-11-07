/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           objectlib.inc
 *  Type:           Library
 *  Description:    Key/value dynamic object storage manager with validation,
 *                  type safety and support for importing from Valve KeyValue
 *                  files.
 *
 *  Copyright (C) 2009-2012  Richard Helgeby, Greyscale
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

/**
 * Object tag.
 */
enum Object
{
    INVALID_OBJECT = -1
}

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
 */
enum ObjectData
{
    Object_Data = 0,    /** Data entry. Handle to raw data array. */
    Object_MetaData     /** Object meta data entry. Handle to object type descriptor. */
}

/**
 * Object type entries. Used internally to name array indexes.
 */
enum ObjectTypeData
{
    ObjectType_Locked = 0,  /** Whether type descriptor is read only. */
    ObjectType_KeySize,     /** Block size of key name array. */
    ObjectType_BlockSize,   /** Block size of raw data array. */
    ObjectType_Keys,        /** Handle to array of key names. */
    ObjectType_NameIndex,   /** Handle to trie index of key names. */
    ObjectType_DataTypes,   /** Handle to array of data type for each value entry. */
    ObjectType_Constraints, /** Handle to validation constraints info for each key. */
}

/**
 * Creates an empty object type descriptor.
 *
 * @param blockSize     (Optional) Maximum number of cells reserved for each
 *                      value entry. Default is 1 cell.
 *                      
 *                      You may Use ByteCountToCells to calculate number of
 *                      cells required to fit a certain string length.
 *
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
}

/**
 * Creates a new object type based on the specified type descriptor.
 *
 * @param typeDescriptor    Source object type template.
 *
 * @return                  New object type descriptor. Must be deleted with
 *                          ObjLib_DeleteType when no longer in use.
 */
stock ObjectType:ObjLib_CloneType(ObjectType:typeDescriptor)
{
}

/**
 * Adds a key to an object type descriptor.
 * 
 * @param typeDescriptor    Handle to type descriptor.
 * @param keyName           Name of the new key.
 * @param dataType          Data type of key.
 */
stock ObjLib_AddKey(ObjectType:typeDescriptor, const String:keyName[], ObjectDataType:dataType = ObjDataType_Any)
{
}

/**
 * Removes a key from an object type descriptor, if not locked.
 *
 * @param typeDescriptor    Object type descriptor to remove key from.
 * @param keyName           Name of key to remove.
 *
 * @return                  True if successful, false if type descriptor was
 *                          locked.
 */
stock bool:ObjLib_RemoveKey(ObjectType:typeDescriptor, const String:keyName[])
{
}

/**
 * Removes a key from an object, if not locked.
 *
 * Warning: Data associated with this key will also be deleted.
 *
 * @param object    Object to remove key from.
 * @param keyName   Name of key to remove.
 *
 * @return          True if successful, false if object was locked.
 */
stock bool:ObjLib_RemoveObjectKey(Object:object, const String:keyName[])
{
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
 * @param typeTemplate      Object type to use.
 * @param mutableObject     (Optional) Whether the object is mutable (add/remove
 *                          keys). Default is true.
 *
 * @return                  Reference to new object. Must be deleted with
 *                          ObjLib_DeleteObject when no longer in use.
 */
stock Object:ObjLib_CreateObject(ObjectType:typeTemplate, bool:mutableObject = true)
{
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
}

/**
 * Creates a clone of an object and its data.
 *
 * @param objectTemplate    Source object.
 * @param mutableObject     (Optional) Make new object mutable. Default is true.
 *
 * @return                  Reference to new cloned object.
 */
stock Object:ObjLib_CloneObject(Object:objectTemplate, bool:mutableObject = true)
{
}

/**
 * Gets an object's type descriptor.
 *
 * @param object            Object reference to get type from.
 *
 * @return                  Object type descriptor reference.
 */
stock ObjetType:ObjLib_GetObjectType(Object:object)
{
}

/**
 * Returns whether the specified object type descriptor is mutable.
 *
 * @return      True if mutable, false otherwise.
 */
stock bool:ObjLib_IsTypeMutable(ObjectType:typeDescriptor)
{
}

/**
 * Returns whether the specified object descriptor is mutable.
 *
 * @return      True if mutable, false otherwise.
 */
stock bool:ObjLib_IsMutable(Object:object)
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
}
