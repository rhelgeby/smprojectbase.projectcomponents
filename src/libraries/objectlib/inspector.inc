/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           inspector.inc
 *  Type:           Library
 *  Description:    Object inspector utility for objectlib.
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
 * Prints object contents to the specified client.
 *
 * Output style:
 *
 * Key:                    Data type:  Value:
 * -----------------------------------------------------------------------------
 * exampleKey1             Array       {0, 1, 2, 3, 4, 5}
 * exampleKey2             String      "test string"
 * exampleKey3             Float       2.5
 * exampleKey4             Object      0x12345678
 *
 * 4 key(s) total.
 *
 * @param client        Client or server index.
 * @param object        Object to inspect.
 */
stock ObjLib_DumpObjectKeys(client, Object:object)
{
    static String:HEADER_FORMAT[] = "%23s %11s %s\n-------------------------------------------------------------------------------";
    static String:ROW_FORMAT[] = "%23s %11s %s";
    
    new String:keyName[OBJECT_KEY_NAME_LEN];
    new String:dataTypeString[32];
    new String:valueString[OBJLIB_MAX_STRING_LEN];
    
    // Get type.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Get keys.
    new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
    new Handle:nullKey = ObjLib_GetObjectNullKey(object);
    
    // Get data types.
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    
    // Print table header.
    ReplyToCommand(client, HEADER_FORMAT, "Key:", "Data type:", "Value:");
    
    // Loop through keys.
    new numKeys = ObjLib_GetNumKeys(typeDescriptor);
    for (new key = 0; key < numKeys; key++)
    {
        // Get key name.
        GetArrayString(keys, key, keyName, sizeof(keyName));
        
        // Get data type.
        ObjLib_DataTypeToString(ObjectDataType:GetArrayCell(dataTypes, key), dataTypeString, sizeof(dataTypeString));
        
        // Convert value to a string.
        new bool:isNull = bool:GetArrayCell(nullKey, key);
        if (isNull)
        {
            strcopy(valueString, sizeof(valueString), "(Null)");
        }
        else
        {
            ObjLib_ValueToString(object, key, valueString, sizeof(valueString));
        }
        
        // Print row.
        ReplyToCommand(client, ROW_FORMAT, keyName, dataTypeString, valueString);
    }
    
    ReplyToCommand(client, "\n%d key(s) total.", numKeys);
}

/*____________________________________________________________________________*/

/**
 * Prints object raw data to the specified client.
 *
 * @param client        Client or server index.
 * @param object        Object to inspect.
 */
stock ObjLib_DumpRawObject(client, Object:object)
{
    static String:HEADER_FORMAT[] = "%23s %15s %s\n-------------------------------------------------------------------------------";
    static String:ROW_FORMAT[] = "%23s %15s %s";
    
    new String:valueString[16];
    
    // Get raw data.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    new Handle:nullKey = ObjLib_GetObjectNullKey(object);
    new Handle:data = ObjLib_GetObjectData(object);
    
    // Print table header.
    ReplyToCommand(client, HEADER_FORMAT, "Field:", "Data type:", "Value:");
    
    // data
    Format(valueString, sizeof(valueString), "0x%x", data);
    ReplyToCommand(client, ROW_FORMAT, "Object_Data", "Array", valueString);
    
    // nullKey
    Format(valueString, sizeof(valueString), "0x%x", nullKey);
    ReplyToCommand(client, ROW_FORMAT, "Object_NullKey", "Array", valueString);
    
    // typeDescriptor
    Format(valueString, sizeof(valueString), "0x%x", typeDescriptor);
    ReplyToCommand(client, ROW_FORMAT, "Object_MetaData", "ObjectType", valueString);
}

/*____________________________________________________________________________*/

/**
 * Prints type descriptor raw data to the specified client.
 *
 * @param client        Client or server index.
 * @param object        Object to inspect.
 */
stock ObjLib_DumpRawType(client, ObjectType:typeDescriptor)
{
    static String:HEADER_FORMAT[] = "%23s %15s %s\n-------------------------------------------------------------------------------";
    static String:ROW_FORMAT[] = "%23s %15s %s";
    
    new String:valueString[16];
    
    // Get raw data.
    new bool:locked = ObjLib_IsTypeMutable(typeDescriptor);
    new Object:parent = ObjLib_GetTypeParentObject(typeDescriptor);
    new keySize = ObjLib_GetTypeKeySize(typeDescriptor);
    new blockSize = ObjLib_GetTypeBlockSize(typeDescriptor);
    new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
    new Handle:nameIndex = ObjLib_GetTypeNameIndex(typeDescriptor);
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    new Handle:constraints = ObjLib_GetTypeConstraints(typeDescriptor);
    new ObjLib_ErrorHandler:errorHandler = ObjLib_GetTypeErrorHandler(typeDescriptor);
    
    // Print table header.
    ReplyToCommand(client, HEADER_FORMAT, "Field:", "Data type:", "Value:");
    
    // locked
    Format(valueString, sizeof(valueString), "%d", locked);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_Locked", "Boolean", valueString);
    
    // parent
    Format(valueString, sizeof(valueString), "0x%x", parent);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_ParentObject", "Object", valueString);
    
    // keySize
    Format(valueString, sizeof(valueString), "%d", keySize);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_KeySize", "Integer", valueString);
    
    // blockSize
    Format(valueString, sizeof(valueString), "%d", blockSize);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_BlockSize", "Integer", valueString);
    
    // keys
    Format(valueString, sizeof(valueString), "0x%x", keys);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_Keys", "Array", valueString);
    
    // nameIndex
    Format(valueString, sizeof(valueString), "0x%x", nameIndex);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_NameIndex", "Trie", valueString);
    
    // dataTypes
    Format(valueString, sizeof(valueString), "0x%x", dataTypes);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_DataTypes", "Array", valueString);
    
    // constraints
    Format(valueString, sizeof(valueString), "0x%x", constraints);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_Constraints", "Array", valueString);
    
    // errorHandler
    Format(valueString, sizeof(valueString), "0x%x", errorHandler);
    ReplyToCommand(client, ROW_FORMAT, "ObjectType_ErrorHandler", "Function", valueString);
}

/*____________________________________________________________________________*/

/**
 * Converts the value at the specified key index to a string.
 *
 * @param object    Object to inspect.
 * @param keyIndex  Index of key to convert.
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of buffer.
 *
 * @return          Number of cells written.
 */
stock ObjLib_ValueToString(Object:object, keyIndex, String:buffer[], maxlen)
{
    decl String:valueString[OBJLIB_MAX_STRING_LEN];
    valueString[0] = 0;
    
    // Get type.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Get data type.
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
    new ObjectDataType:dataType = ObjectDataType:GetArrayCell(dataTypes, keyIndex);
    
    // Get data values.
    new Handle:data = ObjLib_GetObjectData(object);
    
    switch (dataType)
    {
        case ObjDataType_Any, ObjDataType_Cell, ObjDataType_Bool:
        {
            new value = GetArrayCell(data, keyIndex);
            return IntToString(value, buffer, maxlen);
        }
        case ObjDataType_Float:
        {
            new Float:value = Float:GetArrayCell(data, keyIndex);
            return FloatToString(value, buffer, maxlen);
        }
        case ObjDataType_Handle, ObjDataType_Function, ObjDataType_Object, ObjDataType_ObjectType:
        {
            new value = GetArrayCell(data, keyIndex);
            return Format(buffer, maxlen, "0x%x", value);
        }
        case ObjDataType_Array:
        {
            new blockSize = ObjLib_GetTypeBlockSize(typeDescriptor);
            new values[blockSize];
            GetArrayArray(data, keyIndex, values, blockSize);
            return Array_CellsToString(values, blockSize, buffer, maxlen);
        }
        case ObjDataType_String:
        {
            return GetArrayString(data, keyIndex, buffer, maxlen);
        }
        default:
        {
            ThrowError("[BUG] Unexpected type. This is a bug in objectlib.");
        }
    }
    
    return 0;
}
