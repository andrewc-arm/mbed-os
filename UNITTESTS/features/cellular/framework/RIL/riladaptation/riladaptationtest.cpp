/*
 * Copyright (c) 2019, Arm Limited and affiliates.
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

#include "gtest/gtest.h"
#include <string.h>
#include "RILAdaptation.h"
#include "RIL_CellularDevice.h"
#include "RIL_stub.h"

using namespace mbed;

// AStyle ignored as the definition is not clear due to preprocessor usage
// *INDENT-OFF*
class TestRILAdaptation : public testing::Test {
protected:

    void SetUp()
    {
        rad = new RILAdaptation();
        ASSERT_TRUE(rad != NULL);

    }

    void TearDown()
    {
        delete rad;
        rad = NULL;
    }

    RILAdaptation *rad;
};


static ril_token_t ril_token;
static int callback_counter = 0;
static RIL_Errno callback_ril_errno;
static void response_callback(ril_token_t *token, RIL_Errno err, void *response, size_t response_len)
{
    callback_counter++;
    // token is deleted in void RILAdaptation::request_complete so copy values for verifying
    ril_token.request_id = token->request_id;
    ril_token.token_id = token->token_id;
    callback_ril_errno = err;
}

// *INDENT-ON*
TEST_F(TestRILAdaptation, set_get_device)
{
    RIL_CellularDevice *dev = NULL;

    dev = rad->get_device();
    EXPECT_TRUE(dev == NULL);

    dev = new RIL_CellularDevice(*rad);

    RIL_CellularDevice *dev2 = rad->get_device();
    ASSERT_EQ(dev, dev2);

    delete dev;
}

TEST_F(TestRILAdaptation, before_init_ril_paths)
{
    const char *data  = "some_ data";
    ril_token_t *token = rad->send_request(1, (void *)data, strlen(data) + 1, callback(&response_callback), NULL, NULL);
    EXPECT_TRUE(token == NULL);

    // not initialized so return RADIO_STATE_UNAVAILABLE
    RIL_RadioState st = rad->get_radio_state();
    ASSERT_EQ(st, RADIO_STATE_UNAVAILABLE);

    const char *version = rad->get_version();
    EXPECT_TRUE(version == NULL);

    rad->cancel_request(token);
}

TEST_F(TestRILAdaptation, init_ril)
{
    nsapi_error_t err = rad->init_ril();
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);

    RIL_stub_values::bool_value = true;
    err = rad->init_ril();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    // third path
    err = rad->init_ril();
    ASSERT_EQ(err, NSAPI_ERROR_OK);
}

TEST_F(TestRILAdaptation, get_radio_state)
{
    ASSERT_EQ(NSAPI_ERROR_OK, rad->init_ril());

    RIL_RadioState st = rad->get_radio_state();
    ASSERT_EQ(st, RADIO_STATE_UNAVAILABLE);

    RIL_stub_values::radio_state = RADIO_STATE_ON;
    st = rad->get_radio_state();
    ASSERT_EQ(st, RADIO_STATE_ON);
}

TEST_F(TestRILAdaptation, send_request)
{
    ASSERT_EQ(NSAPI_ERROR_OK, rad->init_ril());

    RIL_stub_values::radio_state = RADIO_STATE_UNAVAILABLE;
    const char *data  = "some_ data";
    ril_token_t *token = rad->send_request(1, (void *)data, strlen(data) + 1, callback(&response_callback), NULL, NULL);
    EXPECT_TRUE(token == NULL);

    RIL_stub_values::radio_state = RADIO_STATE_ON;
    token = rad->send_request(5, (void *)data, strlen(data) + 1, callback(&response_callback), NULL, NULL);
    EXPECT_TRUE(token != NULL);
    delete token;
    token = NULL;

    // first token sent successfully
    ASSERT_EQ(ril_token.token_id, 1);
    ASSERT_EQ(ril_token.request_id, 5); // given request id
    ASSERT_STREQ(data, (const char *)RIL_stub_values::data_ptr);
    ASSERT_EQ(callback_counter, 1);
}

TEST_F(TestRILAdaptation, get_version)
{
    ASSERT_EQ(NSAPI_ERROR_OK, rad->init_ril());

    const char *version = rad->get_version();
    ASSERT_STREQ(version, "RIL Simulator v12"); // "RIL Simulator v12" is hardcoded in stub
}

TEST_F(TestRILAdaptation, cancel_request)
{
    ASSERT_EQ(NSAPI_ERROR_OK, rad->init_ril());

    const char *data  = "some_ data";
    ril_token_t *token = new ril_token_t;
    token->cb = callback(&response_callback);
    RIL_stub_values::err_value = RIL_E_CANCELLED;
    rad->cancel_request(token);
    delete token;
    ASSERT_EQ(callback_ril_errno, RIL_E_CANCELLED);
}

TEST_F(TestRILAdaptation, other_ril_callbacks)
{
    ASSERT_EQ(NSAPI_ERROR_OK, rad->init_ril());

    RIL_CellularDevice *dev = new RIL_CellularDevice(*rad);

    RIL_stub *ril_stub = RIL_stub::GetInstance();
    EXPECT_TRUE(ril_stub != NULL);
    ril_stub->_env->OnUnsolicitedResponse(1, NULL, 0);
    ril_stub->_env->RequestTimedCallback(0, 0, 0);

    ril_token_t *token = new ril_token_t;
    ril_stub->_env->OnRequestAck(token);
    delete token;

    delete dev;
}
