#pragma semicolon 1
#include <sourcemod>
#include <pawnunit>
#include "libraries/objectlib"

new TestCollection:TypeTests;
new TestCollection:ObjectTests;
//new TestCollection:ObjectDataTests;

public OnPluginStart()
{
    InitTypeTests();
    InitObjectTests();
    
    PawnUnit_Run(TypeTests);
    PawnUnit_PrintResults(TypeTests);
    
    PawnUnit_ResetStates();
    PawnUnit_ResetStatistics();
    PawnUnit_Run(ObjectTests);
    PawnUnit_PrintResults(ObjectTests);
    
    //PawnUnit_Run(ObjectDataTests);
}

InitTypeTests()
{
    TypeTests = PawnUnit_CreateTestCollection("Type descriptor tests");
    
    new TestCase:createTypeTest = PawnUnit_CreateTestCase("Create type");
    PawnUnit_AddTestPhase(createTypeTest, CreateTypeTest);
    
    new TestCase:cloneTypeTest = PawnUnit_CreateTestCase("Clone type");
    PawnUnit_AddTestPhase(cloneTypeTest, CloneTypeTest);
    
    new TestCase:addKeyTest = PawnUnit_CreateTestCase("Add key");
    PawnUnit_AddTestPhase(addKeyTest, AddKeyTest);
    
    new TestCase:removeKeyTest = PawnUnit_CreateTestCase("Remove key");
    PawnUnit_AddTestPhase(removeKeyTest, RemoveKeyTest);
    
    PawnUnit_AddTestCase(TypeTests, createTypeTest);
    PawnUnit_AddTestCase(TypeTests, cloneTypeTest);
    PawnUnit_AddTestCase(TypeTests, addKeyTest);
    PawnUnit_AddTestCase(TypeTests, removeKeyTest);
}

InitObjectTests()
{
    ObjectTests = PawnUnit_CreateTestCollection("Object tests");
    
    new TestCase:createMutableObjectTest = PawnUnit_CreateTestCase("Create mutable object");
    PawnUnit_AddTestPhase(createMutableObjectTest, CreateMutableObjectTest);
    
    new TestCase:createImmutableObjectTest = PawnUnit_CreateTestCase("Create mutable object");
    PawnUnit_AddTestPhase(createImmutableObjectTest, CreateImmutableObjectTest);
    
    new TestCase:cloneImmutableToImmutableTest = PawnUnit_CreateTestCase("Clone immutable to immutable object");
    PawnUnit_AddTestPhase(cloneImmutableToImmutableTest, CloneImmutableToImmutable);
    
    new TestCase:cloneImmutableToMutableTest = PawnUnit_CreateTestCase("Clone immutable to mutable object");
    PawnUnit_AddTestPhase(cloneImmutableToMutableTest, CloneImmutableToMutable);
    
    new TestCase:cloneMutableToImmutableTest = PawnUnit_CreateTestCase("Clone mutable to immutable object");
    PawnUnit_AddTestPhase(cloneMutableToImmutableTest, CloneMutableToImmutable);
    
    new TestCase:cloneMutableToMutableTest = PawnUnit_CreateTestCase("Clone mutable to mutable object");
    PawnUnit_AddTestPhase(cloneMutableToMutableTest, CloneMutableToMutable);
    
    // Data type tests. These are basically testing type and tag verification
    // since the internal data type is cell anyways.
    new TestCase:anyTypeTest = PawnUnit_CreateTestCase("\"Any\" data type");
    PawnUnit_AddTestPhase(anyTypeTest, AnyTypeTest);
    
    new TestCase:cellTypeTest = PawnUnit_CreateTestCase("Cell data type");
    PawnUnit_AddTestPhase(cellTypeTest, CellTypeTest);
    
    new TestCase:boolTypeTest = PawnUnit_CreateTestCase("Boolean data type");
    PawnUnit_AddTestPhase(boolTypeTest, BoolTypeTest);
    
    new TestCase:floatTypeTest = PawnUnit_CreateTestCase("Float data type");
    PawnUnit_AddTestPhase(floatTypeTest, FloatTypeTest);
    
    new TestCase:handleTypeTest = PawnUnit_CreateTestCase("Handle data type");
    PawnUnit_AddTestPhase(handleTypeTest, HandleTypeTest);
    
    new TestCase:functionTypeTest = PawnUnit_CreateTestCase("Function data type");
    PawnUnit_AddTestPhase(functionTypeTest, FunctionTypeTest);
    
    new TestCase:arrayTypeTest = PawnUnit_CreateTestCase("Array data type");
    PawnUnit_AddTestPhase(arrayTypeTest, ArrayTypeTest);
    
    new TestCase:stringTypeTest = PawnUnit_CreateTestCase("String data type");
    PawnUnit_AddTestPhase(stringTypeTest, StringTypeTest);
    
    new TestCase:objectTypeTest = PawnUnit_CreateTestCase("Object data type");
    PawnUnit_AddTestPhase(objectTypeTest, ObjectTypeTest);
    
    new TestCase:objectTypeTypeTest = PawnUnit_CreateTestCase("ObjectType data type");
    PawnUnit_AddTestPhase(objectTypeTypeTest, ObjectTypeTypeTest);
    
    PawnUnit_AddTestCase(ObjectTests, createMutableObjectTest);
    PawnUnit_AddTestCase(ObjectTests, createImmutableObjectTest);
    PawnUnit_AddTestCase(ObjectTests, cloneImmutableToImmutableTest);
    PawnUnit_AddTestCase(ObjectTests, cloneImmutableToMutableTest);
    PawnUnit_AddTestCase(ObjectTests, cloneMutableToImmutableTest);
    PawnUnit_AddTestCase(ObjectTests, cloneMutableToMutableTest);
    
    PawnUnit_AddTestCase(ObjectTests, anyTypeTest);
    PawnUnit_AddTestCase(ObjectTests, cellTypeTest);
    PawnUnit_AddTestCase(ObjectTests, boolTypeTest);
    PawnUnit_AddTestCase(ObjectTests, floatTypeTest);
    PawnUnit_AddTestCase(ObjectTests, handleTypeTest);
    PawnUnit_AddTestCase(ObjectTests, functionTypeTest);
    PawnUnit_AddTestCase(ObjectTests, arrayTypeTest);
    PawnUnit_AddTestCase(ObjectTests, stringTypeTest);
    PawnUnit_AddTestCase(ObjectTests, objectTypeTest);
    PawnUnit_AddTestCase(ObjectTests, objectTypeTypeTest);
}

/******************
 *   Type tests   *
 ******************/

public TestControlAction:CreateTypeTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    AssertMsg(type != INVALID_OBJECT_TYPE, "failed to create object type")
    
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:CloneTypeTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    AssertMsg(type != INVALID_OBJECT_TYPE, "failed to create object type")
    
    new ObjectType:clonedType = ObjLib_CloneType(type);
    AssertMsg(type != INVALID_OBJECT_TYPE, "failed to clone type")
    
    ObjLib_DeleteType(type);
    ObjLib_DeleteType(clonedType);
    return Test_Continue;
}

public TestControlAction:AddKeyTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    ObjLib_AddKey(type, "newKey");
    AssertMsg(ObjLib_KeyExist(type, "newKey"), "key not added")
    
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:RemoveKeyTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    ObjLib_AddKey(type, "newKey");
    AssertMsg(ObjLib_KeyExist(type, "newKey"), "key not added")
    
    ObjLib_RemoveKey(type, "newKey");
    AssertMsg(!ObjLib_KeyExist(type, "newKey"), "key not removed")
    
    ObjLib_DeleteType(type);
    return Test_Continue;
}


/********************
 *   Object tests   *
 ********************/

public TestControlAction:CreateMutableObjectTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    // Create mutable object (default).
    new Object:object = ObjLib_CreateObject(type);
    AssertMsg(object != INVALID_OBJECT, "failed to create object")
    
    // Mutable objects should clone their type and not refer the original type
    // directly.
    AssertMsg(ObjLib_GetTypeDescriptor(object) != type, "type not cloned")
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:CreateImmutableObjectTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    // Create immutable object.
    new Object:object = ObjLib_CreateObject(type, false);
    AssertMsg(object != INVALID_OBJECT, "failed to create object")
    
    // Immutable should use the type directly.
    AssertMsg(ObjLib_GetTypeDescriptor(object) == type, "type not same")
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:CloneImmutableToImmutable(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    // Create immutable object.
    new Object:object = ObjLib_CreateObject(type, false);
    
    // Clone as immutable.
    new Object:clonedObject = ObjLib_CloneObject(object, false);
    AssertMsg(clonedObject != INVALID_OBJECT, "failed to clone object")
    
    // Immutable clone should use the same type.
    AssertMsg(ObjLib_GetTypeDescriptor(clonedObject) == type, "type not same")
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:CloneImmutableToMutable(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    // Create immutable object.
    new Object:object = ObjLib_CreateObject(type, false);
    
    // Clone as mutable.
    new Object:clonedObject = ObjLib_CloneObject(object, true);
    AssertMsg(clonedObject != INVALID_OBJECT, "failed to clone object")
    
    // Mutable clone should clone type.
    AssertMsg(ObjLib_GetTypeDescriptor(clonedObject) != type, "type not cloned")
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteObject(clonedObject);
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:CloneMutableToImmutable(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    // Create mutable object.
    new Object:object = ObjLib_CreateObject(type, true);
    
    // Clone as immutable.
    new Object:clonedObject = ObjLib_CloneObject(object, false);
    AssertMsg(clonedObject != INVALID_OBJECT, "failed to clone object")
    
    // Immutable clone from mutable object should clone type.
    AssertMsg(ObjLib_GetTypeDescriptor(clonedObject) != type, "type not cloned")
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteObject(clonedObject);
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:CloneMutableToMutable(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    // Create mutable object.
    new Object:object = ObjLib_CreateObject(type, true);
    
    // Clone as mutable.
    new Object:clonedObject = ObjLib_CloneObject(object, true);
    AssertMsg(clonedObject != INVALID_OBJECT, "failed to clone object")
    
    // Mutable clone from mutable object should clone type.
    AssertMsg(ObjLib_GetTypeDescriptor(clonedObject) != type, "type not cloned")
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteObject(clonedObject);
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:AnyTypeTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    // Create mutable object with dummy key of "any" type.
    new Object:object = ObjLib_CreateObject(type, true);
    ObjLib_AddObjectKey(object, "dummyKey", ObjDataType_Any);
    
    new Float:dummy1 = 2.5;
    new dummy2 = 10;
    
    // Verify that it's possible to set and retrieve different data types on the
    // same key. Keys with other data types would throw a type mismatch error,
    // this should not happen when using "any" as data type.
    
    // Set and verify dummy 1.
    ObjLib_SetAny(object, "dummyKey", dummy1);
    AssertMsg(FloatEquals(dummy1, Float:ObjLib_GetAny(object, "dummyKey")), "dummy1 value not equal")
    
    // Set and verify dummy 2.
    ObjLib_SetAny(object, "dummyKey", dummy2);
    AssertMsg(dummy2 == ObjLib_GetAny(object, "dummyKey"), "dummy2 value not equal")
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:CellTypeTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    // Create mutable object with dummy key of cell type.
    new Object:object = ObjLib_CreateObject(type, true);
    ObjLib_AddObjectKey(object, "dummyKey", ObjDataType_Cell);
    
    // Set and verify a dummy value.
    new dummy = 10;
    ObjLib_SetCell(object, "dummyKey", dummy);
    Assert(CellEquals(dummy, ObjLib_GetCell(object, "dummyKey")));
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:BoolTypeTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    // Create mutable object with dummy key of boolean type.
    new Object:object = ObjLib_CreateObject(type, true);
    ObjLib_AddObjectKey(object, "dummyKey", ObjDataType_Bool);
    
    // Set and verify a dummy value.
    new bool:dummy = true;
    ObjLib_SetBool(object, "dummyKey", dummy);
    Assert(True(ObjLib_GetBool(object, "dummyKey")));
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:FloatTypeTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    // Create mutable object with dummy key of float type.
    new Object:object = ObjLib_CreateObject(type, true);
    ObjLib_AddObjectKey(object, "dummyKey", ObjDataType_Float);
    
    // Set and verify a dummy value.
    new Float:dummy = 2.5;
    ObjLib_SetFloat(object, "dummyKey", dummy);
    AssertMsg(FloatEquals(dummy, ObjLib_GetFloat(object, "dummyKey")), "not equal")
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:HandleTypeTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    // Create mutable object with dummy key of handle type.
    new Object:object = ObjLib_CreateObject(type, true);
    ObjLib_AddObjectKey(object, "dummyKey", ObjDataType_Handle);
    
    // Set and verify a dummy value.
    new Handle:dummy = Handle:10;
    ObjLib_SetHandle(object, "dummyKey", dummy);
    AssertMsg(dummy == ObjLib_GetHandle(object, "dummyKey"), "not equal")
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:FunctionTypeTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    // Create mutable object with dummy key of function reference type.
    new Object:object = ObjLib_CreateObject(type, true);
    ObjLib_AddObjectKey(object, "dummyKey", ObjDataType_Function);
    
    // Set and verify a dummy value.
    new Function:dummy = Function:10;
    ObjLib_SetFunction(object, "dummyKey", dummy);
    AssertMsg(dummy == ObjLib_GetFunction(object, "dummyKey"), "not equal")
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:ArrayTypeTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType(3);
    
    // Create mutable object with dummy key of array type.
    new Object:object = ObjLib_CreateObject(type, true);
    ObjLib_AddObjectKey(object, "dummyKey", ObjDataType_Array);
    
    // Set dummy values.
    new dummy[] = {1, 2, 3};
    ObjLib_SetArray(object, "dummyKey", dummy, sizeof(dummy));
    
    // Verify values.
    new result[3];
    ObjLib_GetArray(object, "dummyKey", result, sizeof(result));
    for (new i = 0; i < 3; i++)
    {
        AssertMsg(CellEquals(dummy[i], result[i]), "not equal at index %d", i)
    }
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:StringTypeTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType(16);
    
    // Create mutable object with dummy key of array type.
    new Object:object = ObjLib_CreateObject(type, true);
    ObjLib_AddObjectKey(object, "dummyKey", ObjDataType_String);
    
    // Set dummy string.
    new String:dummy[] = "A Dummy String";
    ObjLib_SetString(object, "dummyKey", dummy);
    
    // Verify string.
    new String:result[16];
    ObjLib_GetString(object, "dummyKey", result, sizeof(result));
    Assert(StringEquals(dummy, result));
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:ObjectTypeTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    // Create mutable object with dummy key of object reference type.
    new Object:object = ObjLib_CreateObject(type, true);
    ObjLib_AddObjectKey(object, "dummyKey", ObjDataType_Object);
    
    // Set and verify a dummy value.
    new Object:dummy = Object:10;
    ObjLib_SetObject(object, "dummyKey", dummy);
    AssertMsg(dummy == ObjLib_GetObject(object, "dummyKey"), "not equal")
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:ObjectTypeTypeTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    // Create mutable object with dummy key of type descriptor type.
    new Object:object = ObjLib_CreateObject(type, true);
    ObjLib_AddObjectKey(object, "dummyKey", ObjDataType_ObjectType);
    
    // Set and verify a dummy value.
    new ObjectType:dummy = ObjectType:10;
    ObjLib_SetObjectType(object, "dummyKey", dummy);
    AssertMsg(dummy == ObjLib_GetObjectType(object, "dummyKey"), "not equal")
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(type);
    return Test_Continue;
}


/**
 * Demonstrates usage of immutable objects.
 */
stock ImmutableObjectExample()
{
    // Note: This code is not tested yet.
    
    new String:buffer[64];
    new String:buffer2[64];
    
    // Declare a "person" type.
    new ObjectType:personType = ObjLib_CreateType();
    ObjLib_AddKey(personType, "name", ObjDataType_String);
    ObjLib_AddKey(personType, "skillPoints", ObjDataType_Cell);
    ObjLib_AddKey(personType, "bestFriend", ObjDataType_Object);
    
    // Create immutable person objects.
    new Object:alice = ObjLib_CreateObject(personType, false);
    new Object:bob = ObjLib_CreateObject(personType, false);
    
    // Initialize Alice. Note the use of data types. Trying to use
    // ObjLib_SetCell on a key with another data type will cause a run time
    // error. The get/set functions are also tagged so that the compiler can do
    // tag checking (trying to set a cell in a float would trigger a warning).
    ObjLib_SetString(alice, "name", "Alice");
    ObjLib_SetCell(alice, "skillPoints", 100);
    ObjLib_SetObject(alice, "bestFriend", bob);
    
    // Initialize Bob.
    ObjLib_SetString(bob, "name", "Bob");
    ObjLib_SetCell(bob, "skillPoints", 50);
    ObjLib_SetObject(bob, "bestFriend", alice);
    
    // Print Alice.
    PrintToServer("Alice");
    ObjLib_GetString(alice, "name", buffer, sizeof(buffer));
    PrintToServer("name: %s", buffer);
    PrintToServer("skillPoints: %d", ObjLib_GetCell(alice, "skillPoints"));
    ObjLib_GetString(ObjLib_GetObject(alice, "bestFriend"), "name", buffer, sizeof(buffer));    // Get name of best friend.
    PrintToServer("bestFriend: %s", buffer);
    
    // Demonstrate that immutable objects share type descriptor.
    // Note that this doesn't mean that ALL immutable objects use the exact same
    // type, but that they share the type they're based on when they were
    // created. This will save memory compared to mutable objects.
    new ObjectType:aliceType = ObjLib_GetTypeDescriptor(alice);
    new ObjectType:bobType = ObjLib_GetTypeDescriptor(bob);
    if (aliceType == bobType)
    {
        PrintToServer("Alice and bob are immutable objects, and use the same type descriptor.");
    }
    
    // Reflection. List data types for each key in personType.
    new Handle:keys = ObjLib_GetTypeKeys(personType);
    new Handle:dataTypes = ObjLib_GetTypeDataTypes(personType);
    new len = GetArraySize(keys);
    
    PrintToServer("Keys in personType");
    for (new i = 0; i < len; i ++)
    {
        // Get key name.
        GetArrayString(keys, i, buffer, sizeof(buffer));
        
        // Get data type string.
        ObjLib_DataTypeToString(ObjectDataType:GetArrayCell(dataTypes, i), buffer2, sizeof(buffer2));
        
        PrintToServer("%s: %s", buffer, buffer2);
    }
    
    // Delete objects and their type when no longer in use.
    ObjLib_DeleteObject(alice);
    ObjLib_DeleteObject(bob);
    ObjLib_DeleteType(personType);
}
