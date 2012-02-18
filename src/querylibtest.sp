#pragma semicolon 1
#include <sourcemod>
#include "libraries/querylib"

#define STRING_BUFFER_LEN 255
#define MAX_TEST_ELEMENTS 32
#define TEST_NAME_LEN 16

#define FLAG_A 1
#define FLAG_B 2

enum TestElement
{
    // Element identifier.
    String:Test_Name[TEST_NAME_LEN],
    
    // Flags for testing filters.
    bool:Test_FlagA,
    bool:Test_FlagB
}

new TestData[MAX_TEST_ELEMENTS][TestElement];   // Data elements.
new Handle:TestList;                            // List of element indexes.
new bool:Accessible[MAX_TEST_ELEMENTS];         // Whether the element is accessible (simple authorization testing).
new ElementCount;                               // Number of elements.
new Handle:NameIndex;                           // Element name index.

new Handle:CollectionA;
new Handle:CollectionB;
new Handle:CollectionC;

new Handle:CallbackSet;

public OnPluginStart()
{
    BuildTestData();
    BuildCollections();
    
    // Build callback set.
    CallbackSet = QueryLib_CreateCallbackSet(GetElementList, GetElementIndex, GetFilterValue, PassedFilter, GetCollection);
    
    RegConsoleCmd("sm_querylib", Command_Info, "Lists test commands.");
    RegConsoleCmd("sm_querylib_run", Command_Run, "Executes a query. Usage: sm_querylib_run <query>");
    RegConsoleCmd("sm_querylib_setaccess", Command_Access, "Sets whether you can access a element or not. Usage: sm_querylib_setaccess <element|index> <0|1>");
    RegConsoleCmd("sm_querylib_dump", Command_Dump, "Dumps elements data to the console. Usage: sm_querylib_dump <element|index>");
    RegConsoleCmd("sm_querylib_test", Command_Test, "Starts unit tests.");
}

public Action:Command_Info(client, argc)
{
    ReplyToCommand(client, "Commands: sm_querylib_run, sm_querylib_setaccess, sm_querylib_dump, sm_querylib_test");
    return Plugin_Handled;
}

public Action:Command_Run(client, argc)
{
    // Check if a query is available.
    if (argc >= 1)
    {
        // Get the query.
        decl String:query[QUERYLIB_MAX_QUERY_LEN];
        query[0] = 0;
        GetCmdArgString(query, sizeof(query));      // Full argument string (excluding command name).
        
        // Prepare result list.
        new Handle:list = CreateArray();
        
        // Run query.
        new errPos = 0;
        new errCode = 0;
        QueryLib_ParseQuery(client, query, CallbackSet, QueryLib_List, list, errCode, errPos);
        
        // Check for parse errors.
        if (errCode < 0)
        {
            decl String:errMsg[STRING_BUFFER_LEN];
            errMsg[0] = 0;
            decl String:errMarker[QUERYLIB_MAX_QUERY_LEN];
            errMarker[0] = 0;
            
            // Print error message.
            QueryLib_ErrCodeToString(errCode, errMsg, sizeof(errMsg));
            ReplyToCommand(client, "Error %d: Parse error at column %d: %s\n", errCode, errPos, errMsg);
            
            // Print query string with marker of error position.
            ReplyToCommand(client, query);
            Array_GetStringMarker(errPos, errMarker, sizeof(errMarker));
            ReplyToCommand(client, errMarker);
            
            return Plugin_Handled;
        }
        
        // Print result list.
        PrintList(client, list);
    }
    else
    {
        // No parameters. Print syntax.
        ReplyToCommand(client, "Executes a query. Usage: sm_querylib_run <query>");
        return Plugin_Handled;
    }
    
    return Plugin_Handled;
}

public Action:Command_Access(client, argc)
{
    if (argc < 2)
    {
        ReplyToCommand(client, "Sets whether you can access a element or not. Usage: sm_querylib_setaccess <element|index> <0|1>");
        return Plugin_Handled;
    }
    
    // Get element index.
    new element = -1;
    new String:strElement[TEST_NAME_LEN];
    GetCmdArg(1, strElement, sizeof(strElement));
    if (IsCharNumeric(strElement[0]))
    {
        // Assume it's a element index.
        element = StringToInt(strElement);
    }
    else
    {
        // Resolve index from element name.
        element = GetElementIndex(strElement);
    }
    
    if (!IsValidElement(element))
    {
        ReplyToCommand(client, "Invalid element name or index.");
        return Plugin_Handled;
    }
    
    // Get value.
    new bool:value;
    new String:strValue[4];
    GetCmdArg(2, strValue, sizeof(strValue));
    value = bool:StringToInt(strValue);
    
    // Set access value.
    Accessible[element] = value;
    ReplyToCommand(client, "Access on element %d set to %d.", element, value);
    
    return Plugin_Handled;
}

public Action:Command_Dump(client, argc)
{
    if (argc == 1)
    {
        // Get element.
        
        // Print element info.
        
        
        // Get element index.
        /*new element = -1;
        new String:strElement[TEST_NAME_LEN];
        GetCmdArg(1, strElement, sizeof(strElement));
        if (IsCharNumeric(strElement[0]))
        {
            // Assume it's a element index.
            element = StringToInt(strElement);
        }
        else
        {
            // Resolve index from element name.
            element = GetElementIndex(strElement);
        }
        
        if (!IsValidElement(element))
        {
            ReplyToCommand(client, "Invalid element name or index.");
            return Plugin_Handled;
        }*/
    }
    else
    {
        PrintList(client);
        
        ReplyToCommand(client, "\nCollections: foobarbazqux, foobar, barbaz");
    }
    
    return Plugin_Handled;
}

public Action:Command_Test(client, argc)
{
    RunUnitTests(client);
    return Plugin_Handled;
}

PrintList(client, Handle:list = INVALID_HANDLE)
{
    static const String:fmt[] = "%-8d %-8s %-8d %-8d %-12d";
    static const String:fmtHeader[] = "%-8s %-8s %-8s %-8s %-12s\n-----------------------------------------------";
    
    new bool:hasList = (list != INVALID_HANDLE);
    new count = hasList ? GetArraySize(list) : ElementCount;
    
    // Print header.
    ReplyToCommand(client, fmtHeader, "Index:", "Name:", "FlagA:", "FlagB:", "Accessible:");
    
    if (count == 0)
    {
        ReplyToCommand(client, "(No elements)");
        return;
    }
    
    // Loop through each element.
    new element;
    for (new i = 0; i < count; i++)
    {
        element = hasList ? GetArrayCell(list, i) : i;
        ReplyToCommand(client, fmt, 
                element,
                TestData[element][Test_Name],
                TestData[element][Test_FlagA],
                TestData[element][Test_FlagB],
                Accessible[element]);
    }
    
    ReplyToCommand(client, "\n%d elements listed.\n", count);
}

/************************
 *   PARSER CALLBACKS   *
 ************************/

public Handle:GetElementList()
{
    return TestList;
}

public GetElementIndex(const String:name[])
{
    new element = -1;
    
    // Lookup the element name in the index.
    if (!GetTrieValue(NameIndex, name, element))
    {
        return -1;
    }
    
    return element;
}

public GetFilterValue(const String:name[])
{
    if (StrEqual(name, "flaga"))
    {
        return FLAG_A;
    }
    else if (StrEqual(name, "flagb"))
    {
        return FLAG_B;
    }
    
    return 0;
}

public bool:PassedFilter(client, element, filter)
{
    if (filter & FLAG_A
        && !TestData[element][Test_FlagA])
    {
        return false;
    }
    
    if (filter & FLAG_B
        && !TestData[element][Test_FlagB])
    {
        return false;
    }
    
    // Simple authorization.
    if (client != 0 && !Accessible[element])
    {
        return false;
    }
    
    return true;
}

public Handle:GetCollection(const String:name[])
{
    if (StrEqual(name, "foobarbazqux"))
    {
        return CollectionA;
    }
    else if (StrEqual(name, "foobar"))
    {
        return CollectionB;
    }
    else if (StrEqual(name, "barbaz"))
    {
        return CollectionC;
    }
    else
    {
        return INVALID_HANDLE;
    }
}


/*****************
 *   TEST DATA   *
 *****************/

BuildTestData()
{
    NameIndex = CreateTrie();
    TestList = CreateArray();
    
    // Make all elements accessible.
    for (new i = 0; i < MAX_TEST_ELEMENTS; i ++)
    {
        Accessible[i] = true;
    }
    
    ElementCount = 0;
    
    // 0
    strcopy(TestData[ElementCount][Test_Name], TEST_NAME_LEN, "foo");
    TestData[ElementCount][Test_FlagA] = false;
    TestData[ElementCount][Test_FlagB] = false;
    PushArrayCell(TestList, ElementCount);
    SetTrieValue(NameIndex, "foo", ElementCount);
    ElementCount++;
    
    // 1
    strcopy(TestData[ElementCount][Test_Name], TEST_NAME_LEN, "foo2");
    TestData[ElementCount][Test_FlagA] = true;
    TestData[ElementCount][Test_FlagB] = false;
    PushArrayCell(TestList, ElementCount);
    SetTrieValue(NameIndex, "foo2", ElementCount);
    ElementCount++;
    
    // 2
    strcopy(TestData[ElementCount][Test_Name], TEST_NAME_LEN, "foo3");
    TestData[ElementCount][Test_FlagA] = true;
    TestData[ElementCount][Test_FlagB] = true;
    PushArrayCell(TestList, ElementCount);
    SetTrieValue(NameIndex, "foo3", ElementCount);
    ElementCount++;
    
    //-----
    
    // 3
    strcopy(TestData[ElementCount][Test_Name], TEST_NAME_LEN, "bar");
    TestData[ElementCount][Test_FlagA] = false;
    TestData[ElementCount][Test_FlagB] = false;
    PushArrayCell(TestList, ElementCount);
    SetTrieValue(NameIndex, "bar", ElementCount);
    ElementCount++;
    
    // 4
    strcopy(TestData[ElementCount][Test_Name], TEST_NAME_LEN, "bar2");
    TestData[ElementCount][Test_FlagA] = true;
    TestData[ElementCount][Test_FlagB] = false;
    PushArrayCell(TestList, ElementCount);
    SetTrieValue(NameIndex, "bar2", ElementCount);
    ElementCount++;
    
    // 5
    strcopy(TestData[ElementCount][Test_Name], TEST_NAME_LEN, "bar3");
    TestData[ElementCount][Test_FlagA] = true;
    TestData[ElementCount][Test_FlagB] = true;
    PushArrayCell(TestList, ElementCount);
    SetTrieValue(NameIndex, "bar3", ElementCount);
    ElementCount++;
    
    //-----
    
    // 6
    strcopy(TestData[ElementCount][Test_Name], TEST_NAME_LEN, "baz");
    TestData[ElementCount][Test_FlagA] = false;
    TestData[ElementCount][Test_FlagB] = true;
    PushArrayCell(TestList, ElementCount);
    SetTrieValue(NameIndex, "baz", ElementCount);
    ElementCount++;
    
    // 7
    strcopy(TestData[ElementCount][Test_Name], TEST_NAME_LEN, "baz2");
    TestData[ElementCount][Test_FlagA] = true;
    TestData[ElementCount][Test_FlagB] = false;
    PushArrayCell(TestList, ElementCount);
    SetTrieValue(NameIndex, "baz2", ElementCount);
    ElementCount++;
    
    // 8
    strcopy(TestData[ElementCount][Test_Name], TEST_NAME_LEN, "baz3");
    TestData[ElementCount][Test_FlagA] = true;
    TestData[ElementCount][Test_FlagB] = false;
    PushArrayCell(TestList, ElementCount);
    SetTrieValue(NameIndex, "baz3", ElementCount);
    ElementCount++;
    
    //-----
    
    // 9
    strcopy(TestData[ElementCount][Test_Name], TEST_NAME_LEN, "qux");
    TestData[ElementCount][Test_FlagA] = false;
    TestData[ElementCount][Test_FlagB] = true;
    PushArrayCell(TestList, ElementCount);
    SetTrieValue(NameIndex, "qux", ElementCount);
    ElementCount++;
    
    // 10
    strcopy(TestData[ElementCount][Test_Name], TEST_NAME_LEN, "qux2");
    TestData[ElementCount][Test_FlagA] = true;
    TestData[ElementCount][Test_FlagB] = false;
    PushArrayCell(TestList, ElementCount);
    SetTrieValue(NameIndex, "qux2", ElementCount);
    ElementCount++;
    
    // 11
    strcopy(TestData[ElementCount][Test_Name], TEST_NAME_LEN, "qux3");
    TestData[ElementCount][Test_FlagA] = true;
    TestData[ElementCount][Test_FlagB] = false;
    PushArrayCell(TestList, ElementCount);
    SetTrieValue(NameIndex, "qux3", ElementCount);
    ElementCount++;
}

BuildCollections()
{
    CollectionA = CreateArray();
    CollectionB = CreateArray();
    CollectionC = CreateArray();
    
    // foo, bar, baz, qux
    PushArrayCell(CollectionA, 0);
    PushArrayCell(CollectionA, 3);
    PushArrayCell(CollectionA, 6);
    PushArrayCell(CollectionA, 9);
    
    // foo - bar3
    PushArrayCell(CollectionB, 0);
    PushArrayCell(CollectionB, 1);
    PushArrayCell(CollectionB, 2);
    PushArrayCell(CollectionB, 3);
    PushArrayCell(CollectionB, 4);
    PushArrayCell(CollectionB, 5);
    
    // bar - baz3
    PushArrayCell(CollectionC, 3);
    PushArrayCell(CollectionC, 4);
    PushArrayCell(CollectionC, 5);
    PushArrayCell(CollectionC, 6);
    PushArrayCell(CollectionC, 7);
    PushArrayCell(CollectionC, 8);
}

bool:IsValidElement(element)
{
    return element >= 0 && element < ElementCount;
}

/******************
 *   UNIT TESTS   *
 ******************/

// Join querylib section to get access to private querylib helper functions.
//#section querylib

BoolToResult(bool:value, String:buffer[], maxlen)
{
    if (value)
    {
        strcopy(buffer, maxlen, "OK");
    }
    else
    {
        strcopy(buffer, maxlen, "FAILED");
    }
}

RunUnitTests(client)
{
    static const String:fmt[] = "%32s %10s";
    static const String:fmtHeader[] = "%32s %10s\n-------------------------------------------";
    
    new String:result[16];
    
    // Print header.
    ReplyToCommand(client, fmtHeader, "Test name:", "Result:");
    
    // Run tests.
    BoolToResult(TestEntryType_Element(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestEntryType_Element", result);
    
    BoolToResult(TestEntryType_FilterCollection(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestEntryType_FilterCollection", result);
    
    BoolToResult(TestEntryType_FilterCollection2(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestEntryType_FilterCollection2", result);
    
    BoolToResult(TestEntryType_Collection(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestEntryType_Collection", result);
    
    BoolToResult(TestEntryType_Collection2(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestEntryType_Collection2", result);
    
    BoolToResult(TestEntryType_Filter(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestEntryType_Filter", result);
    
    BoolToResult(TestEntryType_Filter2(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestEntryType_Filter2", result);
    
    BoolToResult(TestEntryType_SelectMode(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestEntryType_SelectMode", result);
    
    BoolToResult(TestEntryType_Invalid(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestEntryType_Invalid", result);
    
    BoolToResult(TestEntryType_Invalid2(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestEntryType_Invalid2", result);
    
    
    BoolToResult(TestSplitTypes(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestSplitTypes", result);
    
    
    BoolToResult(TestUpdateCounters_List(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestUpdateCounters_List", result);
    
    BoolToResult(TestUpdateCounters_Element(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestUpdateCounters_Element", result);
    
    
    BoolToResult(TestResultValue_Handle(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestResultValue_Handle", result);
    
    BoolToResult(TestResultValue_Handle2(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestResultValue_Handle2", result);
    
    BoolToResult(TestResultValue_Handle3(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestResultValue_Handle3", result);
    
    BoolToResult(TestResultValue_Handle4(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestResultValue_Handle4", result);
    
    BoolToResult(TestResultValue_Handle5(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestResultValue_Handle5", result);
    
    BoolToResult(TestResultValue_NoResult(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestResultValue_NoResult", result);
    
    BoolToResult(TestResultValue_First(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestResultValue_First", result);
    
    BoolToResult(TestResultValue_Last(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestResultValue_Last", result);
    
    BoolToResult(TestResultValue_Random(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestResultValue_Random", result);
    
    BoolToResult(TestResultValue_List(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestResultValue_List", result);
    
    BoolToResult(TestResultValue_InvalidMode(), result, sizeof(result));
    ReplyToCommand(client, fmt, "TestResultValue_InvalidMode", result);
}

/*
 * QueryLib_GetEntryType
 */

bool:TestEntryType_Element()
{
    static const String:entry[] = "dummyElement";
    return QueryLib_GetEntryType(entry) == QueryLibEntry_Element;
}

bool:TestEntryType_FilterCollection()
{
    static const String:entry[] = "f:someFilter:someCollection";
    return QueryLib_GetEntryType(entry) == QueryLibEntry_FilterCollection;
}

bool:TestEntryType_FilterCollection2()
{
    static const String:entry[] = "f:someFilter,secondFilter:someCollection,secondCollection";
    return QueryLib_GetEntryType(entry) == QueryLibEntry_FilterCollection;
}

bool:TestEntryType_Collection()
{
    static const String:entry[] = "c:someCollection";
    return QueryLib_GetEntryType(entry) == QueryLibEntry_Collection;
}

bool:TestEntryType_Collection2()
{
    static const String:entry[] = "c:someCollection,otherCollection";
    return QueryLib_GetEntryType(entry) == QueryLibEntry_Collection;
}

bool:TestEntryType_Filter()
{
    static const String:entry[] = "f:filterA";
    return QueryLib_GetEntryType(entry) == QueryLibEntry_Filter;
}

bool:TestEntryType_Filter2()
{
    static const String:entry[] = "f:filterA,filterB";
    return QueryLib_GetEntryType(entry) == QueryLibEntry_Filter;
}

bool:TestEntryType_SelectMode()
{
    static const String:entry[] = "s:aSelectMode";
    return QueryLib_GetEntryType(entry) == QueryLibEntry_SelectMode;
}

bool:TestEntryType_Invalid()
{
    static const String:entry[] = "a:test";
    return QueryLib_GetEntryType(entry) == QueryLibEntry_Invalid;
}

bool:TestEntryType_Invalid2()
{
    static const String:entry[] = "a:b:c:d";
    return QueryLib_GetEntryType(entry) == QueryLibEntry_Invalid;
}


/*
 * QueryLib_SplitTypes
 */

bool:TestSplitTypes()
{
    // Note: This test assumes that the entry string is valid.
    
    static const String:entry[] = "f:filter1,filter2,filter3:collection1,collection2,collection3";
    
    new bool:result = true;
    
    new String:filterList[32];
    new String:collectionList[32];
    
    QueryLib_SplitTypes(entry, filterList, collectionList, 32);
    
    // Verify filters. Expect 3 filters.
    new String:filterExploded[5][32];
    if (ExplodeString(filterList, QUERYLIB_LIST_SEPARATOR, filterExploded, 5, 32) != 3)
    {
        result = false;
    }
    
    // Verify collections. Expect 3 collections.
    new String:collectionExploded[5][32];
    if (ExplodeString(collectionList, QUERYLIB_LIST_SEPARATOR, collectionExploded, 5, 32) != 3)
    {
        result = false;
    }
    
    return result;
}


/*
 * QueryLib_UpdateCounters
 */

bool:TestUpdateCounters_List()
{
    new bool:asList = true;
    new listCount = 10;     // 10 total elements before this update.
    new resultCount = 3;    // 3 elements was already added.
    new retval = 5;         // 5 new elements was added now.
    
    QueryLib_UpdateCounters(asList, listCount, resultCount, retval);
    
    // Expect listCount and resultCount to have five more elements.
    return (listCount == 15 && resultCount == 8);
}

bool:TestUpdateCounters_Element()
{
    new bool:asList = false;
    new listCount = 5;      // 5 total elements before this update.
    new resultCount = 1;    // 1 elements was already added.
    new retval = 7;         // Element at index 7 was added now (this value
                            // should be ignored now).
    
    QueryLib_UpdateCounters(asList, listCount, resultCount, retval);
    
    // Expect listCount and resultCount to have one more element.
    return (listCount == 6 && resultCount == 2);
}


/*
 * QueryLib_GetResultValue
 */

bool:TestResultValue_Handle()
{
    // Create test list.
    new Handle:list = CreateArray();
    PushArrayCell(list, 2);
    PushArrayCell(list, 5);
    PushArrayCell(list, 7);
    PushArrayCell(list, 8);
    PushArrayCell(list, 9);
    
    new listCount = GetArraySize(list);
    new resultCount = 3;
    new QueryLib_ResultMode:selectMode = QueryLib_First;    // Using a non-list mode.
    new errCode = 0;
    
    QueryLib_GetResultValue(list, listCount, resultCount, false, selectMode, errCode);
    
    // Verify that list handle is closed.
    return list == INVALID_HANDLE;
}

bool:TestResultValue_Handle2()
{
    // Create test list.
    new Handle:list = CreateArray();
    PushArrayCell(list, 2);
    PushArrayCell(list, 5);
    PushArrayCell(list, 7);
    PushArrayCell(list, 8);
    PushArrayCell(list, 9);
    
    new listCount = GetArraySize(list);
    new resultCount = 3;
    new QueryLib_ResultMode:selectMode = QueryLib_Last;    // Using a non-list mode.
    new errCode = 0;
    
    QueryLib_GetResultValue(list, listCount, resultCount, false, selectMode, errCode);
    
    // Verify that list handle is closed.
    return list == INVALID_HANDLE;
}

bool:TestResultValue_Handle3()
{
    // Create test list.
    new Handle:list = CreateArray();
    PushArrayCell(list, 2);
    PushArrayCell(list, 5);
    PushArrayCell(list, 7);
    PushArrayCell(list, 8);
    PushArrayCell(list, 9);
    
    new listCount = GetArraySize(list);
    new resultCount = 3;
    new QueryLib_ResultMode:selectMode = QueryLib_Random;    // Using a non-list mode.
    new errCode = 0;
    
    QueryLib_GetResultValue(list, listCount, resultCount, false, selectMode, errCode);
    
    // Verify that list handle is closed.
    return list == INVALID_HANDLE;
}

bool:TestResultValue_Handle4()
{
    // Create test list.
    new Handle:list = CreateArray();
    PushArrayCell(list, 2);
    PushArrayCell(list, 5);
    PushArrayCell(list, 7);
    PushArrayCell(list, 8);
    PushArrayCell(list, 9);
    
    new listCount = GetArraySize(list);
    new resultCount = 3;
    new QueryLib_ResultMode:selectMode = QueryLib_List;
    new errCode = 0;
    
    // Note: asList set to true so the handle won't be closed.
    QueryLib_GetResultValue(list, listCount, resultCount, true, selectMode, errCode);
    
    // Verify that list handle is NOT closed.
    return list != INVALID_HANDLE;
}

bool:TestResultValue_Handle5()
{
    // Create test list.
    new Handle:list = CreateArray();
    PushArrayCell(list, 2);
    PushArrayCell(list, 5);
    PushArrayCell(list, 7);
    PushArrayCell(list, 8);
    PushArrayCell(list, 9);
    
    new listCount = GetArraySize(list);
    new resultCount = 3;
    new QueryLib_ResultMode:selectMode = QueryLib_First;    // Using non-list mode.
    new errCode = 0;
    
    // Note: asList set to true so the handle won't be closed.
    QueryLib_GetResultValue(list, listCount, resultCount, true, selectMode, errCode);
    
    // Verify that list handle is NOT closed. Even if selectMode is a non-list
    // mode.
    return list != INVALID_HANDLE;
}

bool:TestResultValue_NoResult()
{
    // Create test list (doesn't need to have content).
    new Handle:list = CreateArray();
    
    new bool:result = true;
    
    new listCount = 0;
    new resultCount = 0;    // No result.
    new QueryLib_ResultMode:selectMode = QueryLib_First;
    new errCode = 0;
    
    new retval = QueryLib_GetResultValue(list, listCount, resultCount, false, selectMode, errCode);
    
    // Verify error code.
    if (errCode != QUERYLIB_ERR_NONE_FOUND)
    {
        result = false;
    }
    
    // Expect error code as return value (because select mode isn't a list).
    if (retval != QUERYLIB_ERR_NONE_FOUND)
    {
        result = false;
    }
    
    return result;
}

bool:TestResultValue_First()
{
    // Create test list.
    new Handle:list = CreateArray();
    PushArrayCell(list, 2);
    PushArrayCell(list, 5);
    PushArrayCell(list, 7);
    PushArrayCell(list, 8);
    PushArrayCell(list, 9);
    
    new listCount = GetArraySize(list);
    new resultCount = 3;
    new QueryLib_ResultMode:selectMode = QueryLib_First;
    new errCode = 0;
    
    new retval = QueryLib_GetResultValue(list, listCount, resultCount, false, selectMode, errCode);
    
    // Expect first element.
    return retval == 2;
}

bool:TestResultValue_Last()
{
    // Create test list.
    new Handle:list = CreateArray();
    PushArrayCell(list, 2);
    PushArrayCell(list, 5);
    PushArrayCell(list, 7);
    PushArrayCell(list, 8);
    PushArrayCell(list, 9);
    
    new listCount = GetArraySize(list);
    new resultCount = 3;
    new QueryLib_ResultMode:selectMode = QueryLib_Last;
    new errCode = 0;
    
    new retval = QueryLib_GetResultValue(list, listCount, resultCount, false, selectMode, errCode);
    
    // Expect last element.
    return retval == 9;
}

bool:TestResultValue_Random()
{
    // Create test list.
    new Handle:list = CreateArray();
    PushArrayCell(list, 2);
    PushArrayCell(list, 5);
    PushArrayCell(list, 7);
    PushArrayCell(list, 8);
    PushArrayCell(list, 9);
    
    new listCount = GetArraySize(list);
    new resultCount = 3;
    new QueryLib_ResultMode:selectMode = QueryLib_Random;
    new errCode = 0;
    
    new retval = QueryLib_GetResultValue(list, listCount, resultCount, false, selectMode, errCode);
    
    // Verify that one of the elements are selected.
    switch (retval)
    {
        case 2, 5, 7, 8, 9:
        {   
            return true;
        }
    }
    return false;
}

bool:TestResultValue_List()
{
    // Create test list.
    new Handle:list = CreateArray();
    PushArrayCell(list, 2);
    PushArrayCell(list, 5);
    PushArrayCell(list, 7);
    PushArrayCell(list, 8);
    PushArrayCell(list, 9);
    
    new listCount = GetArraySize(list);
    new resultCount = 3;
    new QueryLib_ResultMode:selectMode = QueryLib_List;
    new errCode = 0;

    // Note: asList is set to false to close the list handle (and because the
    //       result list is fake).
    new retval = QueryLib_GetResultValue(list, listCount, resultCount, false, selectMode, errCode);
    
    // Expect number of elements added to the list (resultCount).
    return retval == 3;
}

bool:TestResultValue_InvalidMode()
{
    // Create test list.
    new Handle:list = CreateArray();
    
    new listCount = 0;
    new resultCount = 3;
    new QueryLib_ResultMode:selectMode = QueryLib_Invalid;
    new errCode = 0;
    
    QueryLib_GetResultValue(list, listCount, resultCount, false, selectMode, errCode);
    
    // Expect invalid mode error.
    return errCode == QUERYLIB_ERR_INVALID_MODE;
}
