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

/*____________________________________________________________________________*/

/**
 * Object tag.
 */
enum Object
{
    INVALID_OBJECT = 0
}

/*____________________________________________________________________________*/

/**
 * Number of bytes reserved for key names.
 */
#define OBJECT_KEY_NAME_LEN     32

/*____________________________________________________________________________*/

/**
 * Object type descriptor tag.
 */
enum ObjectType
{
    INVALID_OBJECT_TYPE = 0
}

/*____________________________________________________________________________*/

/**
 * Data types that objects can store.
 */
enum ObjectDataType
{
    ObjDataType_Any,            /** Does not include arrays/strings. */
    ObjDataType_Cell,
    ObjDataType_Bool,
    ObjDataType_Float,
    ObjDataType_Handle,
    ObjDataType_Function,
    ObjDataType_Array,
    ObjDataType_String,
    ObjDataType_Object,
    ObjDataType_ObjectType
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Object entries. Used internally to name array indexes.
 */
#define OBJECT_DATA_LEN         3   /** Number of elements below. */
enum ObjectData
{
    Object_Data = 0,    /** Data entry. Handle to raw data array. */
    Object_NullKey,     /** Handle to array that tell which key that is null (not initialized). */
    Object_MetaData     /** Object meta data entry. Handle to object type descriptor. */
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Object type entries. Used internally to name array indexes.
 */
#define OBJECT_TYPE_DATA_LEN    9   /** Number of elements below. */
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
    ObjectType_ErrorHandler     /** General error handler. Optional. */
}

/*____________________________________________________________________________*/

/**
 * Error types. Used in error handler callbacks.
 */
enum ObjLibError
{
    ObjLibError_InvalidKey,         /** Key name or index is invalid. */
    ObjLibError_InvalidKeyType,     /** Key type mismatch. */
    ObjLibError_NullDataKey,        /** Attempted to read from uninitialized key. */
    ObjLibError_KeyExist,           /** Attempted to create a key with an existing name. */
    ObjLibError_Immutable,          /** Object or Object type is immutable. */
    ObjLibError_ValidationError     /** Data constraint violation. */
}

/*____________________________________________________________________________*/

/**
 * Number of bytes reserved for various strings.
 */
#define OBJECTLIB_WHITELIST_LEN 64      /** Space reserved in whitelist/blacklist for string constraints. */

/*____________________________________________________________________________*/

/**
 * General error callback.
 *
 * @param typeDescriptor    Related type descriptor.
 * @param errorType         Type of error.
 * @param message           Error message.
 */
functag public ObjLib_ErrorHandler(ObjectType:typeDescriptor, ObjLibError:errorType, const String:message[]);

/*____________________________________________________________________________*/

/**
 * Custom key validation callback.
 *
 * @param object            Object being validated.
 * @param keyName           Key being validated.
 * @param dataType          Type of key.
 * @param typeDescriptor    Object type (meta data).
 */
functag public bool:ObjLib_KeyValidator(Object:object, const String:keyName[], ObjectDataType:dataType, ObjectType:typeDescriptor);

/*____________________________________________________________________________*/


/************************
 *   Helper functions   *
 ************************/

/**
 * Creates a clone of a trie with the specified keys.
 *
 * @param trie      Handle to ADT trie.
 * @param keys      Handle to ADT array with strings of key names in the trie.
 * @param keySize   Max key name size (in bytes).
 *
 * @return          Handle to cloned trie. Must be closed with CloseHandle.
 */
stock Handle:ObjLib_CloneTrie(Handle:trie, Handle:keys, keySize)
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

/*____________________________________________________________________________*/

#include "libraries/objectlib/objecttype"
#include "libraries/objectlib/object"
#include "libraries/objectlib/accessors"
#include "libraries/objectlib/constraints"
#include "libraries/objectlib/errorhandler"
