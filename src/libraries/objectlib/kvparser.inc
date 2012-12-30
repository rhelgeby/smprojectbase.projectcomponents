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
    
    //BrowseKeyValues(kv);
    
    // Create list to store section objects.
    new Handle:sections = CreateArray();
    
    // Go to first sub section in section.
    /*if (!KvGotoFirstSubKey(kv))
    {
        // No sections. Return list as it is (empty).
        return sections;
    }*/
    
    new Object:subObject = ObjLib_ParseSection(kv, sectionKeyName);
    PushArrayCell(sections, subObject);
    
    // Loop through sections.
    /*
    PrintToServer("===== List mode: start =====");
    do
    {
        PrintToServer("----- List mode: iteration start -----");
        if (KvGotoFirstSubKey(kv, false))
        {
            PrintToServer("----- List mode: sub section start -----");
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
            
            KvGoBack(kv);
        }
        
        i++;
        if (i > 10)
        {
            PrintToServer("BUG: Infinite loop. Aborting.");
            break;
        }
    } while (KvGotoNextKey(kv, false));    // Go to next section, skip values.
    PrintToServer("===== List mode end =====");
    */
    return sections;
}

/*____________________________________________________________________________*/

/**
 * Parse the current section into an object. It will do full recursive traversal
 * from the current section.
 *
 * @param kv                Handle to keyvalue tree with the cursor on a
 *                          section.
 * @param addEmptySections  Whether to add sections with no keys. An empty
 *                          object will represent an empty section.
 *
 * @return                  Object with data from the section.
 */
stock Object:ObjLib_ParseSection(Handle:kv, bool:addEmptySections = true)
{
    // Note: This function do recursive calls to itself.
    
    decl String:keyName[OBJLIB_KV_MAX_KEY_LEN];
    keyName[0] = 0;
    
    // Build type template.
    ObjLib_BuildKvObjectType();
    
    // Create mutable object.
    new Object:object = ObjLib_CreateObject(ObjLib_KvObjectType, true);
    
    PrintToServer("-- Parse Section: start --");
    
    // Loop through keys and sections in the current kv section.
    do
    {
        PrintToServer("Iteration start.");
        
        // Get current key or section name.
        KvGetSectionName(kv, keyName, sizeof(keyName));
        PrintToServer("Current key: %s", keyName);
        
        // Check if key is a sub section.
        if (KvGotoFirstSubKey(kv, false))
        {
            // Key is a sub section.
            PrintToServer("Found sub section. Recursive entry.");
            
            // Parse sub section recurively.
            new Object:subObject = ObjLib_ParseSection(kv, sectionKeyName);
            
            // Done with sub section. Go out?
            KvGoBack(kv);
            PrintToServer("Going back.");
            
            // Add sub object to object.
            ObjLib_AddObjectKey(object, keyName, ObjDataType_Object);
            ObjLib_SetObject(object, keyName, subObject);
        }
        else
        {
            // Key is a value, or section is empty.
            
            // Sections don't have a data type. If there is a data type, it's a
            // key.
            if (KvGetDataType(kv, NULL_STRING) != KvData_None)
            {
                // Get key name.
                KvGetSectionName(kv, keyName, sizeof(keyName));
                PrintToServer("Found key: %s", keyName);
                
                // Add string key to store value in.
                ObjLib_AddObjectKey(object, keyName, ObjDataType_String);
                
                // Get and store value.
                ObjLib_GetKvValue(kv, object, keyName, ObjDataType_String);
            }
            else
            {
                // Found an empty section.
                PrintToServer("Found empty section.", object);
                
                // Add an empty sub object to object, if enabled.
                if (addEmptySections)
                {
                    new Object:subObject = ObjLib_CreateObject(ObjLib_KvObjectType);
                    ObjLib_AddObjectKey(object, keyName, ObjDataType_Object);
                    ObjLib_SetObject(object, keyName, subObject);
                }
            }
        }
    } while (KvGotoNextKey(kv, false));     // Go to next key or section.
    
    PrintToServer("-- Parse section: end --");
    
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
 * Adds a sub object to the specified object. This function will handle type
 * checking if a type descriptor is specified.
 *
 * @param object            Object to add section in.
 * @param subObject         Sub object representing the section to add.
 * @param keyName           Name of key to store sub object in.
 */
stock ObjLib_AddSection(Object:object, Object:subObject, const String:keyName[], ObjectType:typeDescriptor = INVALID_OBJECT_TYPE)
{
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
            decl String:value[OBJLIB_KV_MAX_STRING_LEN];
            value[0] = 0;
            KvGetString(kv, NULL_STRING /*keyName*/, value, sizeof(value));
            ObjLib_SetString(object, keyName, value);
            PrintToServer("Reading value of %s: \"%s\"", keyName, value);
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


BrowseKeyValues(Handle:kv, level = 0)
{
    new String:sectionName[32];
    new String:value[32];
    
    PrintToServer("-- Section start (level %d) --", level);
    do
    {
        KvGetSectionName(kv, sectionName, sizeof(sectionName));
        PrintToServer("Current key: %s", sectionName);
        
        // Check if current key is a section. Assume it has sub keys and attempt
        // to enter the section.
        if (KvGotoFirstSubKey(kv, false))
        {
            // Success. Confirmed that it's a sub key.
            
            PrintToServer("Sub key. Recursive entry.");
            BrowseKeyValues(kv, level + 1);
            
            PrintToServer("Going back.");
            KvGoBack(kv);
        }
        else
        {
            // Failed. It's a regular key, or the section is empty. Find out by
            // getting the data type of the key.
            
            // Get data type of current key.
            new KvDataTypes:dataType = KvGetDataType(kv, NULL_STRING);
            
            // Check if key has a data type. If not, the section is empty.
            if (dataType != KvData_None)
            {
                // Cursor is on a key.
                KvGetString(kv, NULL_STRING, value, sizeof(value));
                PrintToServer("Regular key. Value: \"%s\"", value);
            }
            else
            {
                // Section is empty. Do nothing.
                PrintToServer("Section is empty.");
            }
        }
    } while (KvGotoNextKey(kv, false));
    PrintToServer("-- Section end --");
}
