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
    PawnUnit_Run(ObjectTests);
    //PawnUnit_Run(ObjectDataTests);
}

InitTypeTests()
{
    TypeTests = PawnUnit_CreateTestCollection("Type descriptor tests");
    
    new TestCase:createTypeTest = PawnUnit_CreateTestCase("Create type");
    PawnUnit_AddTestPhase(createTypeTest, CreateTypeTest);
    
    new TestCase:cloneTypeTest = PawnUnit_CreateTestCase("Clone type");
    PawnUnit_AddTestPhase(:cloneTypeTest, CloneTypeTest);
    
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
    PawnUnit_AddTestPhase(createMutableObjectTest, CreateMmutableObjectTest);
    
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
    
    PawnUnit_AddTestCase(ObjectTests, createMutableObjectTest);
    PawnUnit_AddTestCase(ObjectTests, createImmutableObjectTest);
    PawnUnit_AddTestCase(ObjectTests, cloneImmutableToImmutableTest);
    PawnUnit_AddTestCase(ObjectTests, cloneImmutableToMutableTest);
    PawnUnit_AddTestCase(ObjectTests, cloneMutableToImmutableTest);
    PawnUnit_AddTestCase(ObjectTests, cloneMutableToMutableTest);
}

/******************
 *   Type tests   *
 ******************/

public TestControlAction:CreateTypeTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    AssertMsg(type != INVALID_OBJECT_TYPE, "failed to create object type");
    
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:CloneTypeTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    AssertMsg(type != INVALID_OBJECT_TYPE, "failed to create object type");
    
    new ObjectType:clonedType = ObjLib_CloneType();
    AssertMsg(type != INVALID_OBJECT_TYPE, "failed to clone type");
    
    ObjLib_DeleteType(type);
    ObjLib_DeleteType(clonedType);
    return Test_Continue;
}

public TestControlAction:AddKeyTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    ObjLib_AddKey(type, "newKey");
    AssertMsg(ObjLib_KeyExist(type, "newKey"), "key not added");
    
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:RemoveKeyTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    ObjLib_AddKey(type, "newKey");
    AssertMsg(ObjLib_KeyExist(type, "newKey"), "key not added");
    
    ObjLib_RemoveKey(type, "newKey");
    AssertMsg(!ObjLib_KeyExist(type, "newKey", "key not removed"));
    
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
    AssertMsg(object != INVALID_OBJECT, "failed to create object");
    
    // Mutable objects should clone their type and not refer the original type
    // directly.
    AssertMsg(ObjLib_GetObjectType(object) != type, "type not cloned");
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteType(type);
    return Test_Continue;
}

public TestControlAction:CreateImmutableObjectTest(TestCase:testCase)
{
    new ObjectType:type = ObjLib_CreateType();
    
    // Create immutable object.
    new Object:object = ObjLib_CreateObject(type, false);
    AssertMsg(object != INVALID_OBJECT, "failed to create object");
    
    // Immutable should use the type directly.
    AssertMsg(ObjLib_GetObjectType(object) == type, "type not same");
    
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
    AssertMsg(clonedObject != INVALID_OBJECT, "failed to clone object");
    
    // Immutable clone should use the same type.
    AssertMsg(ObjLib_GetObjectType(clonedObject) == type, "type not same");
    
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
    AssertMsg(clonedObject != INVALID_OBJECT, "failed to clone object");
    
    // Mutable clone should clone type.
    AssertMsg(ObjLib_GetObjectType(clonedObject) != type, "type not cloned");
    
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
    AssertMsg(clonedObject != INVALID_OBJECT, "failed to clone object");
    
    // Immutable clone from mutable object should clone type.
    AssertMsg(ObjLib_GetObjectType(clonedObject) != type, "type not cloned");
    
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
    AssertMsg(clonedObject != INVALID_OBJECT, "failed to clone object");
    
    // Mutable clone from mutable object should clone type.
    AssertMsg(ObjLib_GetObjectType(clonedObject) != type, "type not cloned");
    
    ObjLib_DeleteObject(object);
    ObjLib_DeleteObject(clonedObject);
    ObjLib_DeleteType(type);
    return Test_Continue;
}
