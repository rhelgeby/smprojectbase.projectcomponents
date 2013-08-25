#pragma semicolon 1
#include <sourcemod>
#include <pawnunit>
#include "libraries/objectlib"

new Handle:KvFull;
new Handle:List;

public OnPluginStart()
{
    RegConsoleCmd("object_list", Command_List, "List objects in root.");
    RegConsoleCmd("object_inspect", Command_Inspect, "Inspect an object. Usage: object_inspect <object address>");
    RegConsoleCmd("object_inspect_ex", Command_InspectEx, "Inspect an object's raw data. Usage: object_inspect_ex <object address>");
    RegConsoleCmd("object_inspect_type_ex", Command_InspectTypeEx, "Inspect an object type's raw data. Usage: object_inspect_type_ex <object type address>");
    
    //ParseUntypedKv();
    ParseTypedKv();
    
    Command_List(0, 0);
}

stock ParseUntypedKv()
{
    KvFull = CreateKeyValues("Root");
    FileToKeyValues(KvFull, "objectlibtest-kvfull.txt");    // Must be located in root of game directory.
    KvRewind(KvFull);
    
    new Object:parseContext = ObjLib_GetParseContext("objectlibtest");
    
    List = ObjLib_ParseInListMode(KvFull, parseContext);
    
    ObjLib_DeleteParseContext(parseContext);
    
    PrintToServer("KeyValue file parsed.");
}

stock ParseTypedKv()
{
    // Load and prepare a keyvalues file.
    KvFull = CreateKeyValues("Root");
    FileToKeyValues(KvFull, "objectlibtest-kvfull.txt");    // Must be located in root of game directory.
    KvRewind(KvFull);
    
    // Declare types. Deepest types/sections must be declared first so you can
    // later attach them to the parent type.
    
    // DataType section. Some keys have constraints.
    new ObjectType:dataTypes = ObjLib_CreateType(16);
    ObjLib_AddKey(dataTypes, "cell", ObjDataType_Cell, ObjLib_GetCellConstraints(true, true, true, 5, 15));
    ObjLib_AddKey(dataTypes, "bool", ObjDataType_Bool, ObjLib_GetBooleanLookupConstraints(BoolType_TrueFalse));
    ObjLib_AddKey(dataTypes, "float", ObjDataType_Float, ObjLib_GetFloatConstraints(true, true, true, 1.0, 5.0));
    ObjLib_AddKey(dataTypes, "string", ObjDataType_String);
    
    // Dummy object sections, used in collection example.
    new ObjectType:dummyType = ObjLib_CreateType();
    ObjLib_AddKey(dummyType, "dummyKey", ObjDataType_Cell);
    
    // Root section. NestedSections doesn't use any object constraints so that
    // the parser will add objects and keys automatically (strings).
    new ObjectType:rootType = ObjLib_CreateType();
    ObjLib_AddKey(rootType, "DataTypes", ObjDataType_Object, ObjLib_GetObjectConstraints(true, dataTypes));
    ObjLib_AddKey(rootType, "NestedSections", ObjDataType_Object);
    ObjLib_AddKey(rootType, "Collection", ObjDataType_Object, ObjLib_GetCollectionConstraints(ObjDataType_Cell, 1));
    
    // This key has collection constraints with sub constraints on collection
    // elements.
    ObjLib_AddKey(rootType, "CollectionOfObjects", ObjDataType_Object, ObjLib_GetCollectionConstraints(ObjDataType_Object, 1, ObjLib_GetObjectConstraints(true, dummyType)));
    
    // Get a parser context. This object stores parser state and settings. Most
    // default settings will do fine in this example, but it's recommended to
    // give it a name so it can be identified in error logs.
    new Object:parseContext = ObjLib_GetParseContext(
            "objectlibtest",    // name
            rootType,           // rootType
            true);              // ignoreEmptySections
    
    // Run parser. Parsed sections are added to a list.
    List = ObjLib_ParseInListMode(KvFull, parseContext);
    
    // All sections (except empty) in the KeyValue file is now parsed,
    // validated and stored in objects. The objects for the root sections are
    // added to the list. Sub sections are referenced through keys in objects.
    
    // Parser context object must be deleted when no longer in use, otherwise
    // there will be a memory leak.
    ObjLib_DeleteParseContext(parseContext);
    
    PrintToServer("KeyValue file parsed.");
}

public Action:Command_List(client, argc)
{
    new len = GetArraySize(List);
    
    ReplyToCommand(client, "Ojbects in list:");
    
    for (new i = 0; i < len; i++)
    {
        new object = GetArrayCell(List, i);
        ReplyToCommand(client, "0x%X", object);
    }
    
    return Plugin_Handled;
}

public Action:Command_Inspect(client, argc)
{
    new String:argBuffer[16];
    GetCmdArg(1, argBuffer, sizeof(argBuffer));
    
    new Object:object;
    
    // Parse hex string.
    StringToIntEx(argBuffer, _:object, 16);
    
    ReplyToCommand(client, "Inspecting object 0x%X.", object);
    
    ObjLib_DumpObjectKeys(client, object);
    
    return Plugin_Handled;
}

public Action:Command_InspectEx(client, argc)
{
    new String:argBuffer[16];
    GetCmdArg(1, argBuffer, sizeof(argBuffer));
    
    new Object:object;
    
    // Parse hex string.
    StringToIntEx(argBuffer, _:object, 16);
    
    ReplyToCommand(client, "Inspecting raw data in object 0x%X.", object);
    
    ObjLib_DumpRawObject(client, object);
    
    return Plugin_Handled;
}

public Action:Command_InspectTypeEx(client, argc)
{
    new String:argBuffer[16];
    GetCmdArg(1, argBuffer, sizeof(argBuffer));
    
    new ObjectType:typeDescriptor;
    
    // Parse hex string.
    StringToIntEx(argBuffer, _:typeDescriptor, 16);
    
    ReplyToCommand(client, "Inspecting raw data in object type 0x%X.", typeDescriptor);
    
    ObjLib_DumpRawType(client, typeDescriptor);
    
    return Plugin_Handled;
}
