/*
 * ============================================================================
 *
 *  PawnUnit
 *
 *  File:          testcollection.inc
 *  Type:          Include
 *  Description:   PawnUnit test collection data structure manager.
 *
 *  Copyright (C) 2012  Richard Helgeby
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

#if defined _pawnunit_testcollection_included
 #endinput
#endif
#define _pawnunit_testcollection_included

/**
 * Test collection reference tag.
 */
enum TestCollection
{
    INVALID_TEST_COLLECTION = -1
}

/**
 * Defines the test collection data elements.
 *
 * Note: Do not modify the order of these elements. They are used to set array
 *       indexes.
 */
enum TestCollectionData
{
    TestCollection_TestCases,   /** Array of test case references. */
    TestCollection_Before,      /** Function to execute before test case. */
    TestCollection_After,       /** Function to execute after test case. */
    TestCollection_Name,        /** Display name of test collection. */
}

/**
 * Creates a test collection.
 *
 * Note: The TestCollection reference must be deleted with
 *       PawnUnit_DeleteTestCollection when no longer in use.
 *
 * @param name          Name of test collection.
 * @param testCases     Optional. Array of test case references. Default is an
 *                      empty array.
 * @param before        Optional. Function to execute before every test case.
 * @param after         Optional. Function to execute after every test case.
 *
 * @return              Reference to the test collection data structure.
 */
stock TestCollection:PawnUnit_CreateTestCollection(const String:name[],
        Handle:testCases = INVALID_HANDLE,
        Function:before = INVALID_FUNCTION,
        Function:after = INVALID_FUNCTION)
{
    // Create array to store data in. Reserve space for strings.
    new Handle:collection = CreateArray(PAWNUNIT_MSG_LEN);
    
    // Create empty test case array if not specified.
    if (testCases == INVALID_HANDLE)
    {
        testCases = CreateArray();
    }
    
    // Write test collection data.
    // Note: Update TestCollectionData enum if write order is changed.
    PushArrayCell(collection, _:testCases); // TestCollection_TestCases
    PushArrayCell(collection, _:before);    // TestCollection_Before
    PushArrayCell(collection, _:after);     // TestCollection_After
    PushArrayString(collection, name);      // TestCollection_Name
    
    return TestCollection:collection;
}

/**
 * Deletes a test collection.
 *
 * @param collection        Test collection reference.
 * @param recursive         Whether to delete the test cases in the collection.
 *                          Warning: Not doing a recursive deletion may cause a
 *                          memory leak if its test cases aren't deleted
 *                          individually too.
 * @param resetReference    Optional. Reset testCollection to
 *                          INVALID_TEST_COLLECTION when deleted. Default is
 *                          true.
 */
stock Handle:PawnUnit_DeleteTestCollection(&TestCollection:collection,
        bool:recursive = true,
        bool:resetReference = true)
{
    PawnUnit_ValidateTestCollection(collection);
    
    new Handle:testCases = GetArrayCell(Handle:collection, _:TestCollection_TestCases);
    
    if (recursive)
    {
        new len = GetArraySize(testCases);
        for (new i = 0; i < len; i++)
        {
            new TestCase:testCase = GetArrayCell(testCases, i);
            PawnUnit_DeleteTestCase(testCase);
        }
    }
    
    CloseHandle(testCases);
    CloseHandle(Handle:collection);
    
    if (resetReference)
    {
        collection = INVALID_TEST_COLLECTION;
    }
}

/**
 * Gets the test case array.
 *
 * @param collection    Test collection reference.
 *
 * @return              Handle to array.
 */
stock Handle:PawnUnit_GetTestCases(TestCollection:collection)
{
    PawnUnit_ValidateTestCollection(collection);
    
    return Handle:GetArrayCell(Handle:collection, _:TestCollection_TestCases);
}

/**
 * Adds a test case to a test collection.
 *
 * @param collection        Test collection reference.
 * @param testCase          Test case to add.
 */
stock PawnUnit_AddTestCase(TestCollection:collection, TestCase:testCase)
{
    PawnUnit_ValidateTestCollection(collection);
    PawnUnit_ValidateTestCase(testCase);
    
    new Handle:testCases = PawnUnit_GetTestCases(collection);
    PushArrayCell(testCases, testCase);
}

/**
 * Gets the function reference for the function to execute before every test
 * case.
 *
 * @param collection        Test collection reference.
 *
 * @return                  Function reference or INVALID_FUNCTION if none
 *                          specified.
 */
stock Function:PawnUnit_GetBefore(TestCollection:collection)
{
    PawnUnit_ValidateTestCollection(collection);
    
    return Function:GetArrayCell(Handle:collection, _:TestCollection_Before);
}

/**
 * Sets the function reference for the function to execute before every test
 * case.
 *
 * @param collection        Test collection reference.
 * @param before            New function to execute before every test case.
 *
 * @return                  Function reference or INVALID_FUNCTION if none
 *                          specified.
 */
stock PawnUnit_SetBefore(TestCollection:collection, Function:before)
{
    PawnUnit_ValidateTestCollection(collection);
    
    SetArrayCell(Handle:collection, _:TestCollection_Before, _:before);
}

/**
 * Gets the function reference for the function to execute after every test
 * case.
 *
 * @param collection        Test collection reference.
 *
 * @return                  Function reference or INVALID_FUNCTION if none
 *                          specified.
 */
stock Function:PawnUnit_GetAfter(TestCollection:collection)
{
    PawnUnit_ValidateTestCollection(collection);
    
    return Function:GetArrayCell(Handle:collection, _:TestCollection_After);
}

/**
 * Sets the function reference for the function to execute after every test
 * case.
 *
 * @param collection        Test collection reference.
 * @param after             New function to execute after every test case.
 *
 * @return                  Function reference or INVALID_FUNCTION if none
 *                          specified.
 */
stock PawnUnit_SetAfter(TestCollection:collection, Function:after)
{
    PawnUnit_ValidateTestCollection(testCollection);
    
    SetArrayCell(Handle:collection, _:TestCollection_After, _:after);
}

/**
 * Gets the test collection name.
 *
 * @param collection        Test collection reference.
 * @param buffer            Destination string buffer.
 * @param maxlen            Size of buffer.
 *
 * @return                  Number of characters copied.
 */
stock PawnUnit_GetTestCollectionName(TestCollection:collection, String:buffer[], maxlen)
{
    PawnUnit_ValidateTestCollection(collection);
    
    return GetArrayString(Handle:collection, _:TestCollection_Name, buffer, maxlen);
}

/**
 * Sets the test collection name.
 *
 * @param collection        Test collection reference.
 * @param name              New name to set.
 *
 * @return                  Number of characters copied.
 */
stock PawnUnit_SetTestCollectionName(TestCollection:collection, const String:name[])
{
    PawnUnit_ValidateTestCollection(collection);
    
    SetArrayString(Handle:collection, _:TestCollection_Name, name);
}


/****************************
 *   Validation functions   *
 ****************************/

/**
 * Returns whether the test collection reference is valid.
 *
 * @param collection        Test collection reference to validate.
 *
 * @return                  True if valid, false otherwise.
 */
stock bool:PawnUnit_IsValidTestCollection(TestCollection:collection)
{
    return collection != INVALID_TEST_COLLECTION;
}

/**
 * Throws an error if the test collection is invalid.
 *
 * @param collection        Test collection reference to validate.
 */
stock PawnUnit_ValidateTestCollection(TestCollection:collection)
{
    if (!PawnUnit_IsValidTestCollection(collection))
    {
        ThrowError("Invalid test collection.");
    }
}
