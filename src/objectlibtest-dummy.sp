#pragma semicolon 1
#include <sourcemod>
#include "libraries/objectlib"

public Dummy()
{
    // Call every function to check for compile errors. This code is never 
    // intended to be executed.
    new ObjectType:type = ObjLib_CreateType();
    ObjLib_DeleteType(type);
    ObjLib_CloneType(type);
    ObjLib_AddKey(type, "dummyKey");
    ObjLib_RemoveKey(type, "dummyKey");
    
    new Object:object = ObjLib_CreateObject(type);
    ObjLib_AddObjectKey(object, "dummyKey");
    ObjLib_RemoveObjectKey(object, "dummyKey");
    ObjLib_DeleteObject(object);
    ObjLib_CloneObject(object);
    ObjLib_GetAny(object, "dummyKey");
    ObjLib_SetAny(object, "dummyKey", 1);
}
