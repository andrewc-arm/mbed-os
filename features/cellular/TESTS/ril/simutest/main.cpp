/*
 * Copyright (c) 2018, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RIL_SIMULATOR
#error [NOT_SUPPORTED] This test is only for RIL_SIMULATOR. Skipping this build.
#endif

#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"

#include "mbed.h"
#include "Semaphore.h"
#include "Mutex.h"

#include "mbed_trace.h"
#define TRACE_GROUP "RILTEST"

#include "ril.h"
#include "RIL_Simulator.h"

static const RIL_RadioFunctions *ril = NULL;

static rtos::Mutex req_lock;
static rtos::Semaphore req_resp_lock(0);
static rtos::Semaphore req_resp_handled_lock(0);

static rtos::Mutex unsol_lock;
static rtos::Semaphore unsol_resp_lock(0);
static rtos::Semaphore unsol_resp_handled_lock(0);

const uint32_t WAIT_TIMEOUT = 1000; // 1sec

struct req_resp_t {
    RIL_Token t;
    RIL_Errno e;
    void *response;
    size_t responselen;
} req_resp;

struct unsol_resp_t {
    int unsolResponse;
    const void *data;
    size_t datalen;
} unsol_resp;


static void onRequestComplete(RIL_Token t, RIL_Errno e,
                              void *response, size_t responselen)
{
    // Prevent handling of multiple request responses at the same time
    req_lock.lock();

//    tr_info("onRequestComplete: t = 0x%x, e = %d, response = 0x%x, responselen = %d",
//        t, e, response, responselen);

    req_resp.t = t;
    req_resp.e = e;
    req_resp.response = response;
    req_resp.responselen = responselen;

    // Indicate test case about received request response
    TEST_ASSERT_EQUAL(osOK, req_resp_lock.release());

    // Wait until test case has handled the request response
    if (req_resp_handled_lock.wait(WAIT_TIMEOUT) != 1) {
        TEST_ASSERT_MESSAGE(0, "Request response handle timeout!");
    }

    // We are now ready to handle next request response
    req_lock.unlock();
}

// Wait for response to OnRequest call
static bool wait_req_resp()
{
    if (req_resp_lock.wait(WAIT_TIMEOUT) != 1) {
        TEST_ASSERT_MESSAGE(0, "Request response timeout!");
        return false;
    } else {
        return true;
    }
}

// Test case has handled the received OnRequestResponse
static void req_resp_handled()
{
    TEST_ASSERT_EQUAL(osOK, req_resp_handled_lock.release());
}

static void OnUnsolicitedResponse(int unsolResponse, const void *data, size_t datalen)
{
    // Prevent handling of multiple request responses at the same time
    unsol_lock.lock();

//    tr_info("OnUnsolicitedResponse: unsolResponse = %d, data = %d, datalen = %d",
//        unsolResponse, data, datalen);

    unsol_resp.unsolResponse = unsolResponse;
    unsol_resp.data = data;
    unsol_resp.datalen = datalen;

    // Indicate test case about received unsol response
    TEST_ASSERT_EQUAL(osOK, unsol_resp_lock.release());

    // Wait until test case has handled the unsol response
    if (unsol_resp_handled_lock.wait(WAIT_TIMEOUT) != 1) {
        TEST_ASSERT_MESSAGE(0, "Unsol response handle timeout!");
    }

    // We are now ready to handle next URC
    unsol_lock.unlock();
}

// Wait for unsolicited event
static bool wait_unsol_resp()
{
    if (unsol_resp_lock.wait(WAIT_TIMEOUT) != 1) {
        TEST_ASSERT_MESSAGE(0, "Unsolicited response timeout!");
        return false;
    } else {
        return true;
    }
}

// Test case has handled the received OnUnsolicitedResponse
static void unsol_resp_handled()
{
    TEST_ASSERT_EQUAL(osOK, unsol_resp_handled_lock.release());
}

static const RIL_Env ril_env = {
    onRequestComplete,
    OnUnsolicitedResponse,
    NULL,
    NULL
};

static void RIL_version_test()
{
    // We only support RIL v12 at the moment
    TEST_ASSERT_EQUAL(12, RIL_VERSION);
}

static void RIL_Init_test()
{
    ril = RIL_Init(&ril_env, 0, NULL);
    TEST_ASSERT(ril);

    wait_unsol_resp();
    TEST_ASSERT_EQUAL(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, unsol_resp.unsolResponse);
    unsol_resp_handled();

    wait_unsol_resp();
    TEST_ASSERT_EQUAL(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, unsol_resp.unsolResponse);
    unsol_resp_handled();
}


static void RIL_REQUEST_GET_SIM_STATUS_test()
{
    int token = 100;

    ril->onRequest(RIL_REQUEST_GET_SIM_STATUS, NULL, 0, &token);
    wait_req_resp();

    TEST_ASSERT_EQUAL(token, *(int *)req_resp.t);
    TEST_ASSERT_EQUAL(RIL_E_SUCCESS, req_resp.e);

    TEST_ASSERT_EQUAL(sizeof(RIL_CardStatus_v6), req_resp.responselen);

    RIL_CardStatus_v6 *sim = (RIL_CardStatus_v6 *)req_resp.response;
    TEST_ASSERT_EQUAL(RIL_CARDSTATE_PRESENT, sim->card_state);

    req_resp_handled();
}

static void RIL_REQUEST_SET_INITIAL_ATTACH_APN_test()
{
    /**
     * RIL_REQUEST_SET_INITIAL_ATTACH_APN
     *
     * Set an apn to initial attach network
     *
     * "data" is a const char **
     * ((const char **)data)[0] is the APN to connect if radio technology is LTE
     * ((const char **)data)[1] is the connection type to request must be one of the
     *                          PDP_type values in TS 27.007 section 10.1.1.
     *                          For example, "IP", "IPV6", "IPV4V6", or "PPP".
     * ((const char **)data)[2] is the PAP / CHAP auth type. Values:
     *                          0 => PAP and CHAP is never performed.
     *                          1 => PAP may be performed; CHAP is never performed.
     *                          2 => CHAP may be performed; PAP is never performed.
     *                          3 => PAP / CHAP may be performed - baseband dependent.
     * ((const char **)data)[3] is the username for APN, or NULL
     * ((const char **)data)[4] is the password for APN, or NULL
     */
    int token = 50;

    const char *apn = "iot";
    const char *pdp_type = "IP";
    const char *auth = "0";
    const char *username = "";
    const char *password = "";

    const char *data[5];
    data[0] = apn;
    data[1] = pdp_type;
    data[2] = auth;
    data[3] = username;
    data[4] = password;

    ril->onRequest(RIL_REQUEST_SET_INITIAL_ATTACH_APN, data, sizeof(data), &token);
    wait_req_resp();

    TEST_ASSERT_EQUAL(token, *(int *)req_resp.t);
    TEST_ASSERT_EQUAL(RIL_E_SUCCESS, req_resp.e);

    req_resp_handled();

    wait_unsol_resp();
    TEST_ASSERT_EQUAL(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, unsol_resp.unsolResponse);
    unsol_resp_handled();
}

static void RIL_REQUEST_DATA_REGISTRATION_STATE_test()
{
    int token = 101;

    ril->onRequest(RIL_REQUEST_DATA_REGISTRATION_STATE, NULL, 0, &token);
    wait_req_resp();

    TEST_ASSERT_EQUAL(token, *(int *)req_resp.t);
    TEST_ASSERT_EQUAL(RIL_E_SUCCESS, req_resp.e);

    const char **data = (const char **)req_resp.response;
    TEST_ASSERT_EQUAL(sizeof(char *) * 11, req_resp.responselen); // 11 * char*

    // TODO: Add more checks
    TEST_ASSERT_EQUAL_STRING("1", data[0]);
    TEST_ASSERT_EQUAL_STRING("14", data[3]); // RADIO_TECH_LTE = 14

    req_resp_handled();
}


static void RIL_REQUEST_RADIO_POWER_test()
{
    TEST_ASSERT_EQUAL(RADIO_STATE_OFF, ril->onStateRequest());

    int token = 102;

    int radio_power = 1;
    ril->onRequest(RIL_REQUEST_RADIO_POWER, &radio_power, sizeof(radio_power), &token);
    wait_req_resp();

    TEST_ASSERT_EQUAL(token, *(int *)req_resp.t);
    TEST_ASSERT_EQUAL(RIL_E_SUCCESS, req_resp.e);
    req_resp_handled();

    wait_unsol_resp();
    TEST_ASSERT_EQUAL(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, unsol_resp.unsolResponse);
    unsol_resp_handled();

    TEST_ASSERT_EQUAL(RADIO_STATE_ON, ril->onStateRequest());

    wait_unsol_resp();
    TEST_ASSERT_EQUAL(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, unsol_resp.unsolResponse);
    unsol_resp_handled();

    token = 103;
    ril->onRequest(RIL_REQUEST_GET_SIM_STATUS, NULL, 0, &token);
    wait_req_resp();

    TEST_ASSERT_EQUAL(token, *(int *)req_resp.t);
    TEST_ASSERT_EQUAL(RIL_E_SUCCESS, req_resp.e);

    TEST_ASSERT_EQUAL(sizeof(RIL_CardStatus_v6), req_resp.responselen);
    RIL_CardStatus_v6 *sim = (RIL_CardStatus_v6 *)req_resp.response;
    TEST_ASSERT_EQUAL(RIL_CARDSTATE_PRESENT, sim->card_state);

    req_resp_handled();
}


static void RIL_REQUEST_SETUP_DATA_CALL_test()
{
    /*
    * "data" is a const char **
    * ((const char **)data)[0] Radio technology to use: 0-CDMA, 1-GSM/UMTS, 2...
    *                          for values above 2 this is RIL_RadioTechnology + 2.
    * ((const char **)data)[1] is a RIL_DataProfile (support is optional)
    * ((const char **)data)[2] is the APN to connect to if radio technology is GSM/UMTS. This APN will
    *                          override the one in the profile. NULL indicates no APN overrride.
    * ((const char **)data)[3] is the username for APN, or NULL
    * ((const char **)data)[4] is the password for APN, or NULL
    * ((const char **)data)[5] is the PAP / CHAP auth type. Values:
    *                          0 => PAP and CHAP is never performed.
    *                          1 => PAP may be performed; CHAP is never performed.
    *                          2 => CHAP may be performed; PAP is never performed.
    *                          3 => PAP / CHAP may be performed - baseband dependent.
    * ((const char **)data)[6] is the connection type to request must be one of the
    *                          PDP_type values in TS 27.007 section 10.1.1.
    *                          For example, "IP", "IPV6", "IPV4V6", or "PPP".
    * ((const char **)data)[7] Optional connection property parameters, format to be defined.
    *
    * "response" is a RIL_Data_Call_Response_v11
    */
    int token = 104;

    const char *rat = "16"; // = RADIO_TECH_LTE + 2
    const char *apn = "internet";
    const char *username = "username";
    const char *password = "password";
    const char *pap_caph = "3";
    const char *pdp_type = "IP";

    const char *req[8] = {0};
    req[0] = rat;
    //req[1] = data_profile;
    req[2] = apn;
    req[3] = username;
    req[4] = password;
    req[5] = pap_caph;
    req[6] = pdp_type;
    //req[7] = optional;

    ril->onRequest(RIL_REQUEST_SETUP_DATA_CALL, &req, sizeof(req), &token);
    wait_req_resp();

    TEST_ASSERT_EQUAL(token, *(int *)req_resp.t);
    TEST_ASSERT_EQUAL(RIL_E_SUCCESS, req_resp.e);

    TEST_ASSERT_EQUAL(sizeof(RIL_Data_Call_Response_v11), req_resp.responselen);
    RIL_Data_Call_Response_v11 *resp = (RIL_Data_Call_Response_v11 *)req_resp.response;
    TEST_ASSERT(resp);

    TEST_ASSERT_EQUAL(PDP_FAIL_NONE, resp->status);
    TEST_ASSERT_EQUAL(0, resp->suggestedRetryTime);
    TEST_ASSERT_EQUAL(0, resp->cid);
    TEST_ASSERT_EQUAL(2, resp->active);
    TEST_ASSERT_EQUAL_STRING("IP", resp->type);
    TEST_ASSERT_EQUAL_STRING("ifname0", resp->ifname);
    TEST_ASSERT_EQUAL_STRING("10.0.0.1", resp->addresses);
    TEST_ASSERT_EQUAL_STRING("8.8.8.8", resp->dnses);
    TEST_ASSERT_EQUAL_STRING("10.0.0.99", resp->gateways);
    TEST_ASSERT_EQUAL_STRING("", resp->pcscf);
    TEST_ASSERT_EQUAL(1500, resp->mtu);

    req_resp_handled();

    wait_unsol_resp();
    TEST_ASSERT_EQUAL(RIL_UNSOL_DATA_CALL_LIST_CHANGED, unsol_resp.unsolResponse);
    TEST_ASSERT_EQUAL(sizeof(RIL_Data_Call_Response_v11), unsol_resp.datalen);
    RIL_Data_Call_Response_v11 *list = (RIL_Data_Call_Response_v11 *)unsol_resp.data;
    TEST_ASSERT(list);

    TEST_ASSERT_EQUAL(PDP_FAIL_NONE, list->status);
    TEST_ASSERT_EQUAL(0, list->suggestedRetryTime);
    TEST_ASSERT_EQUAL(0, list->cid);
    TEST_ASSERT_EQUAL(2, list->active);
    TEST_ASSERT_EQUAL_STRING("IP", list->type);
    TEST_ASSERT_EQUAL_STRING("ifname0", list->ifname);
    TEST_ASSERT_EQUAL_STRING("10.0.0.1", list->addresses);
    TEST_ASSERT_EQUAL_STRING("8.8.8.8", list->dnses);
    TEST_ASSERT_EQUAL_STRING("10.0.0.99", list->gateways);
    TEST_ASSERT_EQUAL_STRING("", list->pcscf);
    TEST_ASSERT_EQUAL(1500, list->mtu);

    unsol_resp_handled();
}

static void RIL_REQUEST_DATA_CALL_LIST_test()
{
    // NOTE! This test expects only 1 active PDP context, so will fail if executed after RIL_REQUEST_SETUP_DATA_CALL

    int token = 105;

    ril->onRequest(RIL_REQUEST_DATA_CALL_LIST, NULL, 0, &token);
    wait_req_resp();

    TEST_ASSERT_EQUAL(token, *(int *)req_resp.t);
    TEST_ASSERT_EQUAL(RIL_E_SUCCESS, req_resp.e);

    TEST_ASSERT_EQUAL(sizeof(RIL_Data_Call_Response_v11) * 1, req_resp.responselen);

    RIL_Data_Call_Response_v11 *list = (RIL_Data_Call_Response_v11 *)req_resp.response;
    TEST_ASSERT(list);

    RIL_Data_Call_Response_v11 &entry = list[0];
    TEST_ASSERT_EQUAL(PDP_FAIL_NONE, entry.status);
    TEST_ASSERT_EQUAL(0, entry.suggestedRetryTime);
    TEST_ASSERT_EQUAL(0, entry.cid);
    TEST_ASSERT_EQUAL(2, entry.active);
    TEST_ASSERT_EQUAL_STRING("IP", entry.type);
    TEST_ASSERT_EQUAL_STRING("ifname0", entry.ifname);
    TEST_ASSERT_EQUAL_STRING("10.0.0.1", entry.addresses);
    TEST_ASSERT_EQUAL_STRING("8.8.8.8", entry.dnses);
    TEST_ASSERT_EQUAL_STRING("10.0.0.99", entry.gateways);
    TEST_ASSERT_EQUAL_STRING("", entry.pcscf);
    TEST_ASSERT_EQUAL(1500, entry.mtu);

    req_resp_handled();
}

static void RIL_REQUEST_DEACTIVATE_DATA_CALL_test()
{
    int token = 106;
    /*
     * "data" is const char **
     * ((char**)data)[0] indicating CID
     * ((char**)data)[1] indicating Disconnect Reason
     *                   0 => No specific reason specified
     *                   1 => Radio shutdown requested
     *
     * "response" is NULL
     */

    char *cid = (char *)"0";
    char *reason = (char *)"0";
    char *req[2];
    req[0] = cid;
    req[1] = reason;

    ril->onRequest(RIL_REQUEST_DEACTIVATE_DATA_CALL, &req, sizeof(req), &token);
    wait_req_resp();

    TEST_ASSERT_EQUAL(token, *(int *)req_resp.t);
    TEST_ASSERT_EQUAL(RIL_E_SUCCESS, req_resp.e);

    req_resp_handled();

    // List should be empty
    token = 107;

    ril->onRequest(RIL_REQUEST_DATA_CALL_LIST, NULL, 0, &token);
    wait_req_resp();

    TEST_ASSERT_EQUAL(token, *(int *)req_resp.t);
    TEST_ASSERT_EQUAL(RIL_E_SUCCESS, req_resp.e);

    TEST_ASSERT_EQUAL(0, req_resp.responselen);

    req_resp_handled();
}

using namespace utest::v1;

static utest::v1::status_t greentea_failure_handler(const Case *const source, const failure_t reason)
{
    greentea_case_failure_abort_handler(source, reason);
    return STATUS_ABORT;
}

static Case cases[] = {
    Case("RIL_version", RIL_version_test, greentea_failure_handler),
    Case("RIL_Init", RIL_Init_test, greentea_failure_handler),              // Do NOT remove!
    Case("RIL_REQUEST_RADIO_POWER", RIL_REQUEST_RADIO_POWER_test, greentea_failure_handler),
    Case("RIL_REQUEST_GET_SIM_STATUS", RIL_REQUEST_GET_SIM_STATUS_test, greentea_failure_handler),
    Case("RIL_REQUEST_SET_INITIAL_ATTACH_APN", RIL_REQUEST_SET_INITIAL_ATTACH_APN_test, greentea_failure_handler),
    Case("RIL_REQUEST_DATA_REGISTRATION_STATE", RIL_REQUEST_DATA_REGISTRATION_STATE_test, greentea_failure_handler),
    Case("RIL_REQUEST_SETUP_DATA_CALL", RIL_REQUEST_SETUP_DATA_CALL_test, greentea_failure_handler),
    Case("RIL_REQUEST_DATA_CALL_LIST", RIL_REQUEST_DATA_CALL_LIST_test, greentea_failure_handler),
    Case("RIL_REQUEST_DEACTIVATE_DATA_CALL", RIL_REQUEST_DEACTIVATE_DATA_CALL_test, greentea_failure_handler),
};

static utest::v1::status_t test_setup(const size_t number_of_cases)
{
    GREENTEA_SETUP(10 * 60, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

static Specification specification(test_setup, cases);

int main()
{
    mbed_trace_init();
    return Harness::run(specification);
}
