/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:           errorhandler.inc
 *  Type:           Library
 *  Description:    Error handler for objectlib.
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

/**
 * Internal use only!
 * General error handler. Calls the error handler if available, or throws a
 * regular error.
 *
 * @param typeDescriptor    Related type descriptor.
 * @param object            Related object, if available.
 * @param errorType         Type of error.
 * @param customHandler     Custom error handler. Overrides handler in type
 *                          descriptor if specified.
 * @param format            Message formatting rules.
 * @param ...               Format parameters.
 *
 * @return      True if error was handled. If it's not handled this function
 *              doesn't return at all.
 */
stock bool:ObjLib_HandleError(ObjectType:typeDescriptor,
                              Object:object = INVALID_OBJECT,
                              ObjLibError:errorType,
                              ObjLib_ErrorHandler:customHandler = INVALID_FUNCTION,
                              const String:format[],
                              any:...)
{
    // Get error handler callback.
    new ObjLib_ErrorHandler:callback;
    if (customHandler != INVALID_FUNCTION)
    {
        // Use custom error handler.
        callback = customHandler;
    }
    else
    {
        // Use handler in type descriptor.
        callback = ObjLib_GetTypeErrorHandler(typeDescriptor);
    }
    
    // Format message.
    decl String:message[255];
    message[0] = 0;
    VFormat(message, sizeof(message), format, 5);
    
    if (callback == INVALID_FUNCTION)
    {
        // No error handler. Throw error instead.
        ThrowError(message);
        
        return false;   // To satisfy compiler.
    }
    else
    {
        // Forward to callback.
        // functag public ObjLib_ErrorHandler(ObjectType:typeDescriptor, ObjLibError:errorType, const String:message[]);
        Call_StartFunction(GetMyHandle(), callback);
        Call_PushCell(typeDescriptor);
        Call_PushCell(errorType);
        Call_PushString(message);
        Call_PushCell(object);
        Call_Finish();
        return true;
    }
}

/*____________________________________________________________________________*/

/** Internal use only! */
stock ObjLib_ErrorHandler:ObjLib_GetErrorHandler(ObjectType:typeDescriptor, ObjLib_ErrorHandler:errorHandler = INVALID_FUNCTION)
{
    if (errorHandler != INVALID_FUNCTION)
    {
        return errorHandler;
    }
    
    return ObjLib_GetTypeErrorHandler(typeDescriptor);
}
