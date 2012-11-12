/*
 * ============================================================================
 *
 *  PawnUnit
 *
 *  File:          pawnunit.inc
 *  Type:          Include
 *  Description:   PawnUnit testing API. See also testcase.inc and
 *                 testcollection.inc.
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

#if defined _pawnunit_included
 #endinput
#endif
#define _pawnunit_included

#define PAWNUNIT_NAME_LEN   64
#define PAWNUNIT_MSG_LEN    255

/**
 * What the test engine should do next.
 */
enum TestControlAction
{
    Test_Abort,         /** Abort testing. */
    Test_Continue,      /** Continue to next test case or collection. */
    Test_Failed,        /** Same as Test_Continue, but marks the test as failed. */
    Test_Suspend        /** Suspend testing. Resume with PawnUnit_Run. */
}

/**
 * Called when a test phase is executed.
 *
 * @param testCase      Reference to current test case.
 *
 * @return              What the test engine should do next. See enum
 *                      TestControlAction.
 */
functag public TestControlAction:PawnUnitTestPhase(TestCase:testCase);


// Strings.
#define PAWNUNIT_STR_RUNNING_TEST       "Running test \"%s\" at phase %d..."
#define PAWNUNIT_STR_TEST_FAILED        "TEST FAILED..."
#define PAWNUNIT_STR_TEST_PASSED        "Test passed..."
#define PAWNUNIT_STR_TESTING_SUSPENDED  "Testing suspended..."
#define PAWNUNIT_STR_TESTING_ABORTED    "Testing aborted..."
#define PAWNUNIT_STR_INVALID_ACTION     "Warning: Invalid test result action: %d  Continuing testing as usual..."


// Current states.
static stock bool:Suspended = false;

//static stock TestCase:CurrentTest = INVALID_TEST_CASE;
static stock CurrentTestIndex = -1;

//static stock PawnUnitTestPhase:CurrentPhase = INVALID_FUNCTION;
static stock CurrentPhaseIndex = -1;

// Global buffer for storing temporary messages.
stock String:MessageBuffer[PAWNUNIT_MSG_LEN];

// Cache of functions to call before and after tests.
static stock Function:BeforeTest = INVALID_FUNCTION;
static stock Function:AfterTest = INVALID_FUNCTION;

// Statistics.
static stock NumTestsPassed = 0;
static stock NumTestsFailed = 0;


#include <pawnunit/assert>
#include <pawnunit/testcase>
#include <pawnunit/testcollection>


/**
 * Run a test collection.
 *
 * @param collection        Test collection to run.
 */
stock PawnUnit_Run(TestCollection:collection)
{
    new Handle:testCases = PawnUnit_GetTestCases(collection);
    
    new String:collectionName[PAWNUNIT_NAME_LEN];
    PawnUnit_GetTestCollectionName(collection, collectionName, sizeof(collectionName));
    
    // Check if there are no tests to run.
    new numTests = GetArraySize(testCases);
    if (numTests == 0)
    {
        ThrowError("No test cases in collection \"%s\"", collectionName);
    }
    
    // Current test index.
    new test = 0;
    
    if (Suspended)
    {
        // Skip to pending test before testing was suspended.
        // Note: If this value is out of bounds, the for loop below will never
        //       execute and testing complete as usual.
        test = CurrentTestIndex + 1;
    }
    
    // Loop through test cases.
    for (; test < numTests && !Suspended; test++)
    {
        new TestCase:testCase = TestCase:GetArrayCell(testCases, test);
        
        // Validate test case.
        if (!PawnUnit_IsValidTestCase(testCase))
        {
            ThrowError("Invalid test case at index %d in collection \"%s\"", collectionName);
        }
        
        PawnUnit_ExecuteBefore(collection);
        
        // Run test case and save result action.
        new TestControlAction:testAction = PawnUnit_RunTest(TestCase:testCase, collectionName);
        
        PawnUnit_ExecuteAfter(collection);
        
        if (!PawnUnit_HandleResultAction(testAction, testCase))
        {
            break;
        }
    }
    
    if (Suspended)
    {
        CurrentTestIndex = test;
    }
}

/**
 * Run a test case.
 *
 * @param testCase          Test case to run.
 * @param collectionName    Optional. Name of collection this test case belongs
 *                          to. Default is none.
 *
 * @return                  Test result action.
 */
stock TestControlAction:PawnUnit_RunTest(TestCase:testCase, const String:collectionName[] = "")
{
    // Get array of test phases.
    new Handle:phases = PawnUnit_GetTestPhases(testCase);
    
    decl String:testName[PAWNUNIT_NAME_LEN];
    testName[0] = 0;
    PawnUnit_GetTestCaseName(testCase, testName, sizeof(testName));
    
    // Check if there are no test phases.
    new numPhases = GetArraySize(phases);
    if (numPhases == 0)
    {
        ThrowError("No test phases in test case \"%s\"", testName);
    }
    
    new phase = 0;
    if (Suspended)
    {
        // Skip to pending phase before testing was suspended.
        // Note: If this value is out of bounds, the for loop below will never
        //       execute and test case complete as usual.
        phase = CurrentPhaseIndex + 1;
        
        Suspended = false;
    }
    
    // Loop through phases.
    for (; phase < numPhases && !Suspended; phase++)
    {
        PawnUnit_Print(PAWNUNIT_STR_RUNNING_TEST, testName, phase);
        new PawnUnitTestPhase:testPhase = PawnUnitTestPhase:GetArrayCell(phases, phase);
        
        // Validate phase.
        if (!PawnUnit_IsValidPhase(testPhase))
        {
            ThrowError("Invalid test phase at index %d in test case \"%s\"", testName);
        }
        
        // Execute the test phase.
        new TestControlAction:phaseAction = PawnUnit_ExecutePhase(testCase, testPhase);
        
        // Handle action and check if stopping.
        if (phaseAction == Test_Continue)
        {
            // Do nothing. We intercept test passed message for each phase. This
            // is handled by PawnUnit_Run.
        }
        else if (phaseAction == Test_Failed)
        {
            // Test phase failed. Normally the handler will say that we should
            // continue, but this is not the case for test phases.
            PawnUnit_SetTestPassed(testCase, false);
            PawnUnit_SetTestCaseError(testCase, MessageBuffer);
            
            return phaseAction;
        }
        else if (!PawnUnit_HandleResultAction(phaseAction, testCase))
        {
            // Aborting or suspending.
            return phaseAction;
        }
    }
    
    if (Suspended)
    {
        CurrentPhaseIndex = phase;
    }
    
    return Test_Continue;
}

/**
 * Resets statistics.
 */
stock PawnUnit_ResetStatistics()
{
    NumTestsPassed = 0;
    NumTestsFailed = 0;
}

/**
 * Resets all states.
 */
stock PawnUnit_ResetStates()
{
    Suspended = false;
    //CurrentTest = INVALID_TEST_CASE;
    CurrentTestIndex = -1;
    //CurrentPhase = INVALID_FUNCTION;
    CurrentPhaseIndex = -1;
    MessageBuffer[0] = 0;
    BeforeTest = INVALID_FUNCTION;
    AfterTest = INVALID_FUNCTION;
}

/**
 * Prints test results for a collection in the server console.
 *
 * @param collection    Test collection reference.
 */
stock PawnUnit_PrintResults(TestCollection:collection)
{
    #define TESTS_IN_COLLECTION "Tests in collection \"%s\""
    #define HEADER_STRING       "Test name:                          Status:"
    #define LINE_STRING         "-------------------------------------------------------------------------------"
    #define DUBLE_LINE_STRING   "==============================================================================="
    #define ENTRY_FMT           "%35s %s"
    #define PASSED_STRING       "[PASSED]"
    #define TESTS_PASSED        "Tests passed:"
    #define TESTS_FAILED        "Tests failed:"

    PawnUnit_ValidateTestCollection(collection);
    
    new Handle:testCases = PawnUnit_GetTestCases(collection);
    
    new String:collectionName[PAWNUNIT_NAME_LEN];
    PawnUnit_GetTestCollectionName(collection, collectionName, sizeof(collectionName));
        
    // Check if there are no tests cases.
    new numTests = GetArraySize(testCases);
    if (numTests == 0)
    {
        ThrowError("No test cases in collection \"%s\"", collectionName);
    }
    
    // Print header.
    PrintToServer(TESTS_IN_COLLECTION, collectionName);
    PrintToServer("%s\n%s", HEADER_STRING, DUBLE_LINE_STRING);
    
    // Loop through test cases.
    for (new test = 0; test < numTests; test++)
    {
        new TestCase:testCase = TestCase:GetArrayCell(testCases, test);
        
        decl String:testName[PAWNUNIT_NAME_LEN];
        testName[0] = 0;
        PawnUnit_GetTestCaseName(testCase, testName, sizeof(testName));
        
        decl String:testMsg[PAWNUNIT_MSG_LEN];
        testMsg[0] = 0;
        PawnUnit_GetTestCaseError(testCase, testMsg, sizeof(testMsg));
        
        new bool:passed = PawnUnit_GetTestPassed(testCase);
        
        // Validate test case.
        if (!PawnUnit_IsValidTestCase(testCase))
        {
            LogError("Invalid test case at index %d in collection \"%s\"", collectionName);
            continue;
        }
        
        // Print result.
        if (passed)
        {
            PrintToServer(ENTRY_FMT, testName, PASSED_STRING);
        }
        else
        {
            PrintToServer(ENTRY_FMT, testName, testMsg);
        }
    }
    
    PrintToServer(LINE_STRING);
    
    PrintToServer("%20s %d\n%20s %d", TESTS_PASSED, NumTestsPassed, TESTS_FAILED, NumTestsFailed);
    PrintToServer(LINE_STRING);
}


/*************************
 *   Private functions   *
 *************************/

static stock TestControlAction:PawnUnit_ExecutePhase(TestCase:testCase, PawnUnitTestPhase:phase)
{
    new any:result = 0;
    
    Call_StartFunction(GetMyHandle(), phase);
    Call_PushCell(testCase);
    Call_Finish(result);
    
    return TestControlAction:result;
}

/**
 * Handles the test result action.
 *
 * @return      True if testing should continue, false if aborting or
 *              suspending.
 */
static stock bool:PawnUnit_HandleResultAction(TestControlAction:action, TestCase:testCase)
{
    switch (action)
    {
        case Test_Abort:
        {
            PawnUnit_Print(PAWNUNIT_STR_TESTING_ABORTED);
            
            PawnUnit_SetTestPassed(testCase, false);
            PawnUnit_SetTestCaseError(testCase, "Testing aborted.");
            PawnUnit_ResetStates();
            
            NumTestsFailed++;
            
            // Stop.
            return false;
        }
        case Test_Continue:
        {
            // Test passed.
            PawnUnit_Print(PAWNUNIT_STR_TEST_PASSED);
            PawnUnit_SetTestPassed(testCase, true);
            
            NumTestsPassed++;
        }
        case Test_Failed:
        {
            PawnUnit_Print(PAWNUNIT_STR_TEST_FAILED);
            PawnUnit_SetTestPassed(testCase, false);
            PawnUnit_SetTestCaseError(testCase, MessageBuffer);
            
            NumTestsFailed++;
        }
        case Test_Suspend:
        {
            PawnUnit_Print(PAWNUNIT_STR_TESTING_SUSPENDED);
            Suspended = true;
            
            // Stop.
            return false;
        }
        default:
        {
            PawnUnit_Print(PAWNUNIT_STR_INVALID_ACTION, action);
        }
    }
    
    // Continue.
    return true;
}

/**
 * Executes the function to run before every test case, if any.
 *
 * @param collection        Test collection reference.
 */
static stock PawnUnit_ExecuteBefore(TestCollection:collection)
{
    new Function:before = PawnUnit_GetBefore(collection);
    if (before != INVALID_FUNCTION)
    {
        Call_StartFunction(GetMyHandle(), before);
        Call_Finish();
    }
}

/**
 * Executes the function to run after every test case, if any.
 *
 * @param collection        Test collection reference.
 */
static stock PawnUnit_ExecuteAfter(TestCollection:collection)
{
    new Function:after = PawnUnit_GetAfter(collection);
    if (after != INVALID_FUNCTION)
    {
        Call_StartFunction(GetMyHandle(), after);
        Call_Finish();
    }
}

/**
 * Prints a message to the server console and logs.
 */
static stock PawnUnit_Print(const String:fmt[], any:...)
{
    decl String:msg[PAWNUNIT_MSG_LEN];
    msg[0] = 0;
    
    VFormat(msg, sizeof(msg), fmt, 2);
    
    PrintToServer(msg);
    LogMessage(msg);
}
