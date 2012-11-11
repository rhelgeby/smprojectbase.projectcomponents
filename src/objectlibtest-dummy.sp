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
    ObjLib_GetCell(object, "dummyKey");
    ObjLib_SetCell(object, "dummyKey", 1);
    
    new bool:dummyBool;
    dummyBool = ObjLib_GetBoolAt(object, 0);
    ObjLib_SetBoolAt(object, 0, dummyBool);

    new Float:dummyFloat;
    dummyFloat = ObjLib_GetFloatAt(object, 0);
    ObjLib_SetFloatAt(object, 0, dummyFloat);
    
    new Handle:dummyHandle;
    dummyHandle = ObjLib_GetHandleAt(object, 0);
    ObjLib_SetHandleAt(object, 0, dummyHandle);
    
    new Function:dummyFunction;
    dummyFunction = ObjLib_GetFunctionAt(object, 0);
    ObjLib_SetFunctionAt(object, 0, dummyFunction);

    new dummyArray[2];
    ObjLib_GetArrayAt(object, 0, dummyArray, sizeof(dummyArray));
    ObjLib_SetArrayAt(object, 0, dummyArray, sizeof(dummyArray));

    new String:dummyString[16];
    ObjLib_GetStringAt(object, 0, dummyString, sizeof(dummyString));
    ObjLib_SetStringAt(object, 0, dummyString);

    new Object:dummyObject;
    dummyObject = ObjLib_GetObjectAt(object, 0);
    ObjLib_SetObjectAt(object, 0, dummyObject);
    
    new ObjectType:dummyObjectType;
    dummyObjectType = ObjLib_GetObjectTypeAt(object, 0);
    ObjLib_SetObjectTypeAt(object, 0, dummyObjectType);
}
