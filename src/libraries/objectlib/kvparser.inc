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
 * longer string values.
 */
#define OBJLIB_KV_MAX_STRING_LEN    255

/*____________________________________________________________________________*/

/**
 * Length of key name buffers used in parser.
 *
 * Note: Don't reserve too much space. The parser is recursive and very large
 *       key name buffers will consume a lot of stack space if the keyvalue tree
 *       is deep.
 */
#define OBJLIB_KV_MAX_KEY_LEN    64

/*____________________________________________________________________________*/

/**
 * Empty object type template with enough space reserved for strings.
 */
new ObjectType:ObjLib_KvObjectType = INVALID_OBJECT_TYPE;
new bool:ObjLib_KvObjectTypeBuilt = false;

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
    return INVALID_OBJECT;
}

/*____________________________________________________________________________*/

/**
 * Parse a KeyValue tree in list mode. This parser assumes that every key in the
 * root is a sub section. Each section is parsed into an object and stored in a
 * list.
 *
 * @param kv                KeyValue tree to parse.
 * @param typeDescriptor    (Optional) Type of object. If not specified it will
 *                          create keys in mutable objects.
 * @param sectionKeyName    Name of key where section name is stored. Must exist
 *                          in type descriptor if a type is used. Otherwise it's
 *                          created automatically.
 *
 * @return                  Handle to list of objects. May be empty if tree is
 *                          empty.
 */
stock Handle:ObjLib_ParseInListMode(Handle:kv, ObjectType:typeDescriptor = INVALID_OBJECT_TYPE, const String:sectionKeyName[])
{
    decl String:keyName[OBJLIB_KV_MAX_KEY_LEN];
    keyName[0] = 0;
    
    new i = 0;
    
    // Go to root.
    // TODO: Don't rewind to support parsing a certain part of the tree.
    KvRewind(kv);
    
    // Create list to store section objects.
    new Handle:sections = CreateArray();
    
    // Go to first sub section in section.
    if (!KvGotoFirstSubKey(kv, true))
    {
        // No sections. Return list as it is (empty).
        return sections;
    }
    
    // Loop through sections.
    PrintToServer("===== List mode: start =====");
    do
    {
        PrintToServer("----- List mode: iteration start -----");
        // Parse section recurively.
        new Object:subObject;
        if (typeDescriptor)
        {
            subObject = ObjLib_ParseSectionAs(kv, typeDescriptor, sectionKeyName);
        }
        else
        {
            subObject = ObjLib_ParseSection(kv, sectionKeyName);
        }
        
        // Add object to list.
        PushArrayCell(sections, subObject);
        
        // Get section name. This also implies going up one level, out of the
        // sub section.
        //ObjLib_GetKvKeyName(kv, keyName, sizeof(keyName));
        //PrintToServer("List mode: Got section name: %s", keyName);
        
        // Go up one level. When reading last key the cursor was moved into that
        // key.
        // TODO: What if sub section is empty?
        //KvGoBack(kv);
        //PrintToServer("List mode: Going up one level (to root).");
        
        // Jump back to the section.
        //KvJumpToKey(kv, keyName);
        //PrintToServer("List mode: Jumping to %s", keyName);
        
        i++;
        if (i > 5)
        {
            PrintToServer("BUG: Infinite loop. Aborting.");
            break;
        }
    } while (KvGotoNextKey(kv, false));    // Go to next section, skip values.
    PrintToServer("===== List mode end =====");
    
    return sections;
}

/*____________________________________________________________________________*/

/**
 * Parse the current section into an object. It will do full recursive traversal
 * from the current section.
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
    // Note: This function do recursive calls to itself.
    
    decl String:keyName[OBJLIB_KV_MAX_KEY_LEN];
    keyName[0] = 0;
    
    // Get name of current section. Also don't go back to parent section, we
    // want to stay in current section.
    ObjLib_GetKvKeyName(kv, keyName, sizeof(keyName), false);
    PrintToServer("Got section name: %s", keyName);
    
    // Build type template.
    ObjLib_BuildKvObjectType();
    
    // Create mutable object.
    new Object:object = ObjLib_CreateObject(ObjLib_KvObjectType);
    
    // Add key and store section name.
    ObjLib_AddObjectKey(object, sectionKeyName, ObjDataType_String);
    ObjLib_SetString(object, sectionKeyName, keyName);
    
    PrintToServer("-- Parse Section: start --");
    
    // Go to first key in section.
    if (KvGotoFirstSubKey(kv, false))
    {
        PrintToServer("Jumped to first sub key.");
    
        // Loop through keys and sections in the current kv section.
        do
        {
            PrintToServer("Iteration start.");
            
            // Check if key is a sub section.
            if (KvGotoFirstSubKey(kv, true))
            {
                // Key is a sub section.
                PrintToServer("Found sub section section. Recursive entry.");
                
                // Parse sub section recurively.
                new Object:subObject = ObjLib_ParseSection(kv, sectionKeyName);
                
                // Go up one level, out of the sub section so it's ready to read
                // next key.
                KvGoBack(kv);
                PrintToServer("Going up one level.");
                
                // Get sub section name.
                ObjLib_GetString(subObject, sectionKeyName, keyName, sizeof(keyName));
                PrintToServer("Got sub section name: %s", keyName);
                //ObjLib_GetKvKeyName(kv, keyName, sizeof(keyName));
                
                // ObjLib_GetKvKeyName moves into the sub section and back when
                // reading the section name. After a the call the keyvalue
                // cursor is at the first key again, so we must jump back to the
                // sub section position.
                //KvJumpToKey(kv, keyName);
                //PrintToServer("Jumping to %s", keyName);
                
                // Add sub object to object.
                ObjLib_AddObjectKey(object, keyName, ObjDataType_Object);
                ObjLib_SetObject(object, keyName, subObject);
            }
            else
            {
                // Key is a value.
                PrintToServer("Key is a value.");
                
                // Get key name.
                KvGotoFirstSubKey(kv, false);   // Jump into key (as if it's a section).
                ObjLib_GetKvKeyName(kv, keyName, sizeof(keyName));  // Read section name of key.
                PrintToServer("Got key name: %s", keyName);
                
                // Add string key to store value in.
                ObjLib_AddObjectKey(object, keyName, ObjDataType_String);
                
                // Get and store value.
                ObjLib_GetKvValue(kv, object, keyName, ObjDataType_String);
                PrintToServer("Reading value of %s", keyName);
                
                // Jump back to the key, because reading a value moves into that
                // key.
                // TODO: Possible infinite loop if there are multiple keys with
                //       the same name. Use key IDs instead of names.
                KvJumpToKey(kv, keyName);
                PrintToServer("Jumping to %s", keyName);
            }
        } while (KvGotoNextKey(kv, false));     // Go to next key or section.
    }
    PrintToServer("-- Parse section: end --");
    
    // Go out of section.
    // This might not be correct to do in all cases. ParseSection should only be
    // responsible for leaving the keyvalue cursor at the same level as when it
    // began parsing the section.
    KvGoBack(kv);
    
    return object;
}

/*____________________________________________________________________________*/

/**
 * Parse the current section into an object of the specified type. It will do
 * full recursive traversal from the current section.
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
    return INVALID_OBJECT;
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Gets the name of the current key.
 *
 * Note: This will jump back to the super section. You might need to use
 *       KvJumpToKey to jump back to the key name retrieved by this function -
 *       unless it's ok to rewind to the first key in the current section.
 */
stock ObjLib_GetKvKeyName(Handle:kv, String:buffer[], maxlen, bool:goBack = true)
{
    KvGetSectionName(kv, buffer, maxlen);
    
    // Go up one level. Get out of the section again.
    if (goBack)
    {
        KvGoBack(kv);
    }
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
            ObjLib_SetAny(object, keyName, value);
        }
        case ObjDataType_Cell:
        {
            new value = KvGetNum(kv, keyName);
            ObjLib_SetCell(object, keyName, value);
        }
        case ObjDataType_Bool:
        {
            // TODO: Support boolean strings ("true", "false", "on", "off", "yes", "no).
            // Get value as string
            // Check if number, if so convert to bool.
            // If string check for boolean words, convert.
            // Throw error if unknown word.
            
            new bool:value = bool:KvGetNum(kv, keyName);
            ObjLib_SetBool(object, keyName, value);
        }
        case ObjDataType_Float:
        {
            new Float:value = KvGetFloat(kv, keyName);
            ObjLib_SetFloat(object, keyName, value);
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
            ObjLib_SetString(object, keyName, value);
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

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_BuildKvObjectType()
{
    if (!ObjLib_KvObjectTypeBuilt)
    {
        ObjLib_KvObjectType = ObjLib_CreateType(ByteCountToCells(OBJLIB_KV_MAX_STRING_LEN));
        ObjLib_KvObjectTypeBuilt = true;
    }
}
