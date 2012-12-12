#pragma semicolon 1
#include <sourcemod>
#include <pawnunit>
#include "libraries/objectlib"

new Handle:KvFull;
new Handle:List;

public OnPluginStart()
{
    RegConsoleCmd("object_list", CommandList, "List objects in root.");
    RegConsoleCmd("object_inspect", Command_Inspect, "Inspect an object. Usage: object_inspect <object address>");
    
    ParseKv();
    CommandList(0, 0);
}

ParseKv()
{
    KvFull = CreateKeyValues("Root");
    FileToKeyValues(KvFull, "objectlibtest-kvfull.txt");    // Must be located in root of game directory.
    
    List = ObjLib_ParseInListMode(KvFull, INVALID_OBJECT_TYPE, "sectionName");
    
    PrintToServer("KeyValue file parsed.");
}

public Action:CommandList(client, argc)
{
    new len = GetArraySize(List);
    
    ReplyToCommand(client, "Ojbects in list:");
    
    for (new i = 0; i < len; i++)
    {
        new object = GetArrayCell(List, i);
        ReplyToCommand(client, "0x%x", object);
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
    
    ReplyToCommand(client, "Inspecting object 0x%x.", object);
    
    ObjLib_DumpObjectKeys(client, object);
    
    return Plugin_Handled;
}
