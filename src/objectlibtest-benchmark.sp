#pragma semicolon 1
#include <sourcemod>
#include <profiler>
#include "libraries/objectlib"

new Handle:profiler;

#define ITERATIONS  1000

public OnPluginStart()
{
    profiler = CreateProfiler();
    
    RegConsoleCmd("objectlib_benchmark_object", Command_BenchmarkObject, "Run a benchmark on objects. Usage: object_benchmark_object [iterations]");
}

GetIterations(argc)
{
    if (argc > 0)
    {
        new String:argBuffer[32];
        new iterations;
        
        GetCmdArg(1, argBuffer, sizeof(argBuffer));
        iterations = StringToInt(argBuffer);
        
        if (iterations <= 0)
        {
            return ITERATIONS;
        }
        
        return iterations;
    }
    else
    {
        return ITERATIONS;
    }
}

public Action:Command_BenchmarkObject(client, argc)
{
    new iterations = GetIterations(argc);
    
    PrintToServer("Running objectlib benchmarks with %d iterations\n-------------------------------------------------------", iterations);
    
    CreateDelete(iterations);
    CreateDeleteMutable(iterations);
    GetCell(iterations);
    SetCell(iterations);
    SetCellWithConstraints(iterations);
    GetCellLongKey(iterations);
    GetShortString(iterations);
    SetShortString(iterations);
    GetLongString(iterations);
    SetLongString(iterations);
    
    PrintToServer("Objectlib benchmarks done.");
    
    return Plugin_Handled;
}

CreateDelete(iterations)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType();
    new Object:object;
    
    PrintToServer("Running CreateDelete...", iterations);
    
    // Create and delete empty objects.
    StartProfiling(profiler);
    for (new i = 0; i < iterations; i++)
    {
        // Create an immutable object.
        object = ObjLib_CreateObject(typeDescriptor);
        
        // Delete it.
        ObjLib_DeleteObject(object);
    }
    StopProfiling(profiler);
    
    // Cleanup.
    ObjLib_DeleteType(typeDescriptor);
    
    PrintToServer("CreateDelete total:     %f", GetProfilerTime(profiler));
    PrintToServer("CreateDelete iteration: %f", GetProfilerTime(profiler) / float(iterations));
}

CreateDeleteMutable(iterations)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType();
    new Object:object;
    
    PrintToServer("Running CreateDeleteMutable...", iterations);
    
    // Create and delete empty objects.
    StartProfiling(profiler);
    for (new i = 0; i < iterations; i++)
    {
        // Create a mutable object.
        object = ObjLib_CreateObject(typeDescriptor, true);
        
        // Delete it.
        ObjLib_DeleteObject(object);
    }
    StopProfiling(profiler);
    
    // Cleanup.
    ObjLib_DeleteType(typeDescriptor);
    
    PrintToServer("CreateDeleteMutable total:     %f", GetProfilerTime(profiler));
    PrintToServer("CreateDeleteMutable iteration: %f", GetProfilerTime(profiler) / float(iterations));
}

GetCell(iterations)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType();
    ObjLib_AddKey(typeDescriptor, "testKey", ObjDataType_Cell);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    ObjLib_SetCell(object, "testKey", 10);
    
    PrintToServer("Running GetCell...", iterations);
    
    // Get cell value from object.
    StartProfiling(profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_GetCell(object, "testKey");
    }
    StopProfiling(profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    PrintToServer("GetCell total:     %f", GetProfilerTime(profiler));
    PrintToServer("GetCell iteration: %f", GetProfilerTime(profiler) / float(iterations));
}

SetCell(iterations)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType();
    ObjLib_AddKey(typeDescriptor, "testKey", ObjDataType_Cell);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    
    PrintToServer("Running SetCell...", iterations);
    
    // Set cell value in object.
    StartProfiling(profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_SetCell(object, "testKey", 10);
    }
    StopProfiling(profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    PrintToServer("SetCell total:     %f", GetProfilerTime(profiler));
    PrintToServer("SetCell iteration: %f", GetProfilerTime(profiler) / float(iterations));
}

SetCellWithConstraints(iterations)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType();
    
    new Object:constraints = ObjLib_GetCellConstraints(true, true, true, -20, 20);
    ObjLib_AddKey(typeDescriptor, "testKey", ObjDataType_Cell, constraints);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    
    PrintToServer("Running SetCellWithConstraints...", iterations);
    
    // Set cell value in object with constraints.
    StartProfiling(profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_SetCell(object, "testKey", 10);
    }
    StopProfiling(profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    PrintToServer("SetCellWithConstraints total:     %f", GetProfilerTime(profiler));
    PrintToServer("SetCellWithConstraints iteration: %f", GetProfilerTime(profiler) / float(iterations));
}

GetCellLongKey(iterations)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType(1, ByteCountToCells(48));
    ObjLib_AddKey(typeDescriptor, "aVeryLongTestKeyWithLotsOfCharacters", ObjDataType_Cell);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    ObjLib_SetCell(object, "aVeryLongTestKeyWithLotsOfCharacters", 10);
    
    PrintToServer("Running GetCellLongKey...", iterations);
    
    // Get cell value from object.
    StartProfiling(profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_GetCell(object, "aVeryLongTestKeyWithLotsOfCharacters");
    }
    StopProfiling(profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    PrintToServer("GetCellLongKey total:     %f", GetProfilerTime(profiler));
    PrintToServer("GetCellLongKey iteration: %f", GetProfilerTime(profiler) / float(iterations));
}

GetShortString(iterations)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType(ByteCountToCells(16));
    ObjLib_AddKey(typeDescriptor, "testKey", ObjDataType_String);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    ObjLib_SetString(object, "testKey", "A test string.");
    
    PrintToServer("Running GetShortString...", iterations);
    
    new String:buffer[64];
    
    // Get cell value from object.
    StartProfiling(profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_GetString(object, "testKey", buffer, sizeof(buffer));
    }
    StopProfiling(profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    PrintToServer("GetShortString total:     %f", GetProfilerTime(profiler));
    PrintToServer("GetShortString iteration: %f", GetProfilerTime(profiler) / float(iterations));
}

SetShortString(iterations)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType(ByteCountToCells(16));
    ObjLib_AddKey(typeDescriptor, "testKey", ObjDataType_String);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    
    PrintToServer("Running SetShortString...", iterations);
    
    // Set cell value in object.
    StartProfiling(profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_SetString(object, "testKey", "A test string.");
    }
    StopProfiling(profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    PrintToServer("SetShortString total:     %f", GetProfilerTime(profiler));
    PrintToServer("SetShortString iteration: %f", GetProfilerTime(profiler) / float(iterations));
}

GetLongString(iterations)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType(ByteCountToCells(256));
    ObjLib_AddKey(typeDescriptor, "testKey", ObjDataType_String);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    ObjLib_SetString(object, "testKey", "A test string. ABCDEFGHIJKLMNOPQRSTUVWXYZ. 0123456789. This is a much longer string that may be used as a description for something.");
    
    PrintToServer("Running GetLongString...", iterations);
    
    new String:buffer[64];
    
    // Get cell value from object.
    StartProfiling(profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_GetString(object, "testKey", buffer, sizeof(buffer));
    }
    StopProfiling(profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    PrintToServer("GetLongString total:     %f", GetProfilerTime(profiler));
    PrintToServer("GetLongString iteration: %f", GetProfilerTime(profiler) / float(iterations));
}

SetLongString(iterations)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType(ByteCountToCells(256));
    ObjLib_AddKey(typeDescriptor, "testKey", ObjDataType_String);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    
    PrintToServer("Running SetLongString...", iterations);
    
    // Set cell value in object.
    StartProfiling(profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_SetString(object, "testKey", "A test string. ABCDEFGHIJKLMNOPQRSTUVWXYZ. 0123456789. This is a much longer string that may be used as a description for something.");
    }
    StopProfiling(profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    PrintToServer("SetLongString total:     %f", GetProfilerTime(profiler));
    PrintToServer("SetLongString iteration: %f", GetProfilerTime(profiler) / float(iterations));
}
