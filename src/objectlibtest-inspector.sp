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
    
    ParseKv();
    Command_List(0, 0);
}

ParseKv()
{
    KvFull = CreateKeyValues("Root");
    FileToKeyValues(KvFull, "objectlibtest-kvfull.txt");    // Must be located in root of game directory.
    KvRewind(KvFull);
    
    new Object:parseContext = ObjLib_GetParseContext("objectlibtest");
    
    List = ObjLib_ParseInListMode(KvFull, parseContext);
    
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
