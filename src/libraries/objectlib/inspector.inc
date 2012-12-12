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
    #define HEADER_FORMAT   "%23s %11s %s\n-------------------------------------------------------------------------------"
    #define ROW_FORMAT   "%23s %11s %s"
    
    decl String:keyName[OBJECT_KEY_NAME_LEN];
    decl String:dataTypeString[32];
    decl String:valueString[OBJLIB_MAX_STRING_LEN];
    keyName[0] = 0;
    dataTypeString[0] = 0;
    valueString[0] = 0;
    
    // Get type.
    new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(object);
    
    // Get keys.
    new Handle:keys = ObjLib_GetTypeKeys(typeDescriptor);
    
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
        ObjLib_ValueToString(Object, key, valueString, sizeof(valueString));
        
        // Print row.
        ReplyToCommand(client, ROW_FORMAT, keyName, dataTypeString, valueString);
    }
    
    ReplyToCommand(client, "\n%n key(s) total.", numKeys);
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
            return Format(buffer, maxlen, "%x", value);
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
