/*
 * ============================================================================
 *
 *  PawnUnit
 *
 *  File:          testcase.inc
 *  Type:          Include
 *  Description:   PawnUnit test case data structure manager.
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

#if defined _pawnunit_testcase_included
 #endinput
#endif
#define _pawnunit_testcase_included

/**
 * Test case reference tag.
 */
enum TestCase
{
    INVALID_TEST_CASE = -1
}

/**
 * Defines the test case data elements.
 *
 * Note: Do not modify the order of these elements. They are used to set array
 *       indexes.
 */
enum TestCaseData
{
    TestCase_Phases = 0,    /** Array of function handles. */
    TestCase_Passed,        /** Specifies whether the test passed or not. */
    TestCase_ErrorMessage,  /** Error message if the test failed. */
    TestCase_Name,          /** Display name of test case. */
}

/**
 * Creates a test case.
 *
 * Note: The TestCase reference must be deleted with PawnUnit_DeleteTestCase
 *       when no longer in use.
 *
 * @param name      Name of test case.
 * @param phases    Optional. Array of function handles. Default is an empty
 *                  array.
 *
 * @return          Reference to the test case data structure.
 */
stock TestCase:PawnUnit_CreateTestCase(const String:name[], Handle:phases = INVALID_HANDLE)
{
    // Create array to store data in. Reserve space for strings.
    new Handle:testCase = CreateArray(PAWNUNIT_MSG_LEN);
    
    // Create empty phase array if not specified.
    if (phases == INVALID_HANDLE)
    {
        phases = CreateArray();
    }
    
    // Write test case data.
    // Note: Update TestCaseData enum if write order is changed.
    PushArrayCell(testCase, phases);    // TestCase_Phases
    PushArrayCell(testCase, 0);         // TestCase_Passed
    PushArrayString(testCase, "");      // TestCase_ErrorMessage
    PushArrayString(testCase, name);    // TestCase_Name
    
    return TestCase:testCase;
}

/**
 * Deletes a test case.
 *
 * @param testCase          Test case reference.
 * @param resetReference    Optional. Reset testCase to INVALID_TEST_CASE when
 *                          deleted. Default is true.
 */
stock Handle:PawnUnit_DeleteTestCase(&TestCase:testCase, bool:resetReference = true)
{
    PawnUnit_ValidateTestCase(testCase);
    
    // Delete test phase array.
    CloseHandle(Handle:GetArrayCell(Handle:testCase, _:TestCase_Phases));
    
    CloseHandle(Handle:testCase);
    
    if (resetReference)
    {
        testCase = INVALID_TEST_CASE;
    }
}

/**
 * Gets the test phase function array.
 *
 * @param testCase  Test case reference.
 *
 * @return          Handle to array.
 */
stock Handle:PawnUnit_GetTestPhases(TestCase:testCase)
{
    PawnUnit_ValidateTestCase(testCase);
    
    return Handle:GetArrayCell(Handle:testCase, _:TestCase_Phases);
}

/**
 * Adds a test phase function to a test case.
 *
 * @param testCase      Test case reference.
 * @param phase         Function to execute in this phase.
 */
stock PawnUnit_AddTestPhase(TestCase:testCase, PawnUnitTestPhase:phase)
{
    PawnUnit_ValidateTestCase(testCase);
    PawnUnit_ValidateTestPhase(phase);
    
    new Handle:phases = PawnUnit_GetTestPhases(testCase);
    PushArrayCell(phases, _:phase);
}

/**
 * Gets the test case name.
 *
 * @param testCase      Test case reference.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of buffer.
 *
 * @return              Number of characters copied.
 */
stock PawnUnit_GetTestCaseName(TestCase:testCase, String:buffer[], maxlen)
{
    PawnUnit_ValidateTestCase(testCase);
    
    return GetArrayString(Handle:testCase, _:TestCase_Name, buffer, maxlen);
}

/**
 * Sets the test case name.
 *
 * @param testCase      Test case reference.
 * @param name          New name to set.
 *
 * @return              Number of characters copied.
 */
stock PawnUnit_SetTestCaseName(TestCase:testCase, const String:name[])
{
    PawnUnit_ValidateTestCase(testCase);
    
    return SetArrayString(Handle:testCase, _:TestCase_Name, name);
}

/**
 * Gets the test case error message.
 *
 * @param testCase      Test case reference.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of buffer.
 *
 * @return              Number of characters copied.
 */
stock PawnUnit_GetTestCaseError(TestCase:testCase, String:buffer[], maxlen)
{
    PawnUnit_ValidateTestCase(testCase);
    
    return GetArrayString(Handle:testCase, _:TestCase_ErrorMessage, buffer, maxlen);
}

/**
 * Sets the test case error message.
 *
 * @param testCase      Test case reference.
 * @param errorMessage  New error message to set.
 *
 * @return              Number of characters copied.
 */
stock PawnUnit_SetTestCaseError(TestCase:testCase, const String:errorMessage[])
{
    PawnUnit_ValidateTestCase(testCase);
    
    return SetArrayString(Handle:testCase, _:TestCase_ErrorMessage, errorMessage);
}

/**
 * Gets whether a test case passed the test.
 *
 * Note: Only valid after testing is done.
 *
 * @param testCase      Test case reference.
 *
 * @return              True if passed, false otherwise.
 */
stock bool:PawnUnit_GetTestPassed(TestCase:testCase)
{
    PawnUnit_ValidateTestCase(testCase);
    
    return bool:GetArrayCell(Handle:testCase, _:TestCase_Passed);
}

/**
 * Sets whether a test case passed the test.
 *
 * @param testCase      Test case reference.
 * @param passed        Passed state.
 */
stock bool:PawnUnit_SetTestPassed(TestCase:testCase, bool:passed)
{
    PawnUnit_ValidateTestCase(testCase);
    
    SetArrayCell(Handle:testCase, _:TestCase_Passed, passed);
}


/****************************
 *   Validation functions   *
 ****************************/

/**
 * Returns whether the test case reference is valid.
 *
 * @param testCase      Test case reference to validate.
 *
 * @return              True if valid, false otherwise.
 */
stock bool:PawnUnit_IsValidTestCase(TestCase:testCase)
{
    return testCase != INVALID_TEST_CASE;
}

/**
 * Throws an error if the test case is invalid.
 *
 * @param testCase      Test case reference to validate.
 */
stock PawnUnit_ValidateTestCase(TestCase:testCase)
{
    if (!PawnUnit_IsValidTestCase(testCase))
    {
        ThrowError("Invalid test case.");
    }
}

/**
 * Returns whether the test phase function is valid.
 *
 * @param phase         Test phase function to validate.
 *
 * @return              True if valid, false otherwise.
 */
stock bool:PawnUnit_IsValidPhase(PawnUnitTestPhase:phase)
{
    return phase != INVALID_FUNCTION;
}

/**
 * Throws an error if the test phase is invalid.
 *
 * @param phase         Test phase to validate.
 */
stock PawnUnit_ValidateTestPhase(Function:phase)
{
    if (!PawnUnit_IsValidPhase(phase))
    {
        ThrowError("Invalid test phase function reference.");
    }
}
