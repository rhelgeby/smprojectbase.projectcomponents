/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           kvparser.inc
 *  Type:           Library
 *  Description:    Valve KeyValue format parser for objectlib.
 *
 *  Copyright (C) 2012  Richard Helgeby, Greyscale
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

#include "libraries/utilities"
#include "libraries/arrays"

/*____________________________________________________________________________*/

/**
 * Length of string buffers used in parser. Increase this if you need to read
 * longer string values.
 */
#define OBJLIB_KV_MAX_STRING_LEN    255

/*____________________________________________________________________________*/

/**
 * Length of key name buffers used in parser.
 *
 * Note: Don't reserve too much space. The parser is recursive and very large
 *       key name buffers will consume a lot of stack space if the keyvalue tree
 *       is deep.
 */
#define OBJLIB_KV_MAX_KEY_LEN    64

/*____________________________________________________________________________*/

/**
 * Empty object type template with enough space reserved for strings.
 */
new ObjectType:ObjLib_KvObjectType = INVALID_OBJECT_TYPE;
new bool:ObjLib_KvObjectTypeBuilt = false;

/*____________________________________________________________________________*/

/**
 * Object type template for parser context object.
 */
new ObjectType:ObjLib_KvContextType = INVALID_OBJECT_TYPE;
new bool:ObjLib_KvContextTypeBuilt = false;

/*____________________________________________________________________________*/

/**
 * Parser states. The parser will check the state in parseParams on every
 * iteration.
 */
enum ObjLibParseState
{
    ObjLibState_Stopped,    /** Stop parsing. */
    ObjLibState_Running,    /** Continue parsing. */
    ObjLibState_Aborted     /** Abort parser due to error. */
}

/*____________________________________________________________________________*/

/**
 * Temporary global storage for simple error handler. Used to wrap error handler
 * in boolean word parser (utilities.inc).
 */
//new ObjLib_ErrorHandler:TempErrorHandler = INVALID_FUNCTION;
//new ObjectType:TempType = INVALID_OBJECT_TYPE;

/*____________________________________________________________________________*/

/**
 * Creates a parse context object. This object store the state and various
 * settings required by the parser.
 *
 * @param name              (Optional) Parser name. Useful to identify parser in
 *                          error logs or error handlers.
 * @param rootType          (Optional) Initial type to use when parsing the
 *                          first section. Default is no type; build type while
 *                          parsing.
 * @param addEmptySections  (Optional) Whether to add sections with no keys.
 *                          An empty object will represent an empty section.
 *                          Default is false, not adding.
 * @param booleanType       (Optional) Which boolean words to allow when
 *                          converting to boolean values. Default is
 *                          BoolType_Any (utilities.inc).
 * @param continueOnError   (Optional) Continue parsing remaining keys/sections
 *                          if there's an error parsing an element. The element
 *                          that failed will be skipped. Default is true.
 * @param errorHandler      (Optional) Custom error handler. Overrides any other
 *                          error handler if specified. Default is none, use
 *                          built in error handler.
 *
 * @return                  Parse context object. Must be deleted with
 *                          ObjLib_DeleteParseContext when no longer in use.
 */
stock Object:ObjLib_GetParseContext(const String:name[] = "",
                                    ObjectType:rootType = INVALID_OBJECT_TYPE,
                                    bool:ignoreEmptySections = false,
                                    Util_BoolStringType:booleanType = BoolType_Any,
                                    bool:continueOnError = true,
                                    ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Make sture type is ready.
    ObjLib_BuildKvContextType();
    
    new Object:parseContext = ObjLib_CreateObject(ObjLib_KvContextType);
    
    // Set parameters.
    ObjLib_SetObjectType(parseContext, "rootType", rootType);
    ObjLib_SetBool(parseContext, "ignoreEmptySections", ignoreEmptySections);
    ObjLib_SetCell(parseContext, "booleanType", booleanType);
    ObjLib_SetBool(parseContext, "continueOnError", continueOnError);
    ObjLib_SetFunction(parseContext, "errorHandler", errorHandler);
    ObjLib_SetCell(parseContext, "parseState", ObjLibState_Running);
    if (strlen(name) > 0)
    {
        ObjLib_SetString(parseContext, "name", name);
    }
    
    // Create stacks.
    new Handle:objectStack = CreateArray();
    new Handle:typeStack = CreateArray();
    new Handle:nameStack = CreateArray(ByteCountToCells(PLATFORM_MAX_PATH));
    
    // Set stacks.
    ObjLib_SetHandle(parseContext, "objectStack", objectStack);
    ObjLib_SetHandle(parseContext, "typeStack", typeStack);
    ObjLib_SetHandle(parseContext, "nameStack", nameStack);
    
    return parseContext;
}

/*____________________________________________________________________________*/

stock ObjLib_DeleteParseContext(Object:parseContext)
{
    // Get and delete stack arrays.
    new Handle:objectStack = ObjLib_GetHandle(parseContext, "objectStack");
    new Handle:typeStack = ObjLib_GetHandle(parseContext, "typeStack");
    new Handle:nameStack = ObjLib_GetHandle(parseContext, "nameStack");
    
    CloseHandle(objectStack);
    CloseHandle(typeStack);
    CloseHandle(nameStack);
}

/*____________________________________________________________________________*/

/**
 * Parse a KeyValue file in object mode. The entire root section is parsed into
 * an object.
 *
 * @param typeDescriptor    (Optional) Type of object. If not specified it will
 *                          create keys in mutable objects.
 * @param sectionKeyName    Name of key where section name is stored. Must exist
 *                          in type descriptor if a type is used. Otherwise it's
 *                          created automatically.
 *
 * @return                  Object with data from the root section.
 */
stock Object:ObjLib_ParseInObjectMode(ObjectType:typeDescriptor = INVALID_OBJECT_TYPE, const String:sectionKeyName[])
{
    return INVALID_OBJECT;
}

/*____________________________________________________________________________*/

/**
 * Parse a KeyValue tree in list mode. This parser assumes that every key in the
 * root is a sub section. Each section is parsed into an object and stored in a
 * list.
 *
 * @param kv                KeyValue tree to parse.
 * @param parseContext      Parse context object. Use ObjLib_GetParseParams to
 *                          create one.
 *
 * @return                  Handle to list of objects. May be empty if tree is
 *                          empty.
 */
stock Handle:ObjLib_ParseInListMode(Handle:kv, Object:parseContext)
{
    decl String:keyName[OBJLIB_KV_MAX_KEY_LEN];
    keyName[0] = 0;
    
    // Create list to store section objects.
    new Handle:sections = CreateArray();
    
    // Go to first sub section in section.
    if (!KvGotoFirstSubKey(kv))
    {
        // No sections. Return list as it is (empty).
        return sections;
    }
    
    // Loop through sections.
    PrintToServer("===== List mode: start =====");
    do
    {
        PrintToServer("----- List mode: iteration start -----");
        
        // Get section name before entering section.
        KvGetSectionName(kv, keyName, sizeof(keyName));
        
        // Enter section.
        KvGotoFirstSubKey(kv, false);
        
        // Verify that the cursor is on a key and not an empty section.
        if (KvGetDataType(kv, NULL_STRING) != KvData_None)
        {
            PrintToServer("Parsing section \"%s\"", keyName);
            
            new Object:subObject = ObjLib_ParseSection(kv, parseContext);
            
            // Add object to result list.
            PushArrayCell(sections, subObject);
            
            KvGoBack(kv);
        }
        else
        {
            // TODO: Empty section. Check if they should be added.
            if (!ObjLib_GetBool(parseContext, "ignoreEmptySections"))
            {
                // Add empty section.
                // TODO: How to avoid duplicated code?
            }
        }
    } while (KvGotoNextKey(kv));    // Go to next section.
    PrintToServer("===== List mode end =====");
    
    return sections;
}

/*____________________________________________________________________________*/

/**
 * Parse the current section into an object. It will do full recursive traversal
 * from the current section.
 *
 * Note: This function expects the KeyValue cursor to be positioned on the first
 *       sub key in the section.
 *
 * @param kv                Handle to keyvalue tree with the cursor on a
 *                          section.
 * @param parseContext      Parse context object. Use ObjLib_GetParseParams to
 *                          create one.
 *
 * @return                  Object with data from the section, or INVALID_OBJECT
 *                          if the section object was stored in the parent
 *                          object on the object stack.
 */
stock Object:ObjLib_ParseSection(Handle:kv, Object:parseContext)
{
    // Note: This function do recursive calls to itself.
    
    decl String:keyName[OBJLIB_KV_MAX_KEY_LEN];
    decl String:sectionName[OBJLIB_KV_MAX_KEY_LEN];
    keyName[0] = 0;
    sectionName[0] = 0;
    
    // Get current section name. At this point the cursor is pointing to the
    // first sub key, not the section itself. Save the position (keyId) of this
    // key and go back to get the section name.
    new keyId;
    KvGetSectionSymbol(kv, keyId);
    KvGoBack(kv);
    KvGetSectionName(kv, sectionName, sizeof(sectionName));
    KvJumpToKeySymbol(kv, keyId);   // Jump back to the first sub key again.
    
    // Prepare new section object.
    ObjLib_OnSectionStart(kv, sectionName, parseContext);
    
    // Get current section object from the object stack (keep it on the stack).
    new Object:object = INVALID_OBJECT;
    Array_PeekCell(ObjLib_GetHandle(parseContext, "objectStack"), object);
    
    // Validate object.
    if (object == INVALID_OBJECT)
    {
        ThrowError("[BUG] No object on object stack. This is a bug in objectlib.");
    }
    PrintToServer("-- Parse Section Start: %s --", sectionName);
    
    // Loop through keys and sections in the current kv section.
    do
    {
        PrintToServer("Iteration start.");
        
        // Read parser state to see if the parser should stop.
        new ObjLibParseState:parseState = ObjLibParseState:ObjLib_GetCell(parseContext, "parseState");
        if (parseState != ObjLibState_Running)
        {
            // Parser was stopped or an error occoured. Exit loop and end
            // this section.
            break;
        }
        
        // Get current key or section name.
        KvGetSectionName(kv, keyName, sizeof(keyName));
        PrintToServer("Current key: %s", keyName);
        
        // Check if key is a sub section.
        if (KvGotoFirstSubKey(kv, false))
        {
            // Key is a sub section.
            PrintToServer("Found sub section. Recursive entry.");
            
            // Parse sub section recurively. In case there were errors or the
            // parsing is aborted, this will be caught in the next iteration of
            // this do-loop (where it's reading parseState).
            ObjLib_ParseSection(kv, parseContext);
            
            // Done with sub section. Go out.
            KvGoBack(kv);
            PrintToServer("Going back.");
        }
        else
        {
            // Key is a value, or section is empty. Figure out which one.
            
            // Sections don't have a data type. If there is a data type, it's a
            // key.
            new KvDataTypes:dataType = KvGetDataType(kv, NULL_STRING);
            if (dataType != KvData_None)
            {
                // Get key name.
                KvGetSectionName(kv, keyName, sizeof(keyName));
                PrintToServer("Found key: %s", keyName);
                
                // Execute new key handler.
                ObjLib_OnKeyValue(kv, keyName, dataType, parseContext);
            }
            else
            {
                // Found an empty section.
                PrintToServer("Found empty section.");
                
                // Check if empty sections are included.
                if (!ObjLib_GetBool(parseContext, "ignoreEmptySections"))
                {
                    // Add empty section object. The empty object will use a
                    // type if available in constraints.
                    ObjLib_OnSectionStart(kv, keyName, parseContext);
                    ObjLib_OnSectionEnd(kv, keyName, parseContext);
                }
            }
        }
    } while (KvGotoNextKey(kv, false));     // Go to next key or section.
    
    PrintToServer("-- Parse section End: %s --", sectionName);
    
    // End the section object. If an object is returned there is no parent to
    // store it in, or an error occoured. The parseState parameter in
    // parseContext will tell which one.
    return ObjLib_OnSectionEnd(kv, sectionName, parseContext);
}

/*____________________________________________________________________________*/

/**
 * Handles a new section. Creates a new object to store section sub keys and
 * does validation if a type descriptor is available.
 *
 * @param kv                Handle to keyvalue tree with the cursor on a
 *                          section.
 * @param sectionName       Name of the current section.
 * @param parseContext      Parse context object. (Type: ObjLib_KvContextType)
 */
stock ObjLib_OnSectionStart(Handle:kv, const String:sectionName[], Object:parseContext)
{
    // Get type for this section.
    new ObjectType:typeDescriptor = ObjLib_GetSectionType(sectionName, parseContext);
    
    // Get stacks.
    new Handle:typeStack = ObjLib_GetHandle(parseContext, "typeStack");
    new Handle:objectStack = ObjLib_GetHandle(parseContext, "objectStack");
    new Handle:nameStack = ObjLib_GetHandle(parseContext, "nameStack");
    
    new bool:mutableObject = false;
    if (typeDescriptor == ObjLib_KvObjectType)
    {
        // Type template used. Make a mutable object so the parser will be
        // able to create keys.
        mutableObject = true;
    }
    
    // Create object for this section.
    new Object:object = ObjLib_CreateObject(typeDescriptor, mutableObject);
    
    // Push to cursor stacks so they become the current type and object.
    PushArrayCell(typeStack, typeDescriptor);
    PushArrayCell(objectStack, object);
    PushArrayString(nameStack, sectionName);
}

/*____________________________________________________________________________*/

/**
 * Cleanup handler when the parser reached the end of a section.
 *
 * Current section object is on top of the object stack. This handler will
 * store it in the parent object, or return it if there's no parent.
 *
 * @param kv                Handle to keyvalue tree.
 * @param sectionName       Name of the ending section.
 * @param parseContext      Parse context object. (Type: ObjLib_KvParamsType)
 *
 * @return                  Parsed section object if stack is empty (there is
 *                          no parent that can store it) - or on errors.
 *                          Otheriwse INVALID_OBJECT (the object was stored
 *                          in the parent object).
 */
stock Object:ObjLib_OnSectionEnd(Handle:kv, const String:sectionName[], Object:parseContext)
{
    // Get stacks.
    new Handle:typeStack = ObjLib_GetHandle(parseContext, "typeStack");
    new Handle:objectStack = ObjLib_GetHandle(parseContext, "objectStack");
    new Handle:nameStack = ObjLib_GetHandle(parseContext, "nameStack");
    
    // Get error handler.
    new ObjLib_ErrorHandler:errorHandler = ObjLib_GetFunction(parseContext, "errorHandler");
    if (errorHandler == INVALID_FUNCTION)
    {
        // No error handler specified. Replace with fallback error
        // handler.
        errorHandler = ObjLib_KvErrorHandler;
    }
    
    // Pop current section from name stack.
    Array_Pop(nameStack);
    
    // Get and pop section object from the object stack.
    new Object:object = INVALID_OBJECT;
    Array_PopCell(objectStack, object);
    
    // Validate object.
    if (object == INVALID_OBJECT)
    {
        ThrowError("[BUG] No object on the stack. This is a bug in objectlib.");
    }
    
    // Get and pop object type.
    new ObjectType:typeDescriptor = INVALID_OBJECT_TYPE;
    Array_PopCell(typeStack, typeDescriptor);
    
    // Validate type.
    if (typeDescriptor == INVALID_OBJECT_TYPE)
    {
        ThrowError("[BUG] No type descriptor on the stack. This is a bug in objectlib.");
    }
    
    // Get parent object (don't remove from stack).
    new Object:parent = INVALID_OBJECT;
    Array_PeekCell(objectStack, parent);
    
    // Validate parent.
    if (parent == INVALID_OBJECT)
    {
        // There is no parent object, the object stack is empty. Return the
        // object as it is.
        return object;
    }
    
    // Get parent type.
    new ObjectType:parentType = ObjLib_GetTypeDescriptor(parent);
    
    // Add section object to parent. Validate constraints first.
    
    // Get index of sub section key in parent object. This key may not exist if
    // the parent object type is not predefined in rootType.
    new keyIndex = ObjLib_GetKeyIndex(parent, sectionName);
    
    // Get key constraints, if any.
    new Object:constraints = INVALID_OBJECT;
    if (keyIndex >= 0)
    {
        // Get the constraints from the parent type. This might be
        // INVALID_OBJECT too.
        constraints = ObjLib_GetTypeConstraintsAt(parentType, keyIndex);
    }
    
    // Check if section has constraints.
    if (constraints != INVALID_OBJECT)
    {
        // Constraints available. Check type of constraints.
        new ObjectType:constraintType = ObjLib_GetTypeDescriptor(constraints);
        if (constraintType != ObjLib_ObjectConstraints &&
            constraintType != ObjLib_ObjectTypeConstraints)
        {
            // This key is not supposed to be a sub section. Trigger validation
            // error.
            
            // Call error handler.
            new Action:result = Plugin_Handled;
            if (ObjLib_HandleError(parentType,                  // typeDescriptor
                                   object,                      // object
                                   ObjLibError_ValidationError, // errorType
                                   errorHandler,                // customHandler
                                   result,                      // result
                                   parseContext,                // data
                                   "Expected \"%s\" to be a key value pair, but it's a sub section (object). Type constraints does not allow this.",   // format
                                   sectionName))
            {
                // Error was handled in customHandler. Update parser state.
                ObjLib_SetCell(parseContext, "parseState", result);
                
                // Return to parse loop. The parse state will tell if parser
                // should abort or continue.
                return object;
            }
            else
            {
                // Error not handled. This code is actually not reachable
                // because the error handler doesn't return when calling
                // ThrowError. Return something to satisfy compiler.
                return INVALID_OBJECT;
            }
        }
    }
    else
    {
        // No constraints.
        
        // Check if key in parent exists.
        if (keyIndex < 0)
        {
            // Create object key for storing this section.
            ObjLib_AddObjectKey(parent, sectionName, ObjDataType_Object);
        }
    }
    
    // Add object to parent.
    ObjLib_SetObject(parent, sectionName, object, errorHandler);
    
    return INVALID_OBJECT;
}

/*____________________________________________________________________________*/

/**
 * Reads a keyvalue pair.
 *
 * @param kv                Handle to keyvalue tree.
 * @param keyName           Name of the key.
 * @param dataType          Data type in KeyValue tree.
 * @param parseContext      Object with parser settings.
 *                          (Type: ObjLib_KvParamsType)
 */
stock ObjLib_OnKeyValue(Handle:kv, const String:keyName[], KvDataTypes:kvDataType, Object:parseContext)
{
    // Get stacks.
    new Handle:objectStack = ObjLib_GetHandle(parseContext, "objectStack");
    
    // Get object.
    new Object:object = INVALID_OBJECT;
    Array_PeekCell(objectStack, object);
    if (object == INVALID_OBJECT)
    {
        // Bug. An object should already be prepared and pushed onto the stack
        // when the section started.
        ThrowError("[BUG] No object. This is a bug in objectlib.");
    }
    
    // Get error handler.
    new ObjLib_ErrorHandler:errorHandler = ObjLib_GetFunction(parseContext, "errorHandler");
    if (errorHandler == INVALID_FUNCTION)
    {
        // No error handler specified. Replace with fallback error
        // handler.
        errorHandler = ObjLib_KvErrorHandler;
    }
    
    new ObjectDataType:keyDataType = ObjDataType_String;
    
    // Check if object is using the type template (object is mutable).
    if (ObjLib_IsMutable(object))
    {
        // Object has no predefined type descriptor. Add string key with no
        // constraints.
        ObjLib_AddObjectKey(object, keyName, ObjDataType_String);
        keyDataType = ObjDataType_String;
    }
    else
    {
        // Object has a predefined type descriptor. Get key data type.
        
        new ObjectType:typeDescriptor = ObjLib_KvGetCurrentType(parseContext);
        new Handle:keyDataTypes = ObjLib_GetTypeDataTypes(typeDescriptor);
        new keyIndex = ObjLib_GetKeyIndexFromType(typeDescriptor, keyName);
        
        // Validate key.
        if (keyIndex < 0)
        {
            // Unexpected key. Throw validation error.
            new Action:result = Plugin_Handled;
            if (ObjLib_HandleError(typeDescriptor,                      // typeDescriptor
                                   object,                              // object
                                   ObjLibError_InvalidKey,              // errorType
                                   errorHandler,                        // customHandler
                                   result,                              // result
                                   parseContext,                        // data
                                   "Unexpected key. The key \"%s\" does not exist in the type descriptor.",   // format
                                   keyName))
            {
                // Error was handled in customHandler. Update parser state.
                ObjLib_SetCell(parseContext, "parseState", result);
                
                // Return to parse loop. The parse state will tell if parser
                // should abort or continue.
                return;
            }
        }
        
        // Get constraint type.
        new Object:constraints = ObjLib_GetTypeConstraintsAt(typeDescriptor, keyIndex);
        new ObjectType:constraintsType = INVALID_OBJECT_TYPE;
        if (constraints != INVALID_OBJECT)
        {
            constraintsType = ObjLib_GetTypeDescriptor(constraints);
        }
        
        // Check if this is a lookup case. Override type to string if using
        // lookup constraints.
        if (constraintsType == ObjLib_LookupConstraints)
        {
            // This will trigger a lookup case when using lookup constraints and
            // a SetString function.
            keyDataType = ObjDataType_String;
        }
        else
        {
            // Not a lookup case. Get data type of key.
            keyDataType = ObjectDataType:GetArrayCell(keyDataTypes, keyIndex);
        }
    }
    
    // Get and store value. Objectlib will handle type mismatch or constraint
    // violations.
    ObjLib_StoreKvValue(kv, object, keyName, keyDataType, parseContext);
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Reads a value from a key and store it in the object.
 *
 * @param kv            Handle to keyvalue tree.
 * @param object        Object to store value in.
 * @param keyName       Key to read.
 * @param dataType      Key type.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 * @param parseContect  Object with parser settings.
 *                      (Type: ObjLib_KvParamsType)
 */
stock ObjLib_StoreKvValue(Handle:kv, Object:object, const String:keyName[], ObjectDataType:dataType, Object:parseContext)
{
    // Note: Using NULL_STRING as key name in KeyValue tree so that it will read
    //       the value at the current cursor position (which is prepared by the
    //       parser).
    
    // Get error handler.
    new ObjLib_ErrorHandler:errorHandler = ObjLib_GetFunction(parseContext, "errorHandler");
    if (errorHandler == INVALID_FUNCTION)
    {
        // No error handler specified. Replace with fallback error
        // handler.
        errorHandler = ObjLib_KvErrorHandler;
    }
    
    // Read value from KeyValue tree according to the expected data type.
    // Objectlib will validate type and constraints.
    switch (dataType)
    {
        case ObjDataType_Any:
        {
            new value = KvGetNum(kv, NULL_STRING);
            ObjLib_SetAny(object, keyName, value, errorHandler);
        }
        case ObjDataType_Cell:
        {
            new value = KvGetNum(kv, NULL_STRING);
            ObjLib_SetCell(object, keyName, value, errorHandler);
        }
        case ObjDataType_Bool:
        {
            // Note: This does direct conversion, from string to integer. The
            //       user may use a boolean lookup constraint for converting
            //       boolean words and call SetString instead.
            new bool:value = bool:KvGetNum(kv, NULL_STRING);
            ObjLib_SetBool(object, keyName, value, errorHandler);
        }
        case ObjDataType_Float:
        {
            new Float:value = KvGetFloat(kv, NULL_STRING);
            ObjLib_SetFloat(object, keyName, value, errorHandler);
        }
        case ObjDataType_Handle:
        {
            // Not supported.
            ThrowError("Handle type is not supported by the objectlib KeyValue parser.");
        }
        case ObjDataType_Function:
        {
            // Not supported.
            ThrowError("Function type is not supported by the objectlib KeyValue parser.");
        }
        case ObjDataType_Array:
        {
            // Not supported.
            ThrowError("Array type is not supported by the objectlib KeyValue parser.");
        }
        case ObjDataType_String:
        {
            decl String:value[OBJLIB_KV_MAX_STRING_LEN];
            value[0] = 0;
            KvGetString(kv, NULL_STRING, value, sizeof(value));
            ObjLib_SetString(object, keyName, value, errorHandler);
            PrintToServer("Reading value of %s: \"%s\"", keyName, value);
        }
        case ObjDataType_Object:
        {
            // This shouldn't happen. Objects should be read as kv sections or
            // handled as named lookups when available.
            ThrowError("[BUG] Can't read key as an object. The parser should check if this key is a sub section, if not it's a bug in objectlib.");
        }
        case ObjDataType_ObjectType:
        {
            // Not supported.
            ThrowError("ObjectType type is not supported by the objectlib KeyValue parser.");
        }
        default:
        {
            ThrowError("[BUG] Unexpected data type. This is a bug in objectlib.");
        }
    }
}

/*____________________________________________________________________________*/

/**
 * Fallback error handler in case no error handler is provided to parser.
 *
 * Does general cleanup to prevent memory leaks.
 *
 * @param typeDescriptor    Related type descriptor.
 * @param errorType         Type of error.
 * @param message           Error message.
 * @param object            Related object, if available.
 * @param parseContext      Object with parser settings.
 *                          (Type: ObjLib_KvParamsType)
 *
 * @return                  What to do next:
 *                          Plugin_Handled - Stop parsing.
 *                          Plugin_Continue - Skip current element, continue
 *                          parsing remaining keys.
 */
public Action:ObjLib_KvErrorHandler(ObjectType:typeDescriptor, ObjLibError:errorType, const String:message[], Object:object, Object:parseContext)
{
    new bool:continueOnError = ObjLib_GetBool(parseContext, "continueOnError");
    
    // Get path to current location in keyvalue tree.
    decl String:path[PLATFORM_MAX_PATH];
    ObjLib_KvBuildPath(parseContext, path, sizeof(path));
    
    // Get parser name if available.
    decl String:name[OBJLIB_KV_MAX_KEY_LEN];
    name[0] = 0;
    if (!ObjLib_IsKeyNull(parseContext, "name"))
    {
        ObjLib_GetString(parseContext, "name", name, sizeof(name));
    }
    else
    {
        strcopy(name, sizeof(name), "Unnamed parser");
    }
    
    // Log general parse error.
    LogError("[%s] Error parsing element in %s: %s", name, path, message);
    
    // Check if parser should continue.
    if (continueOnError)
    {
        return Plugin_Continue;
    }
    else
    {
        return Plugin_Handled;
    }
}


/*____________________________________________________________________________*/

/*public ObjLib_BoolParseErrorHandler()
{
    ObjLib_HandleError(ObjectType:typeDescriptor,   // typeDescriptor
                       INVALID_OBJECT,              // object
                       ObjLibError_ValidationError, // errorType
                       TempErrorHandler,            // customHandler
                       "Invalid boolean string.");  // format
}*/

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_BuildKvObjectType()
{
    if (!ObjLib_KvObjectTypeBuilt)
    {
        ObjLib_KvObjectType = ObjLib_CreateType(ByteCountToCells(OBJLIB_KV_MAX_STRING_LEN));
        ObjLib_KvObjectTypeBuilt = true;
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Builds the global context object type used to store parser settings.
 */
stock ObjLib_BuildKvContextType()
{
    if (!ObjLib_KvContextTypeBuilt)
    {
        // Create parse context type. Reserve some space for strings.
        ObjLib_KvContextType = ObjLib_CreateType(ByteCountToCells(OBJLIB_KV_MAX_KEY_LEN));
        
        // Key for storing a reference to the root type descriptor (the root in
        // this case is the section where the parser started from).
        ObjLib_AddKey(ObjLib_KvContextType, "rootType", ObjDataType_ObjectType, INVALID_OBJECT);
        
        // Flag for specifying whether to ignore empty sections.
        ObjLib_AddKey(ObjLib_KvContextType, "ignoreEmptySections", ObjDataType_Bool, INVALID_OBJECT);
        
        // Which types of boolean words allowed when converting a string to a
        // boolean value. Stored as Util_BoolStringType.
        ObjLib_AddKey(ObjLib_KvContextType, "booleanType", ObjDataType_Cell, INVALID_OBJECT);
        
        // Continue parsing remaining keys if there's an error parsing a key.
        ObjLib_AddKey(ObjLib_KvContextType, "continueOnError", ObjDataType_Bool, INVALID_OBJECT);
        
        // Optional user error handler callback to use on parser errors
        // (validation, unexpected keys, etc). This must be a
        // ObjLib_ErrorHandler function.
        ObjLib_AddKey(ObjLib_KvContextType, "errorHandler", ObjDataType_Function, INVALID_OBJECT);
        
        // Parser name (optional). Useful to see which parser that failed in
        // error logs.
        ObjLib_AddKey(ObjLib_KvContextType, "name", ObjDataType_String, INVALID_OBJECT);
        
        // State value read by parser on every iteration. Used to stop parser
        // and handle errors.
        ObjLib_AddKey(ObjLib_KvContextType, "parseState", ObjDataType_Cell, INVALID_OBJECT);
        
        // Object cursor stack. The top object is the current object being
        // written. ADT array used as a stack.
        ObjLib_AddKey(ObjLib_KvContextType, "objectStack", ObjDataType_Handle, INVALID_OBJECT);
        
        // Object type cursor stack. ADT array used as a stack.
        ObjLib_AddKey(ObjLib_KvContextType, "typeStack", ObjDataType_Handle, INVALID_OBJECT);
        
        // Section name stack used to build path to current location. ADT array
        // used as a stack.
        ObjLib_AddKey(ObjLib_KvContextType, "nameStack", ObjDataType_Handle, INVALID_OBJECT);
        
        ObjLib_KvContextTypeBuilt = true;
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Gets the type of the current section, or the root type if type stack is
 * empty.
 *
 * @param parseContext      Object with parser settings.
 *                          (Type: ObjLib_KvParamsType)
 *
 * @return                  Root type or type at the current level. Root type
 *                          may be INVALID_OBJECT, which means the parser isn't
 *                          validating against a type descriptor.
 */
stock ObjectType:ObjLib_KvGetCurrentType(Object:parseContext)
{
    new ObjectType:typeDescriptor = INVALID_OBJECT_TYPE;
    
    // Get and check if a type is on the type stack.
    if (Array_PeekCell(ObjLib_GetHandle(parseContext, "typeStack"), typeDescriptor))
    {
        // Stack not empty, use this type.
        return typeDescriptor;
    }
    
    // Stack empty, use root type.
    return ObjLib_GetObjectType(parseContext, "rootType");
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Gets the type of the specified sub section (relative to the current section).
 *
 * If a key with object data type has no constraints, the type template will be
 * used.
 *
 * Note: If a type template is returned, only use it to create mutable objects
 *       from it.
 *
 * @param sectionName       Name of the sub section to get type of.
 * @param parseContext      Object with parser settings.
 *                          (Type: ObjLib_KvParamsType)
 *
 * @return                  Type of the specified section. If no type is
 *                          specified it will return the type template
 *                          (ObjLib_KvObjectType).
 */
stock ObjectType:ObjLib_GetSectionType(const String:sectionName[], Object:parseContext)
{
    // Make sure type template is ready.
    ObjLib_BuildKvObjectType();
    
    // Get type of parent section. It is currently on the top of the stack
    // (when called from ObjLib_OnSectionStart). If the stack is empty it will
    // get rootType in parseParams.
    new ObjectType:parentType = ObjLib_KvGetCurrentType(parseContext);
    
    // Check if no type specified at all.
    if (parentType == INVALID_OBJECT_TYPE
        || !ObjLib_KvHasRootType(parseContext))
    {
        // Not even a root type is available. Use type template.
        return ObjLib_KvObjectType;
    }
    
    // Get constraints for this section.
    new keyIndex = ObjLib_GetKeyIndexFromType(parentType, sectionName);
    if (keyIndex < 0)
    {
        // Key doesn't exist in parent object, it's mutable. Use type template.
        // TODO: Is this always the case? What if parent is typed, but this
        //       section is invalid. It is validated at the end of the section,
        //       but it might be better to make sure it's valid as early as
        //       possible.
        return ObjLib_KvObjectType;
    }
    
    // Check if this section has no constraints.
    new Object:constraints = ObjLib_GetTypeConstraintsAt(parentType, keyIndex);
    if (constraints == INVALID_OBJECT)
    {
        // No constraint object. Use type template.
        return ObjLib_KvObjectType;
    }
    
    // Check if this section is bound by object constraints.
    new ObjectType:constraintType = ObjLib_GetTypeDescriptor(constraints);
    if (constraintType == ObjLib_ObjectConstraints)
    {
        // Use type descriptor specified in the constraint object.
        new ObjectType:typeDescriptor = ObjLib_GetObjectType(constraints, "type");
        
        // Validate type descriptor in constraint.
        if (typeDescriptor == INVALID_OBJECT_TYPE)
        {
            // Constraint object has no type constraint, use type template.
            return ObjLib_KvObjectType;
        }
        
        // Type is valid.
        return typeDescriptor;
    }
    
    // No constraints, use type template.
    return ObjLib_KvObjectType;
}

/*____________________________________________________________________________*/

/**
 * Returns whether the parser has a root type specified.
 *
 * @param       Object with parser settings.
 *              (Type: ObjLib_KvParamsType)
 *
 * @return      True if root type exist, false otherwise.
 */
stock bool:ObjLib_KvHasRootType(Object:parseContext)
{
    return ObjLib_GetObjectType(parseContext, "rootType") != INVALID_OBJECT_TYPE;
}

/*____________________________________________________________________________*/

/**
 * Builds a path to the current parser location.
 *
 * @param parseParam        Object with parser settings.
 * @param buffer            Destination string buffer.
 * @param maxlen            Size of buffer.
 *
 * @return                  Number of bytes written (including null terminator).
 */
stock ObjLib_KvBuildPath(Object:parseParams, String:buffer[], maxlen)
{
    new Handle:nameStack = ObjLib_GetHandle(parseParams, "nameStack");
    new len = GetArraySize(nameStack);
    new count = 0;
    
    decl String:nameElement[OBJECT_KEY_NAME_LEN];
    nameElement[0] = 0;
    
    // Initialize root path.
    count = strcopy(buffer, maxlen, "/");
    
    // Check if no elements.
    if (len == 0)
    {
        return count + 1;
    }
    
    // Append name elements to path.
    for (new i = 0; i < len; i++)
    {
        GetArrayString(nameStack, i, nameElement, sizeof(nameElement));
        count += StrCat(buffer, maxlen, nameElement);
        
        // Append path separator, except at the end.
        if (i < len - 1)
        {
            count += StrCat(buffer, maxlen, "/");
        }
    }
    
    return count + 1;
}

/*____________________________________________________________________________*/

/**
 * Example of full recursive traversal.
 */
stock BrowseKeyValues(Handle:kv, level = 0)
{
    new String:sectionName[32];
    new String:value[32];
    
    PrintToServer("-- Section start (level %d) --", level);
    do
    {
        KvGetSectionName(kv, sectionName, sizeof(sectionName));
        PrintToServer("Current key: %s", sectionName);
        
        // Check if current key is a section. Assume it has sub keys and attempt
        // to enter the section.
        if (KvGotoFirstSubKey(kv, false))
        {
            // Success. Confirmed that it's a sub key.
            
            PrintToServer("Sub key. Recursive entry.");
            BrowseKeyValues(kv, level + 1);
            
            PrintToServer("Going back.");
            KvGoBack(kv);
        }
        else
        {
            // Failed. It's a regular key, or the section is empty. Find out by
            // getting the data type of the key.
            
            // Get data type of current key.
            new KvDataTypes:dataType = KvGetDataType(kv, NULL_STRING);
            
            // Check if key has a data type. If not, the section is empty.
            if (dataType != KvData_None)
            {
                // Cursor is on a key.
                KvGetString(kv, NULL_STRING, value, sizeof(value));
                PrintToServer("Regular key. Value: \"%s\"", value);
            }
            else
            {
                // Section is empty. Do nothing.
                PrintToServer("Section is empty.");
            }
        }
    } while (KvGotoNextKey(kv, false));
    PrintToServer("-- Section end --");
}
