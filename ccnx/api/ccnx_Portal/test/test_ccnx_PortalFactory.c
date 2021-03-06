/*
 * Copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX OR PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ################################################################################
 * #
 * # PATENT NOTICE
 * #
 * # This software is distributed under the BSD 2-clause License (see LICENSE
 * # file).  This BSD License does not make any patent claims and as such, does
 * # not act as a patent grant.  The purpose of this section is for each contributor
 * # to define their intentions with respect to intellectual property.
 * #
 * # Each contributor to this source code is encouraged to state their patent
 * # claims and licensing mechanisms for any contributions made. At the end of
 * # this section contributors may each make their own statements.  Contributor's
 * # claims and grants only apply to the pieces (source code, programs, text,
 * # media, etc) that they have contributed directly to this software.
 * #
 * # There is no guarantee that this section is complete, up to date or accurate. It
 * # is up to the contributors to maintain their portion of this section and up to
 * # the user of the software to verify any claims herein.
 * #
 * # Do not remove this header notification.  The contents of this section must be
 * # present in all distributions of the software.  You may only modify your own
 * # intellectual property statements.  Please provide contact information.
 *
 * - Palo Alto Research Center, Inc
 * This software distribution does not grant any rights to patents owned by Palo
 * Alto Research Center, Inc (PARC). Rights to these patents are available via
 * various mechanisms. As of January 2016 PARC has committed to FRAND licensing any
 * intellectual property used by its contributions to this software. You may
 * contact PARC at cipo@parc.com for more information or visit http://www.ccnx.org
 */
/**
 * @header <#Headline Name#>
 * @abstract <#Abstract#>
 * @discussion
 *     <#Discussion#>
 *
 * @author Scott, Glenn <Glenn.Scott@parc.com>, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include "../ccnx_PortalFactory.c"

#include <stdio.h>
#include <unistd.h>

#include <LongBow/unit-test.h>
#include <LongBow/debugging.h>

#include <parc/algol/parc_SafeMemory.h>
#include <parc/algol/parc_Memory.h>
#include <parc/security/parc_IdentityFile.h>

#include <ccnx/transport/test_tools/bent_pipe.h>

#include <parc/security/parc_Pkcs12KeyStore.h>

LONGBOW_TEST_RUNNER(test_ccnx_PortalFactory /*, .requires="FeatureLongBowSubProcess"*/)
{
    LONGBOW_RUN_TEST_FIXTURE(CreateAcquireRelease);
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Errors);
}

LONGBOW_TEST_RUNNER_SETUP(test_ccnx_PortalFactory)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);

    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_RUNNER_TEARDOWN(test_ccnx_PortalFactory)
{
//    LongBowSubProcess *metis = longBowTestRunner_GetClipBoardData(testRunner);
//    longBowSubProcess_Display(0, metis);
//    longBowSubProcess_Destroy(&metis);

    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(CreateAcquireRelease)
{
    LONGBOW_RUN_TEST_CASE(CreateAcquireRelease, ccnxPortalFactory_Create);
    LONGBOW_RUN_TEST_CASE(CreateAcquireRelease, ccnxPortalFactory_AcquireRelease);
}

LONGBOW_TEST_FIXTURE_SETUP(CreateAcquireRelease)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(CreateAcquireRelease)
{
    unlink("ccnxPortalFactory_keystore");

    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(CreateAcquireRelease, ccnxPortalFactory_Create)
{
    const char *keystoreName = "ccnxPortalFactory_keystore";

    parcSecurity_Init();

    bool success = parcPkcs12KeyStore_CreateFile(keystoreName, "keystore_password", "consumer", 1024, 30);
    assertTrue(success, "parcPkcs12KeyStore_CreateFile('%s', 'keystore_password') failed.", keystoreName);

    PARCIdentityFile *identityFile = parcIdentityFile_Create(keystoreName, "keystore_password");
    PARCIdentity *identity = parcIdentity_Create(identityFile, PARCIdentityFileAsPARCIdentity);

    CCNxPortalFactory *factory = ccnxPortalFactory_Create(identity);
    parcIdentityFile_Release(&identityFile);
    parcIdentity_Release(&identity);

    ccnxPortalFactory_Release(&factory);

    parcSecurity_Fini();
}

LONGBOW_TEST_CASE(CreateAcquireRelease, ccnxPortalFactory_AcquireRelease)
{
    const char *keystoreName = "ccnxPortalFactory_keystore";

    parcSecurity_Init();

    bool success = parcPkcs12KeyStore_CreateFile(keystoreName, "keystore_password", "consumer", 1024, 30);
    assertTrue(success, "parcPkcs12KeyStore_CreateFile('%s', 'keystore_password') failed.", keystoreName);

    PARCIdentityFile *identityFile = parcIdentityFile_Create(keystoreName, "keystore_password");
    PARCIdentity *identity = parcIdentity_Create(identityFile, PARCIdentityFileAsPARCIdentity);

    CCNxPortalFactory *factory = ccnxPortalFactory_Create(identity);
    parcIdentityFile_Release(&identityFile);
    parcIdentity_Release(&identity);

    CCNxPortalFactory *reference = ccnxPortalFactory_Acquire(factory);
    assertTrue(factory == reference, "Expected Acquire to return its argument.");

    ccnxPortalFactory_Release(&factory);
    ccnxPortalFactory_Release(&reference);

    parcSecurity_Fini();
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxPortalFactory_GetIdentity);
    LONGBOW_RUN_TEST_CASE(Global, ccnxPortalFactory_GetKeyId);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    unlink("ccnxPortalFactory_keystore");

    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, ccnxPortalFactory_GetIdentity)
{
    const char *keystoreName = "ccnxPortalFactory_keystore";

    parcSecurity_Init();
    bool success = parcPkcs12KeyStore_CreateFile(keystoreName, "keystore_password", "consumer", 1024, 30);
    assertTrue(success, "parcPkcs12KeyStore_CreateFile('%s', 'keystore_password') failed.", keystoreName);

    PARCIdentityFile *identityFile = parcIdentityFile_Create(keystoreName, "keystore_password");
    PARCIdentity *identity = parcIdentity_Create(identityFile, PARCIdentityFileAsPARCIdentity);

    CCNxPortalFactory *factory = ccnxPortalFactory_Create(identity);

    const PARCIdentity *actual = ccnxPortalFactory_GetIdentity(factory);

    assertTrue(identity == actual, "Expected the result to be the same as provided to the constructor");
   
    parcIdentityFile_Release(&identityFile);
    parcIdentity_Release(&identity);

    ccnxPortalFactory_Release(&factory);
    parcSecurity_Fini();
}

LONGBOW_TEST_CASE(Global, ccnxPortalFactory_GetKeyId)
{
    const char *keystoreName = "ccnxPortalFactory_keystore";

    parcSecurity_Init();
    bool success = parcPkcs12KeyStore_CreateFile(keystoreName, "keystore_password", "consumer", 1024, 30);
    assertTrue(success, "parcPkcs12KeyStore_CreateFile('%s', 'keystore_password') failed.", keystoreName);

    PARCIdentityFile *identityFile = parcIdentityFile_Create(keystoreName, "keystore_password");
    PARCIdentity *identity = parcIdentity_Create(identityFile, PARCIdentityFileAsPARCIdentity);

    CCNxPortalFactory *factory = ccnxPortalFactory_Create(identity);

    const PARCKeyId *actual = ccnxPortalFactory_GetKeyId(factory);

    PARCSigner *signer = parcIdentity_CreateSigner(identity);
    PARCKeyId *expected = parcSigner_CreateKeyId(signer);
    parcSigner_Release(&signer);

    assertTrue(parcKeyId_Equals(expected, actual), "KeyIds not equal");

    parcKeyId_Release(&expected);

    ccnxPortalFactory_Release(&factory);
   
    parcIdentityFile_Release(&identityFile);
    parcIdentity_Release(&identity);

    parcSecurity_Fini();
}

LONGBOW_TEST_FIXTURE(Errors)
{
    LONGBOW_RUN_TEST_CASE(Errors, ccnxPortalFactory_Create_NULL_Identity);
}

LONGBOW_TEST_FIXTURE_SETUP(Errors)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Errors)
{
    unlink("ccnxPortalFactory_keystore");

    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE_EXPECTS(Errors, ccnxPortalFactory_Create_NULL_Identity, .event = &LongBowTrapInvalidValue)
{
    CCNxPortalFactory *factory = ccnxPortalFactory_Create(NULL);

    ccnxPortalFactory_Release(&factory);
}

int
main(int argc, char *argv[argc])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(test_ccnx_PortalFactory);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
