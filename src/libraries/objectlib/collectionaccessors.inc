/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           collectionaccessors.inc
 *  Type:           Library
 *  Description:    Accessor functions for collection objects.
 *
 *  Copyright (C) 2013  Richard Helgeby, Greyscale
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

/**
 * Adds a cell value at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param value         Value to add.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddCell(Collection:collection, any:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Cell, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionAddCell(collection, value);
}

/*____________________________________________________________________________*/

/**
 * Inserts a cell value at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param value         Value to insert.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddCellAt(Collection:collection, index, any:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Cell, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionInsertCellAt(collection, index, value);
}

/*____________________________________________________________________________*/

/**
 * Adds a boolean value at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param value         Value to add.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddBool(Collection:collection, bool:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Bool, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionAddCell(collection, value);
}

/*____________________________________________________________________________*/

/**
 * Inserts a boolean value at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param value         Value to insert.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddBoolAt(Collection:collection, index, bool:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Bool, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionInsertCellAt(collection, index, value);
}

/*____________________________________________________________________________*/

/**
 * Adds a float value at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param value         Value to add.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddFloat(Collection:collection, Float:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Float, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionAddCell(collection, value);
}

/*____________________________________________________________________________*/

/**
 * Inserts a float value at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param value         Value to insert.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddFloatAt(Collection:collection, index, Float:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Float, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionInsertCellAt(collection, index, value);
}

/*____________________________________________________________________________*/

/**
 * Adds a handle value at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param value         Value to add.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddHandle(Collection:collection, Handle:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Handle, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionAddCell(collection, value);
}

/*____________________________________________________________________________*/

/**
 * Inserts a handle value at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param value         Value to insert.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddHandleAt(Collection:collection, index, Handle:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Handle, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionInsertCellAt(collection, index, value);
}

/*____________________________________________________________________________*/

/**
 * Adds a function value at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param value         Value to add.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddFunction(Collection:collection, Function:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Function, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionAddCell(collection, value);
}

/*____________________________________________________________________________*/

/**
 * Inserts a function value at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param value         Value to insert.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddFunctionAt(Collection:collection, index, Function:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Function, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionInsertCellAt(collection, index, value);
}

/*____________________________________________________________________________*/

/**
 * Adds an array at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param values        Values to insert.
 * @param size          Size of array.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddArray(Collection:collection, const any:values[], size, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Array, errorHandler))
    {
        return;
    }
    
    stock ObjLib_CollectionAddArray(collection, values, size);
}

/*____________________________________________________________________________*/

/**
 * Inserts an array at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param values        Values to insert.
 * @param size          Size of array.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddArrayAt(Collection:collection, index, const any:values[], size, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Array, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionInsertArrayAt(collection, index, values, size);
}

/*____________________________________________________________________________*/

/**
 * Adds a string value at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param value         Value to add.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddString(Collection:collection, const String:value[], ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_String, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionAddString(collection, value);
}

/*____________________________________________________________________________*/

/**
 * Inserts a string value at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param value         Value to insert.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddStringAt(Collection:collection, index, const String:value[], ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_String, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionInsertStringAt(collection, index, value);
}

/*____________________________________________________________________________*/

/**
 * Adds an object reference at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param value         Value to add.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddObject(Collection:collection, Object:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Object, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionAddCell(collection, value);
}

/*____________________________________________________________________________*/

/**
 * Inserts an object reference at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param value         Value to insert.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddObjectAt(Collection:collection, index, Object:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_Object, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionInsertCellAt(collection, index, value);
}

/*____________________________________________________________________________*/

/**
 * Adds a type descriptor reference at end of the collection.
 *
 * @param collection    Collection object to add to.
 * @param value         Value to add.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 */
stock ObjLib_AddObjectType(Collection:collection, ObjectType:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_ObjectType, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionAddCell(collection, value);
}

/*____________________________________________________________________________*/

/**
 * Inserts a type descriptor reference at the specified index in the collection.
 *
 * @param collection    Collection object to add to.
 * @param index         Index where value will be inserted.
 * @param value         Value to insert.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_AddObjectTypeAt(Collection:collection, index, ObjectType:value, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    
    // Validate type.
    if (!ObjLib_CollectionTypeCheck(collection, ObjDataType_ObjectType, errorHandler))
    {
        return;
    }
    
    ObjLib_CollectionInsertCellAt(collection, index, value);
}

/*____________________________________________________________________________*/

/**
 * Removes a cell value at the specified index in the collection.
 *
 * @param collection    Collection object to remove from.
 * @param index         Index of value to remove.
 * @param errorHandler  Custom error handler. Overrides any other error handler
 *                      if specified.
 *
 * @error               Invalid index.
 */
stock ObjLib_RemoveAt(Collection:collection, index, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    ObjLib_ValidateCollection(collection);
    ObjLib_CollectionRemoveAt(Collection:collection, index);
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Inserts or adds a value/array to a collection using the appropriate public
 * functions.
 *
 * @param collection            The collection object.
 * @param dataType              Data type of value to insert.
 * @param index                 Index to insert at. Use a negative index to add
 *                              at the end of the collection.
 * @param values                Values to insert. Only the first value is used
 *                              for non-array types.
 * @param size                  Number of values.
 * @param errorHandler          Error handler to use for general object errors.
 */
stock ObjLib_CollectionSetValue(Collection:collection, ObjectDataType:dataType, index, const any:values[], size, ObjLib_ErrorHandler:errorHandler)
{
    // Whether to insert or add.
    new bool:insert = (index >= 0);
    
    // Use appropriate add function according to the data type.
    switch (dataType)
    {
        case ObjDataType_Any:
        {
            ThrowError("Not implemented!");
        }
        case ObjDataType_Cell:
        {
            if (insert)
            {
                ObjLib_AddCellAt(collection, index, values[0], errorHandler);
            }
            else
            {
                ThrowError("Not implemented!");
                //ObjLib_AddCell(collection, index, values[0], errorHandler);
            }
        }
        case ObjDataType_Bool:
        {
            ThrowError("Not implemented!");
        }
        case ObjDataType_Float:
        {
            ThrowError("Not implemented!");
        }
        case ObjDataType_Handle:
        {
            ThrowError("Not implemented!");
        }
        case ObjDataType_Function:
        {
            ThrowError("Not implemented!");
        }
        case ObjDataType_Array:
        {
            ThrowError("Not implemented!");
        }
        case ObjDataType_String:
        {
            ThrowError("Not implemented!");
        }
        case ObjDataType_Object:
        {
            ThrowError("Not implemented!");
        }
        case ObjDataType_ObjectType:
        {
            ThrowError("Not implemented!");
        }
        default:
        {
            // Unexpected type.
            ThrowError("[BUG] Unexpected data type. This is a bug in objectlib.");
        }
    }
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_CollectionInsertCellAt(Collection:collection, index, any:value)
{
    // Get array.
    new Handle:elements = ObjLib_GetHandle(Object:collection, "elements");
    new elementCount = GetArraySize(elements);
    
    // Validate index.
    ObjLib_CollectionIndexCheck(elementCount, index);
    
    // Move elements to make free space at the specified index.
    ShiftArrayUp(elements, index);
    
    // Insert value.
    SetArrayCell(elements, index, value);
    
    return 1;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_CollectionInsertArrayAt(Collection:collection, index, const any:values[], size)
{
    // Get array.
    new Handle:elements = ObjLib_GetHandle(Object:collection, "elements");
    new elementCount = GetArraySize(elements);
    
    // Validate index.
    ObjLib_CollectionIndexCheck(elementCount, index);
    
    // Move elements to make free space at the specified index.
    ShiftArrayUp(elements, index);
    
    // Insert array.
    return SetArrayArray(elements, index, values, size);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_CollectionInsertStringAt(Collection:collection, index, const String:value[])
{
    // Get array.
    new Handle:elements = ObjLib_GetHandle(Object:collection, "elements");
    new elementCount = GetArraySize(elements);
    
    // Validate index.
    ObjLib_CollectionIndexCheck(elementCount, index);
    
    // Move elements to make free space at the specified index.
    ShiftArrayUp(elements, index);
    
    // Insert string.
    return SetArrayString(elements, index, value);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_CollectionAddCell(Collection:collection, any:value)
{
    // Get array.
    new Handle:elements = ObjLib_GetHandle(Object:collection, "elements");
    
    // Add value.
    PushArrayCell(elements, value);
    return 1;
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_CollectionAddArray(Collection:collection, const any:values[], size)
{
    // Get array.
    new Handle:elements = ObjLib_GetHandle(Object:collection, "elements");
    
    // Add array.
    return PushArrayArray(elements, values, size);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_CollectionAddString(Collection:collection, const String:value[])
{
    // Get array.
    new Handle:elements = ObjLib_GetHandle(Object:collection, "elements");
    
    // Add string.
    return PushArrayString(elements, value);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_CollectionRemoveAt(Collection:collection, index)
{
    // Get array.
    new Handle:elements = ObjLib_GetHandle(Object:collection, "elements");
    new elementCount = GetArraySize(elements);
    
    // Validate index.
    ObjLib_CollectionIndexCheck(elementCount, index);
    
    // Remove element.
    RemoveFromArray(elements, index);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_CollectionIndexCheck(elementCount, index)
{
    // Validate index.
    if (!Util_IsInBounds(0, elementCount - 1, index))
    {
        ThrowError("Index out of bounds (%d).", index);
    }
}
