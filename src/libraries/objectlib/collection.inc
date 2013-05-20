/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           collection.inc
 *  Type:           Library
 *  Description:    Collection objects. Stores a collection of elements of a
 *                  certain type.
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
 * Global type descriptor for collection objects. Do not delete this type. You
 * can compare object types against this type to see if it's a collection
 * object (by using ObjLib_TypeOf).
 */
new ObjectType:ObjLibCollectionType = INVALID_OBJECT_TYPE;
new bool:ObjLibCollectionTypeBuilt = false;

/*____________________________________________________________________________*/

/**
 * Creates a collection object that stores entries of the specified data type.
 *
 * @param dataType          (Optinal) Data type of entries in collection.
 *                          Default is ObjDataType_Object.
 * @param blockSize         Number of cells each element can hold. Only useful
 *                          for string and array types. Default is 1 cell.
 * @param constraints       (Optional) NOT IMPLEMENTED! Element constraints.
 *                          Must be a constraint object. Default is no
 *                          constraints.
 *
 * @return                  Reference to new collection object. Must be deleted
 *                          with ObjLib_DeleteCollection when no longer in use.
 *
 * @error                   
 */
stock Collection:ObjLib_CreateCollection(ObjectDataType:dataType = ObjDataType_Object, blockSize = 1, Object:constraints = INVALID_OBJECT)
{
    // Validate constraints.
    if (constraints != INVALID_OBJECT && !ObjLib_IsConstraintObject(constraints))
    {
        ThrowError("Invalid constraint object (%x).", constraints);
    }
    
    // Create collection object.
    new Object:collection = ObjLib_CreateObject(ObjLibCollectionType, false);
    
    // Create element array.
    new Handle:elements = CreateArray(blockSize);
    
    // Initialize collection.
    ObjLib_SetCell(collection, "dataType", dataType);
    ObjLib_SetHandle(collection, "elements", elements);
    ObjLib_SetObject(collection, "constraints", constraints);
    
    return Collection:collection;
}

/*____________________________________________________________________________*/

/**
 * Deletes a collection object and its elements.
 *
 * @param collection        Collection object to delete.
 * @param resetReference    (Optional) Reset object to INVALID_COLLECTION when
 *                          deleted. Default is true.
 *
 * @error                   Invalid collection object.
 */
stock ObjLib_DeleteCollection(Collection:collection, bool:resetReference = true)
{
    // Delete the collection object. Also close handles stored in keys.
    ObjLib_DeleteObject(Object:collection, false, true);
    
    // Reset reference if enabled.
    if (resetReference)
    {
        collection = INVALID_COLLECTION;
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 */
stock ObjLib_BuildCollectionType()
{
    if (!ObjLibCollectionTypeBuilt)
    {
        ObjLibCollectionType = ObjLib_CreateType(
                1,                      // blockSize
                ByteCountToCells(16),   // keySzie
                INVALID_FUNCTION);      // errorHandler
        
        // Data type of elements in collection.
        ObjLib_AddKey(ObjLibCollectionType, "dataType", ObjDataType_Cell);
        
        // Handle to array of elements.
        ObjLib_AddKey(ObjLibCollectionType, "elements", ObjDataType_Handle);
        
        // Element constraints (applies to all elements in the collection).
        ObjLib_AddKey(ObjLibCollectionType, "constraints", ObjDataType_Object);
        
        ObjLibCollectionTypeBuilt = true;
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 */
stock ObjLib_ValidateCollection(Collection:collection)
{
    if (collection == INVALID_COLLECTION
        || !ObjLib_TypeOf(Object:collection, ObjLibCollectionType))
    {
        ThrowError("Invalid collection (%x).", collection);
    }
}

/*____________________________________________________________________________*/

/**
 * Internal use only!
 * Validates a data type against a collection's data type at the specified.
 *
 * This function will throw an validation error on type mismatch.
 *
 * @param collection        Collection object.
 * @param dataType          Data type to validate.
 * @param errorHandler      Custom error handler. Overrides error handler in
 *                          type descriptor if specified.
 *
 * @return                  True if passed, false otherwise.
 */
stock bool:ObjLib_CollectionTypeCheck(Collection:collection, ObjectDataType:dataType, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    // Get collection data type.
    new ObjectDataType:collectionType = ObjectDataType:ObjLib_GetCell(Object:collection, "dataType");
    
    // Check for type mismatch.
    if (dataType != collectionType)
    {
        new ObjectType:typeDescriptor = ObjLib_GetTypeDescriptor(Object:collection);
        ObjLib_TypeMismatchError(typeDescriptor, Object:collection, dataType, collectionType, errorHandler);
        return false;
    }
    
    return true;
}
