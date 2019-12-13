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
#include "RIL_CellularInformation.h"
#include "RIL_CellularDevice.h"
#include "RIL_stub.h"
#include "RILAdaptation_stub.h"

using namespace mbed;

// AStyle ignored as the definition is not clear due to preprocessor usage
// *INDENT-OFF*
class TestRIL_CellularInformation : public testing::Test {
protected:

    void SetUp()
    {
        RILAdaptation_stub::return_null_token = false;
        RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
        RILAdaptation_stub::cb_response = 0;
        RILAdaptation_stub::cb_response_len = 0;

        ril = new RILAdaptation();
        ASSERT_TRUE(ril != NULL);
        dev =  new RIL_CellularDevice(*ril);
        ASSERT_TRUE(dev != NULL);
        info = new RIL_CellularInformation(*dev);
        ASSERT_TRUE(info != NULL);
    }

    void TearDown()
    {
        delete info;
        info = NULL;

        delete dev;
        dev = NULL;

        delete ril;
        ril = NULL;
    }

    RILAdaptation *ril;
    RIL_CellularDevice *dev;
    RIL_CellularInformation *info;
};

class test_RIL_CellularInformation : public RIL_CellularInformation {
public:
    test_RIL_CellularInformation(RIL_CellularDevice &device) : RIL_CellularInformation(device) {};

    void unsolicited_response(int response_id, const void *data, size_t data_len)
    {
        RIL_CellularInformation::unsolicited_response(response_id, data, data_len);
    }
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
TEST_F(TestRIL_CellularInformation, get_manufacturer)
{
    char buf[10];
    nsapi_error_t err = info->get_manufacturer(buf, 10);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestRIL_CellularInformation, get_model)
{
    char buf[10];
    nsapi_error_t err = info->get_model(buf, 10);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestRIL_CellularInformation, get_iccid)
{
    char buf[10];
    nsapi_error_t err = info->get_iccid(buf, 10);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);
}

TEST_F(TestRIL_CellularInformation, get_revision)
{
    char buf[10];
    nsapi_error_t err = info->get_revision(buf, 0);
    ASSERT_EQ(err, NSAPI_ERROR_PARAMETER);

    err = info->get_revision(0, 10);
    ASSERT_EQ(err, NSAPI_ERROR_PARAMETER);

    err = info->get_revision(buf, 10);
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);

    const char *rev_buf = "v12";
    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    RILAdaptation_stub::cb_response = (void *)rev_buf;
    RILAdaptation_stub::cb_response_len = 10;
    err = info->get_revision(buf, 10);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_STREQ(rev_buf, buf);
}

TEST_F(TestRIL_CellularInformation, get_imsi)
{
    char buf[10];
    nsapi_error_t err = info->get_imsi(buf, 0);
    ASSERT_EQ(err, NSAPI_ERROR_PARAMETER);

    err = info->get_imsi(0, 10);
    ASSERT_EQ(err, NSAPI_ERROR_PARAMETER);

    err = info->get_imsi(buf, 10);
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);

    const char *rev_buf = "123456";
    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    RILAdaptation_stub::cb_response = (void *)rev_buf;
    RILAdaptation_stub::cb_response_len = 10;
    err = info->get_imsi(buf, 10);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_STREQ(rev_buf, buf);
}

TEST_F(TestRIL_CellularInformation, get_serial_number)
{
    char buf[10];
    nsapi_error_t err = info->get_serial_number(buf, 0, CellularInformation::IMEI);
    ASSERT_EQ(err, NSAPI_ERROR_PARAMETER);

    err = info->get_serial_number(0, 10, CellularInformation::IMEI);
    ASSERT_EQ(err, NSAPI_ERROR_PARAMETER);

    err = info->get_serial_number(buf, 10, CellularInformation::IMEI);
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);

    err = info->get_serial_number(buf, 10, CellularInformation::SN);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);

    err = info->get_serial_number(buf, 10, CellularInformation::SVN);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);

    const char *rev_buf = "123456";
    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    RILAdaptation_stub::cb_response = (void *)&rev_buf;
    RILAdaptation_stub::cb_response_len = 7;
    err = info->get_serial_number(buf, 10, CellularInformation::IMEI);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_STREQ(rev_buf, buf);
}

TEST_F(TestRIL_CellularInformation, unsolicited_response)
{
    test_RIL_CellularInformation *test_info = new test_RIL_CellularInformation(*dev);
    test_info->unsolicited_response(5, 0, 0);

    delete test_info;
}
