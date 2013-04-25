#pragma semicolon 1
#include <sourcemod>
#include <profiler>
#include "libraries/objectlib"

new Handle:Profiler;

// Don't set too high. It's used to create arrays in some tests.
#define ITERATIONS  1000

public OnPluginStart()
{
    Profiler = CreateProfiler();
    
    RegConsoleCmd("objectlib_benchmark_object", Command_BenchmarkObject, "Run a benchmark on objects. Usage: object_benchmark_object [iterations] [short iterations]");
    RegConsoleCmd("objectlib_benchmark_object_report", Command_BenchmarkObjectReport, "Run a benchmark on objects and print a formatted report. Usage: object_benchmark_object_report [max iterations] [max short iterations]");
}

GetArg(argc, arg = 1)
{
    if (argc >= arg)
    {
        new String:argBuffer[32];
        new value;
        
        GetCmdArg(arg, argBuffer, sizeof(argBuffer));
        value = StringToInt(argBuffer);
        
        if (value <= 0)
        {
            return ITERATIONS;
        }
        
        return value;
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
    new iterations = GetArg(argc);
    new shortIterations = GetArg(argc, 2);
    
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
    AddKey(shortIterations);
    RemoveKey(shortIterations);
    CreateBasicType(shortIterations);
    LookupKey(iterations);
    
    PrintToServer("Objectlib benchmarks done.");
    
    return Plugin_Handled;
}

public Action:Command_BenchmarkObjectReport(client, argc)
{
    new max = GetArg(argc);
    new shortMax = GetArg(argc, 2);
    
    PrintToServer("Running objectlib benchmark report. Max iterations: %d", max);
    PrintToServer("Format: <iterations> <total time> <iteration time>");
    PrintToServer("This may take a few minutes...");
    
    // Double iterations per loop iteration.
    #define REPORT_LOOP         for(new i = 64; i < max; i *= 2)
    #define REPORT_LOOP_SHORT   for(new i = 64; i < shortMax; i *= 2)
    
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
    
    PrintToServer("AddKey");
    REPORT_LOOP_SHORT {AddKey(i, true);}
    
    PrintToServer("RemoveKey");
    REPORT_LOOP_SHORT {RemoveKey(i, true);}
    
    PrintToServer("CreateBasicType");
    REPORT_LOOP_SHORT {CreateBasicType(i, true);}
    
    PrintToServer("LookupKey");
    REPORT_LOOP {LookupKey(i, true);}
    
    PrintToServer("Objectlib benchmarks done.");
    
    return Plugin_Handled;
}

GetRandomCharacters(numChars, String:buffer[], maxlen)
{
    if (numChars > maxlen)
    {
        numChars = maxlen - 1;
    }
    
    new count = 0;
    for (new i = 0; i < numChars; i++)
    {
        // Write random letter.
        buffer[count] = (GetURandomInt() % 27) + 97;
        count++;
    }
    
    // Terminate string.
    buffer[count] = 0;
    
    return count;
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

AddKey(iterations, bool:report = false)
{
    decl ObjectType:typeDescriptor[iterations];
    
    if (!report) PrintToServer("Preparing AddKey...");
    
    // Create type descriptors.
    for (new i = 0; i < iterations; i++)
    {
        if (!report && ObjectTypeCount % 64 == 0)
        {
            PrintToServer("%d type descriptors...", ObjectTypeCount);
        }
        typeDescriptor[i] = ObjLib_CreateType();
    }
    
    if (!report) PrintToServer("Running AddKey, %d iterations...", iterations);
    
    // Add key to each type descriptor.
    StartProfiling(Profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_AddKey(typeDescriptor[i], "testKey", ObjDataType_Cell);
    }
    StopProfiling(Profiler);
    
    // Cleanup.
    if (!report) PrintToServer("Cleanup AddKey...");
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_DeleteType(typeDescriptor[i]);
    }
    
    Report(iterations, report, "AddKey");
}

RemoveKey(iterations, bool:report = false)
{
    decl ObjectType:typeDescriptor[iterations];
    
    if (!report) PrintToServer("Preparing AddKey...");
    
    // Create type descriptors.
    for (new i = 0; i < iterations; i++)
    {
        if (!report && ObjectTypeCount % 64 == 0)
        {
            PrintToServer("%d type descriptors...", ObjectTypeCount);
        }
        
        typeDescriptor[i] = ObjLib_CreateType();
        ObjLib_AddKey(typeDescriptor[i], "testKey", ObjDataType_Cell);
    }
    
    if (!report) PrintToServer("Running AddKey, %d iterations...", iterations);
    
    // Add key to each type descriptor.
    StartProfiling(Profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_RemoveKey(typeDescriptor[i], "testKey");
    }
    StopProfiling(Profiler);
    
    // Cleanup.
    if (!report) PrintToServer("Cleanup AddKey...");
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_DeleteType(typeDescriptor[i]);
    }
    
    Report(iterations, report, "AddKey");
}

CreateBasicType(iterations, bool:report = false)
{
    decl ObjectType:typeDescriptor[iterations];
    new ObjectType:type;
    
    if (!report) PrintToServer("Running CreateBasicType, %d iterations...", iterations);
    
    // Create type descriptors.
    StartProfiling(Profiler);
    for (new i = 0; i < iterations; i++)
    {
        type = ObjLib_CreateType();
        ObjLib_AddKey(type, "valueA", ObjDataType_Cell);
        ObjLib_AddKey(type, "valueB", ObjDataType_Float);
        ObjLib_AddKey(type, "valueC", ObjDataType_Handle);
        ObjLib_AddKey(type, "stringA", ObjDataType_String);
        ObjLib_AddKey(type, "stringB", ObjDataType_String);
        
        typeDescriptor[i] = type;
    }
    StopProfiling(Profiler);
    
    // Cleanup.
    if (!report) PrintToServer("Cleanup CreateBasicType...");
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_DeleteType(typeDescriptor[i]);
    }
    
    Report(iterations, report, "CreateBasicType");
}

LookupKey(iterations, bool:report = false)
{
    new ObjectType:typeDescriptor = ObjLib_CreateType();
    new String:keyName[32];
    
    if (!report) PrintToServer("Preparing LookupKey...");
    
    // Add lots of dummy keys of random characters. 256 random keys.
    for (new i = 0; i < 256; i++)
    {
        GetRandomCharacters(30, keyName, sizeof(keyName) - 1);
        ObjLib_AddKey(typeDescriptor, keyName, ObjDataType_Cell);
    }
    
    // The key we're looking up.
    ObjLib_AddKey(typeDescriptor, "testKey", ObjDataType_Cell);
    
    new Object:object = ObjLib_CreateObject(typeDescriptor);
    ObjLib_SetCell(object, "testKey", 10);  // Dummy value.
    
    if (!report) PrintToServer("Running LookupKey...");
    
    // Get cell value from object, implies looking up a key.
    StartProfiling(Profiler);
    for (new i = 0; i < iterations; i++)
    {
        ObjLib_GetCell(object, "testKey");
    }
    StopProfiling(Profiler);
    
    // Cleanup.
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(typeDescriptor);
    
    Report(iterations, report, "LookupKey");
}
