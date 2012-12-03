/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           kvparser.inc
 *  Type:           Library
 *  Description:    Valve KeyValue format parser for objectlib.
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
 * Length of string buffers used in parser. Increase this if you need to read
 * longer strings.
 */
#define OBJLIB_KV_MAX_STRING_LEN    255

/*____________________________________________________________________________*/

/**
 * Length of key name buffers used in parser.
 */
#define OBJLIB_KV_MAX_KEY_LEN    64

/*____________________________________________________________________________*/

/**
 * Parse a KeyValue file in object mode. The entire root section is parsed into
 * an object.
 *
 * @param typeDescriptor    (Optional) Type of object. If not specified it will
 *                          create keys in mutable objects.
 * @param sectionKeyName    Name of key where section name is stored. Must exist
 *                          in type descriptor if a type is used. Otherwise it's
 *                          created automatically.
 *
 * @return                  Object with data from the root section.
 */
stock Object:ObjLib_ParseInObjectMode(ObjectType:typeDescriptor = INVALID_OBJECT_TYPE, const String:sectionKeyName[])
{
}

/*____________________________________________________________________________*/

/**
 * Parse a KeyValue file in list mode. This parser assumes that every key in the
 * root is a sub section. Each section is parsed into an object and stored in a
 * list.
 *
 * @param typeDescriptor    (Optional) Type of object. If not specified it will
 *                          create keys in mutable objects.
 * @param sectionKeyName    Name of key where section name is stored. Must exist
 *                          in type descriptor if a type is used. Otherwise it's
 *                          created automatically.
 *
 * @return                  Handle to list of objects.
 */
stock Handle:ObjLib_ParseInListMode(ObjectType:typeDescriptor = INVALID_OBJECT_TYPE, const String:sectionKeyName[])
{
}

/*____________________________________________________________________________*/

/**
 * Parse the current section into an object.
 *
 * @param kv                Handle to keyvalue tree with the cursor on a
 *                          section.
 * @param sectionKeyName    Name of key where section name is stored. This key
 *                          will be created.
 *
 * @return                  Object with data from the section.
 */
stock Object:ObjLib_ParseSection(Handle:kv, const String:sectionKeyName[])
{
    decl String:sectionName[OBJLIB_KV_MAX_KEY_LEN];
    sectionName[0] = 0;
    
    // Get section name.
    KvGetSectionName(kv, buffer, maxlen);
    KvGoBack(kv);
    
    
}

/*____________________________________________________________________________*/

/**
 * Parse the current section into an object of the specified type.
 *
 * @param kv                Handle to keyvalue tree with the cursor on a
 *                          section.
 * @param typeDescriptor    Type of object.
 * @param sectionKeyName    Name of key where section name is stored. This key
 *                          must exist in the type descriptor.
 *
 * @return                  Object with data from the section.
 *
 * @error                   Unexpected key.
 */
stock Object:ObjLib_ParseSectionAs(Handle:kv, ObjectType:typeDescriptor, const String:sectionKeyName[])
{
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Gets the name of the current key.
 */
stock ObjLib_GetKvKeyName(Handle:kv, String:buffer[], maxlen)
{
    KvGetSectionName(kv, buffer, maxlen);
    KvGoBack(kv);
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Reads a value from a key and store it in the object.
 *
 * @param kv        Handle to keyvalue tree.
 * @param object    Object to store value in.
 * @param keyName   Key to read.
 * @param dataType  Key type.
 */
stock ObjLib_GetKvValue(Handle:kv, Object:object, const String:keyName[], ObjectDataType:dataType)
{
    switch (dataType)
    {
        case ObjDataType_Any:
        {
            new value = KvGetNum(kv, keyName);
            OjbLib_SetAny(object, keyName, value);
        }
        case ObjDataType_Cell:
        {
            new value = KvGetNum(kv, keyName);
            OjbLib_SetCell(object, keyName, value);
        }
        case ObjDataType_Bool:
        {
            // TODO: Support boolean strings ("true", "false", "on", "off", "yes", "no).
            // Get value as string
            // Check if number, if so convert to bool.
            // If string check for boolean words, convert.
            // Throw error if unknown word.
            
            new bool:value = bool:KvGetNum(kv, keyName);
            OjbLib_SetBool(object, keyName, value);
        }
        case ObjDataType_Float:
        {
            new Float:value = KvGetFloat(kv, keyName);
            OjbLib_SetFloat(object, keyName, value);
        }
        case ObjDataType_Handle:
        {
            // Not supported.
            ThrowError("Handle type is not supported by the objectlib KeyValue parser.");
        }
        case ObjDataType_Function:
        {
            // Not supported.
            ThrowError("Function type is not supported by the objectlib KeyValue parser.");
        }
        case ObjDataType_Array:
        {
            // Not supported.
            ThrowError("Array type is not supported by the objectlib KeyValue parser.");
        }
        case ObjDataType_String:
        {
            new String:value[OBJLIB_KV_MAX_STRING_LEN];
            KvGetString(kv, keyName, value, sizeof(value));
            OjbLib_SetString(object, keyName, value);
        }
        case ObjDataType_Object:
        {
            // This shouldn't happen. Objects should be read as kv sections.
            ThrowError("[BUG] Can't read key as object. The parser should check if this key is a section, if not it's a bug in objectlib.");
        }
        case ObjDataType_ObjectType:
        {
            // Not supported.
            ThrowError("ObjectType type is not supported by the objectlib KeyValue parser.");
        }
        default:
        {
            ThrowError("[BUG] Unexpected data type. This is a bug in objectlib.");
        }
    }
}
