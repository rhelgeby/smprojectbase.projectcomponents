#pragma semicolon 1
#include <sourcemod>
#include <profiler>
#include "libraries/objectlib"

new Handle:Profiler;

#define ITERATIONS  1000

public OnPluginStart()
{
    Profiler = CreateProfiler();
    
    RegConsoleCmd("objectlib_benchmark_object", Command_BenchmarkObject, "Run a benchmark on objects. Usage: object_benchmark_object [iterations]");
    RegConsoleCmd("objectlib_benchmark_object_report", Command_BenchmarkObjectReport, "Run a benchmark on objects and print a formatted report. Usage: object_benchmark_object_report [max iterations]");
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

Report(iterations, bool:report, const String:name[])
{
    if (report)
    {
        PrintToServer("%d\t%f\t%f", iterations, GetProfilerTime(Profiler), GetProfilerTime(Profiler) / float(iterations));
    }
    else
    {
        PrintToServer("%s total:     %f", name, GetProfilerTime(Profiler));
        PrintToServer("%s iteration: %f", name, GetProfilerTime(Profiler) / float(iterations));
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

public Action:Command_BenchmarkObjectReport(client, argc)
{
    new max = GetIterations(argc);
    
    PrintToServer("Running objectlib benchmark report. Max iterations: %d", max);
    PrintToServer("Format: <iterations> <total time> <iteration time>");
    PrintToServer("This may take a few minutes...");
    
    // Double iterations per loop iteration.
    #define REPORT_LOOP     for(new i = 64; i < max; i *= 2)
    
    PrintToServer("CreateDelete");
    REPORT_LOOP {CreateDelete(i, true);}
    
    PrintToServer("CreateDeleteMutable");
    REPORT_LOOP {CreateDeleteMutable(i, true);}
    
    PrintToServer("GetCell");
    REPORT_LOOP {GetCell(i, true);}
    
    PrintToServer("SetCell");
    REPORT_LOOP {SetCell(i, true);}
    
    PrintToServer("SetCellWithConstraints");
    REPORT_LOOP {SetCellWithConstraints(i, true);}
    
    PrintToServer("GetCellLongKey");
    REPORT_LOOP {GetCellLongKey(i, true);}
    
    PrintToServer("GetShortString");
    REPORT_LOOP {GetShortString(i, true);}
    
    PrintToServer("SetShortString");
    REPORT_LOOP {SetShortString(i, true);}
    
    PrintToServer("GetLongString");
    REPORT_LOOP {GetLongString(i, true);}
    
    PrintToServer("SetLongString");
    REPORT_LOOP {SetLongString(i, true);}
    
    PrintToServer("Objectlib benchmarks done.");
    
    return Plugin_Handled;
}

CreateDelete(iterations, bool:report = false)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType();
    new Object:object;
    
    if (!report) PrintToServer("Running CreateDelete...");
    
    // Create and delete empty objects.
    StartProfiling(Profiler);
    for (new i = 0; i < iterations; i++)
    {
        // Create an immutable object.
        object = ObjLib_CreateObject(typeDescriptor);
        
        // Delete it.
        ObjLib_DeleteObject(object);
    }
    StopProfiling(Profiler);
    
    // Cleanup.
    ObjLib_DeleteType(typeDescriptor);
    
    Report(iterations, report, "CreateDelete");
}

CreateDeleteMutable(iterations, bool:report = false)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType();
    new Object:object;
    
    if (!report) PrintToServer("Running CreateDeleteMutable...");
    
    // Create and delete empty objects.
    StartProfiling(Profiler);
    for (new i = 0; i < iterations; i++)
    {
        // Create a mutable object.
        object = ObjLib_CreateObject(typeDescriptor, true);
        
        // Delete it.
        ObjLib_DeleteObject(object);
    }
    StopProfiling(Profiler);
    
    // Cleanup.
    ObjLib_DeleteType(typeDescriptor);
    
    Report(iterations, report, "CreateDeleteMutable");
}

GetCell(iterations, bool:report = false)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType();
    ObjLib_AddKey(typeDescriptor, "testKey", ObjDataType_Cell);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    ObjLib_SetCell(object, "testKey", 10);
    
    if (!report) PrintToServer("Running GetCell...");
    
    // Get cell value from object.
    StartProfiling(Profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_GetCell(object, "testKey");
    }
    StopProfiling(Profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    Report(iterations, report, "GetCell");
}

SetCell(iterations, bool:report = false)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType();
    ObjLib_AddKey(typeDescriptor, "testKey", ObjDataType_Cell);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    
    if (!report) PrintToServer("Running SetCell...");
    
    // Set cell value in object.
    StartProfiling(Profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_SetCell(object, "testKey", 10);
    }
    StopProfiling(Profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    Report(iterations, report, "SetCell");
}

SetCellWithConstraints(iterations, bool:report = false)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType();
    
    new Object:constraints = ObjLib_GetCellConstraints(true, true, true, -20, 20);
    ObjLib_AddKey(typeDescriptor, "testKey", ObjDataType_Cell, constraints);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    
    if (!report) PrintToServer("Running SetCellWithConstraints...");
    
    // Set cell value in object with constraints.
    StartProfiling(Profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_SetCell(object, "testKey", 10);
    }
    StopProfiling(Profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    Report(iterations, report, "SetCellWithConstraints");
}

GetCellLongKey(iterations, bool:report = false)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType(1, ByteCountToCells(48));
    ObjLib_AddKey(typeDescriptor, "aVeryLongTestKeyWithLotsOfCharacters", ObjDataType_Cell);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    ObjLib_SetCell(object, "aVeryLongTestKeyWithLotsOfCharacters", 10);
    
    if (!report) PrintToServer("Running GetCellLongKey...");
    
    // Get cell value from object.
    StartProfiling(Profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_GetCell(object, "aVeryLongTestKeyWithLotsOfCharacters");
    }
    StopProfiling(Profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    Report(iterations, report, "GetCellLongKey");
}

GetShortString(iterations, bool:report = false)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType(ByteCountToCells(16));
    ObjLib_AddKey(typeDescriptor, "testKey", ObjDataType_String);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    ObjLib_SetString(object, "testKey", "A test string.");
    
    if (!report) PrintToServer("Running GetShortString...");
    
    new String:buffer[64];
    
    // Get cell value from object.
    StartProfiling(Profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_GetString(object, "testKey", buffer, sizeof(buffer));
    }
    StopProfiling(Profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    Report(iterations, report, "GetShortString");
}

SetShortString(iterations, bool:report = false)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType(ByteCountToCells(16));
    ObjLib_AddKey(typeDescriptor, "testKey", ObjDataType_String);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    
    if (!report) PrintToServer("Running SetShortString...");
    
    // Set cell value in object.
    StartProfiling(Profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_SetString(object, "testKey", "A test string.");
    }
    StopProfiling(Profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    Report(iterations, report, "SetShortString");
}

GetLongString(iterations, bool:report = false)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType(ByteCountToCells(256));
    ObjLib_AddKey(typeDescriptor, "testKey", ObjDataType_String);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    ObjLib_SetString(object, "testKey", "A test string. ABCDEFGHIJKLMNOPQRSTUVWXYZ. 0123456789. This is a much longer string that may be used as a description for something.");
    
    if (!report) PrintToServer("Running GetLongString...");
    
    new String:buffer[64];
    
    // Get cell value from object.
    StartProfiling(Profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_GetString(object, "testKey", buffer, sizeof(buffer));
    }
    StopProfiling(Profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    Report(iterations, report, "GetLongString");
}

SetLongString(iterations, bool:report = false)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType(ByteCountToCells(256));
    ObjLib_AddKey(typeDescriptor, "testKey", ObjDataType_String);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    
    if (!report) PrintToServer("Running SetLongString...");
    
    // Set cell value in object.
    StartProfiling(Profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_SetString(object, "testKey", "A test string. ABCDEFGHIJKLMNOPQRSTUVWXYZ. 0123456789. This is a much longer string that may be used as a description for something.");
    }
    StopProfiling(Profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    Report(iterations, report, "SetLongString");
}
