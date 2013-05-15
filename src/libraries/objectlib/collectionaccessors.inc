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
    
    ObjLib_CollectionAddCellAt(collection, index, any:value);
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_CollectionAddCellAt(Collection:collection, index, any:value)
{
    // Get array.
    new Handle:elements = ObjLib_GetHandle(Object:collection, "elements");
    new size = GetArraySize(elements);
    
    // Validate index.
    if (index >= size)
    {
        ThrowError("Index out of bounds (%d).", index);
        return;
    }
    
    // Move elements to make free space at the specified index.
    ShiftArrayUp(elements, index);
    
    // Insert value.
    SetArrayCell(elements, index, value);
}
