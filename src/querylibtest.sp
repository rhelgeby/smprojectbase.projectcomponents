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
}

public Action:Command_Info(client, argc)
{
    ReplyToCommand(client, "Commands: sm_querylib_run, sm_querylib_setaccess, sm_querylib_dump");
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
        if (errCode > 0)
        {
            decl String:errMsg[STRING_BUFFER_LEN];
            errMsg[0] = 0;
            decl String:errMarker[QUERYLIB_MAX_QUERY_LEN];
            errMarker[0] = 0;
            
            // Print error message.
            QueryLib_ErrCodeToString(errCode, errMsg, sizeof(errMsg));
            ReplyToCommand(client, "Parse error (error code = %d, position = %d): %s\n", errCode, errPos, errMsg);
            
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
    ReplyToCommand(client, "Element access set to %d.", value);
    
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
    }
    
    return Plugin_Handled;
}



PrintList(client, Handle:list = INVALID_HANDLE)
{
    static const String:fmt[] = "%-8d %-8s %-8d %-8d %-12d";
    static const String:fmtHeader[] = "%-8d %-8s %-8d %-8d %-12d\n-----------------------------------------------";
    
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
    if (StrEqual(name, "barbazqux"))
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
    TestData[ElementCount][Test_FlagB] = false;
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
    TestData[ElementCount][Test_FlagB] = true;
    PushArrayCell(TestList, ElementCount);
    SetTrieValue(NameIndex, "baz3", ElementCount);
    ElementCount++;
    
    //-----
    
    // 9
    strcopy(TestData[ElementCount][Test_Name], TEST_NAME_LEN, "qux");
    TestData[ElementCount][Test_FlagA] = false;
    TestData[ElementCount][Test_FlagB] = false;
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
    TestData[ElementCount][Test_FlagB] = true;
    PushArrayCell(TestList, ElementCount);
    SetTrieValue(NameIndex, "qux3", ElementCount);
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
