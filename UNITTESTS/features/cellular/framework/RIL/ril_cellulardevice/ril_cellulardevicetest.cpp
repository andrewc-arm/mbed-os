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
#include "RIL_CellularNetwork.h"
#include "RIL_CellularDevice.h"
#include "RIL_stub.h"
#include "RILAdaptation_stub.h"
#include "FileHandle_stub.h"
#include "ConditionVariable_stub.h"

using namespace mbed;
using namespace events;

// AStyle ignored as the definition is not clear due to preprocessor usage
// *INDENT-OFF*
class TestRIL_CellularDevice : public testing::Test {
protected:

    void SetUp()
    {
        RILAdaptation_stub::return_null_token = false;
        RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
        RILAdaptation_stub::cb_response = 0;
        RILAdaptation_stub::cb_response_len = 0;
        RILAdaptation_stub::nsapi_error_value = NSAPI_ERROR_OK;
        RILAdaptation_stub::radio_state = RADIO_STATE_ON;
    }

    void TearDown()
    {
    }
};

static const intptr_t cellular_properties[RIL_CellularDevice::PROPERTY_MAX] = {
    1,  // C_EREG
    0,  // C_GREG
    0,  // C_REG
    1,  // PROPERTY_IPV4_STACK
    1,  // PROPERTY_IPV6_STACK
    1,  // PROPERTY_IPV4V6_STACK
    1,  // PROPERTY_NON_IP_PDP_TYPE
};

class test_device : public RIL_CellularDevice {
public:
    test_device(RILAdaptation &ril) : RIL_CellularDevice(ril) {
        _property_array = cellular_properties;
    }
    virtual void unsolicited_response(int response_id, const void *data, size_t data_len)
    {
        RIL_CellularDevice::unsolicited_response(response_id, data, data_len);
    }
};

static void device_callback(nsapi_event_t event, intptr_t ptr)
{
}

static void ready_callback(void)
{
}

class test_device2 : public RIL_CellularDevice {
public:
    test_device2(RILAdaptation &ril) : RIL_CellularDevice(ril)
    {
        expected_token_id = -1;
        expected_request_id = -1;
    }

    void request_response(ril_token_t *token, RIL_Errno err, void *response, size_t response_len)
    {
        token->cond_mutex->lock();

        if (expected_token_id != -1) {
            ASSERT_EQ(expected_token_id, token->token_id);
        }
        if (expected_request_id != -1) {
            ASSERT_EQ(expected_request_id, token->request_id);
        }

        token->response_error = NSAPI_ERROR_OK;
        token->cond_mutex->unlock();
    }

    nsapi_error_t test_send_request(int request_id, char *buf, size_t buf_size)
    {
        nsapi_error_t ret = lock_and_send_request(request_id, buf, buf_size, callback(this, &test_device2::request_response));

        if (ret == NSAPI_ERROR_OK) {
            // copy buf?
        }

        return ret;
    }

    int expected_token_id;
    int expected_request_id;
};

// *INDENT-ON*
TEST_F(TestRIL_CellularDevice, create)
{
    RILAdaptation ril;
    RIL_CellularDevice *dev = new RIL_CellularDevice(ril);
    delete dev;

    test_device *dev2 = new test_device(ril);
    delete dev2;
}

TEST_F(TestRIL_CellularDevice, get_property)
{
    RILAdaptation ril;
    test_device *dev = new test_device(ril);

    intptr_t prop = dev->get_property(RIL_CellularDevice::PROPERTY_C_EREG);
    ASSERT_EQ(prop, 1);

    prop = dev->get_property(RIL_CellularDevice::PROPERTY_C_REG);
    ASSERT_EQ(prop, 0);

    delete dev;
}

TEST_F(TestRIL_CellularDevice, hard_power_on)
{
    RILAdaptation ril;
    test_device *dev = new test_device(ril);

    nsapi_error_t err = dev->hard_power_on();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    RILAdaptation_stub::nsapi_error_value = NSAPI_ERROR_UNSUPPORTED;
    err = dev->hard_power_on();
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);

    delete dev;
}

TEST_F(TestRIL_CellularDevice, hard_power_off)
{
    RILAdaptation ril;
    test_device *dev = new test_device(ril);

    nsapi_error_t err = dev->hard_power_off();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    RILAdaptation_stub::nsapi_error_value = NSAPI_ERROR_UNSUPPORTED;
    err = dev->hard_power_off();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    delete dev;
}

TEST_F(TestRIL_CellularDevice, soft_power_on)
{
    RILAdaptation ril;
    test_device *dev = new test_device(ril);

    nsapi_error_t err = dev->soft_power_on();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    err = dev->soft_power_on();
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);

    delete dev;
}

TEST_F(TestRIL_CellularDevice, soft_power_off)
{
    RILAdaptation ril;
    test_device *dev = new test_device(ril);

    nsapi_error_t err = dev->soft_power_off();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    err = dev->soft_power_off();
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);

    delete dev;
}

TEST_F(TestRIL_CellularDevice, set_pin)
{
    RILAdaptation ril;
    test_device *dev = new test_device(ril);

    nsapi_error_t err = dev->set_pin(NULL);
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    err = dev->set_pin("1234");
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    delete dev;
}

TEST_F(TestRIL_CellularDevice, get_sim_state)
{
    RILAdaptation ril;
    test_device *dev = new test_device(ril);

    CellularDevice::SimState sim_state = CellularDevice::SimStateUnknown;
    nsapi_error_t err = dev->get_sim_state(sim_state);
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);
    ASSERT_EQ(sim_state, CellularDevice::SimStateUnknown);

    RILAdaptation_stub::cb_err = RIL_E_GENERIC_FAILURE;
    err = dev->get_sim_state(sim_state);
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);
    ASSERT_EQ(sim_state, CellularDevice::SimStateUnknown);


    RIL_CardStatus_v6 card_status;
    card_status.card_state = RIL_CARDSTATE_PRESENT;

    RILAdaptation_stub::cb_err = RIL_E_SUCCESS;
    RILAdaptation_stub::cb_response = &card_status;
    RILAdaptation_stub::cb_response_len = sizeof(RIL_CardStatus_v6);
    RILAdaptation_stub::radio_state = RADIO_STATE_SIM_READY;

    err = dev->get_sim_state(sim_state);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_EQ(sim_state, CellularDevice::SimStateReady);

    card_status.card_state = RIL_CARDSTATE_ABSENT;
    RILAdaptation_stub::cb_response = &card_status;
    err = dev->get_sim_state(sim_state);
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);
    ASSERT_EQ(sim_state, CellularDevice::SimStateReady);

    card_status.card_state = RIL_CARDSTATE_PRESENT;
    RILAdaptation_stub::cb_response = &card_status;
    RILAdaptation_stub::radio_state = RADIO_STATE_OFF;
    err = dev->get_sim_state(sim_state);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_EQ(sim_state, CellularDevice::SimStateUnknown);

    RILAdaptation_stub::radio_state = RADIO_STATE_RUIM_LOCKED_OR_ABSENT;
    err = dev->get_sim_state(sim_state);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_EQ(sim_state, CellularDevice::SimStatePinNeeded);

    RILAdaptation_stub::radio_state = (RIL_RadioState)99;
    err = dev->get_sim_state(sim_state);
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    ASSERT_EQ(sim_state, CellularDevice::SimStateUnknown);

    delete dev;
}

TEST_F(TestRIL_CellularDevice, create_context)
{
    RILAdaptation ril;
    test_device *dev = new test_device(ril);

    CellularContext *ctx = dev->create_context(NULL);
    ASSERT_TRUE(ctx != NULL);

    CellularContext *ctx1 = dev->create_context(NULL, "internet", true);
    ASSERT_TRUE(ctx1 != NULL);
    ASSERT_TRUE(ctx != ctx1);

    CellularContext *ctx2 = dev->create_context(NULL, "internet", true, true);
    ASSERT_TRUE(ctx2 != NULL);
    ASSERT_TRUE(ctx2 != ctx1);

#if (DEVICE_SERIAL && DEVICE_INTERRUPTIN)
    UARTSerial *serial = new UARTSerial(NC, NC);
    CellularContext *ctx3 = dev->create_context(serial, "internet", NC);
    ASSERT_TRUE(ctx3 == NULL);

    CellularContext *ctx4 = dev->create_context(serial, "internet", NC, true, true, true);
    ASSERT_TRUE(ctx4 == NULL);

    delete serial;
#endif

    delete dev;
}

TEST_F(TestRIL_CellularDevice, delete_context)
{
    RILAdaptation ril;
    test_device *dev = new test_device(ril);

    CellularContext *ctx = dev->create_context(NULL);
    ASSERT_TRUE(ctx != NULL);
    CellularContext *ctx_list = dev->get_context_list();
    ASSERT_TRUE(ctx_list != NULL);

    dev->delete_context(ctx);
    ctx_list = dev->get_context_list();
    ASSERT_TRUE(ctx_list == NULL);

    ctx = dev->create_context(NULL);
    ASSERT_TRUE(ctx != NULL);
    ASSERT_TRUE(ctx->_next == NULL);

    CellularContext *ctx2 = dev->create_context(NULL);
    ASSERT_TRUE(ctx2 != NULL);
    ASSERT_TRUE(ctx2->_next == NULL);

    dev->delete_context(ctx);
    ctx_list = dev->get_context_list();
    ASSERT_TRUE(ctx_list != NULL);

    dev->delete_context(ctx2);
    ctx_list = dev->get_context_list();
    ASSERT_TRUE(ctx_list == 0);

    ctx = dev->create_context(NULL);
    ASSERT_TRUE(ctx != NULL);
    ctx_list = dev->get_context_list();
    ASSERT_TRUE(ctx_list != NULL);

    ctx2 = dev->create_context(NULL);
    ASSERT_TRUE(ctx2 != NULL);
    ASSERT_TRUE(ctx2->_next == NULL);

    CellularContext *ctx3 = dev->create_context(NULL);
    ASSERT_TRUE(ctx3 != NULL);
    ASSERT_TRUE(ctx3->_next == NULL);

    dev->delete_context(0);

    dev->delete_context(ctx3);
    ctx_list = dev->get_context_list();
    ASSERT_TRUE(ctx_list != NULL);

    dev->delete_context(ctx2);
    ctx_list = dev->get_context_list();
    ASSERT_TRUE(ctx_list != NULL);

    dev->delete_context(ctx);
    ctx_list = dev->get_context_list();
    ASSERT_TRUE(ctx_list == NULL);

    delete dev;
}

TEST_F(TestRIL_CellularDevice, open_close_network)
{
    RILAdaptation ril;
    test_device *dev = new test_device(ril);
    CellularNetwork *nw = dev->open_network();
    ASSERT_TRUE(nw);
    dev->close_network();

    nw = dev->open_network();
    ASSERT_TRUE(nw);
    CellularNetwork *nw2 = dev->open_network();
    ASSERT_TRUE(nw2);
    ASSERT_TRUE(nw2 == nw);
    CellularNetwork *nw3 = dev->open_network();
    ASSERT_TRUE(nw3);
    ASSERT_TRUE(nw3 == nw);
    dev->close_network();
    dev->close_network();
    dev->close_network();

#if (DEVICE_SERIAL && DEVICE_INTERRUPTIN)
    UARTSerial *serial = new UARTSerial(NC, NC);
    nw2 = dev->open_network(serial);
    ASSERT_TRUE(nw2);
    delete serial;
#endif

    delete dev;
}

TEST_F(TestRIL_CellularDevice, open__close_sms)
{
    RILAdaptation ril;
    test_device *dev = new test_device(ril);
    CellularSMS *sms = dev->open_sms();
    ASSERT_TRUE(sms == NULL);
    dev->close_sms();

#if (DEVICE_SERIAL && DEVICE_INTERRUPTIN)
    UARTSerial *serial = new UARTSerial(NC, NC);
    CellularSMS *sms2 = dev->open_sms(serial);
    ASSERT_TRUE(sms2 == NULL);
    ASSERT_TRUE(sms2 == sms);

    delete serial;
#endif

    delete dev;
}

TEST_F(TestRIL_CellularDevice, open_close_information)
{
    RILAdaptation ril;
    test_device *dev = new test_device(ril);
    CellularInformation *info = dev->open_information();
    ASSERT_TRUE(info != NULL);
    dev->close_information();

    info = dev->open_information();
    ASSERT_TRUE(info);
    CellularInformation *info2 = dev->open_information();
    ASSERT_TRUE(info2);
    ASSERT_TRUE(info2 == info);
    CellularInformation *info3 = dev->open_information();
    ASSERT_TRUE(info3);
    ASSERT_TRUE(info3 == info);
    dev->close_information();
    dev->close_information();
    dev->close_information();

#if (DEVICE_SERIAL && DEVICE_INTERRUPTIN)
    UARTSerial *serial = new UARTSerial(NC, NC);
    info2 = dev->open_information(serial);
    ASSERT_TRUE(info2);

    delete serial;
#endif

    delete dev;
}

TEST_F(TestRIL_CellularDevice, set_timeout)
{
    RILAdaptation ril;
    RIL_CellularDevice *dev = new RIL_CellularDevice(ril);
    dev->set_timeout(5000);
    delete dev;
}

TEST_F(TestRIL_CellularDevice, modem_debug_on)
{
    RILAdaptation ril;
    RIL_CellularDevice *dev = new RIL_CellularDevice(ril);
    dev->modem_debug_on(true);
    dev->modem_debug_on(false);
    delete dev;
}

TEST_F(TestRIL_CellularDevice, init)
{
    RILAdaptation ril;
    RIL_CellularDevice *dev = new RIL_CellularDevice(ril);
    nsapi_error_t err = dev->init();
    ASSERT_EQ(err, NSAPI_ERROR_OK);
    delete dev;
}

TEST_F(TestRIL_CellularDevice, is_ready)
{
    RILAdaptation ril;
    RIL_CellularDevice *dev = new RIL_CellularDevice(ril);
    nsapi_error_t err = dev->is_ready();
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    RILAdaptation_stub::radio_state = RADIO_STATE_OFF;
    err = dev->is_ready();
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);

    delete dev;
}

TEST_F(TestRIL_CellularDevice, set_ready_cb)
{
    RILAdaptation ril;
    RIL_CellularDevice *dev = new RIL_CellularDevice(ril);
    dev->set_ready_cb(NULL);
    dev->set_ready_cb(ready_callback);

    delete dev;
}

TEST_F(TestRIL_CellularDevice, set_power_save_mode)
{
    RILAdaptation ril;
    RIL_CellularDevice *dev = new RIL_CellularDevice(ril);
    nsapi_error_t err = dev->set_power_save_mode(0, 0);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);

    delete dev;
}

TEST_F(TestRIL_CellularDevice, get_at_handler)
{
    RILAdaptation ril;
    RIL_CellularDevice *dev = new RIL_CellularDevice(ril);
    ATHandler *at = dev->get_at_handler();
    ASSERT_TRUE(at == NULL);

    delete dev;
}

TEST_F(TestRIL_CellularDevice, release_at_handler)
{
    RILAdaptation ril;
    RIL_CellularDevice *dev = new RIL_CellularDevice(ril);
    EventQueue que;
    FileHandle_stub fh1;
    ATHandler *at = new ATHandler(&fh1, que, 0, ",");

    nsapi_error_t err = dev->release_at_handler(at);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);

    err = dev->release_at_handler(0);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);

    delete at;
    delete dev;
}

TEST_F(TestRIL_CellularDevice, get_context_list)
{
    RILAdaptation ril;
    RIL_CellularDevice *dev = new RIL_CellularDevice(ril);
    CellularContext *ctx = dev->get_context_list();
    ASSERT_TRUE(ctx == NULL);

    CellularContext *ctx2 = dev->create_context(NULL);
    ASSERT_TRUE(ctx2 != NULL);
    ASSERT_TRUE(ctx2->_next == NULL);

    ctx = dev->get_context_list();
    ASSERT_TRUE(ctx == ctx2);

    dev->delete_context(ctx2);
    ctx = dev->get_context_list();
    ASSERT_TRUE(ctx == NULL);

    delete dev;
}

TEST_F(TestRIL_CellularDevice, send_at_command)
{
    RILAdaptation ril;
    RIL_CellularDevice *dev = new RIL_CellularDevice(ril);
    nsapi_error_t err = dev->send_at_command(0, 0);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);

    err = dev->send_at_command("AT+CPIN?", 8);
    ASSERT_EQ(err, NSAPI_ERROR_UNSUPPORTED);

    delete dev;
}

TEST_F(TestRIL_CellularDevice, unsolicited_response)
{
    RILAdaptation ril;
    test_device *dev = new test_device(ril);

    dev->unsolicited_response(1, 0, 0);

    CellularInformation *info = dev->open_information();
    ASSERT_TRUE(info);
    CellularNetwork *nw = dev->open_network();
    ASSERT_TRUE(nw);
    CellularContext *ctx = dev->create_context(NULL);
    ASSERT_TRUE(ctx);
    CellularContext *ctx2 = dev->create_context(NULL);
    ASSERT_TRUE(ctx2);
    dev->set_ready_cb(ready_callback);
    dev->unsolicited_response(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, 0, 0);

    delete dev;
}

TEST_F(TestRIL_CellularDevice, lock_and_send_return_null)
{
    RILAdaptation ril;
    test_device2 *unit = new test_device2(ril);

    char buf[50];
    RILAdaptation_stub::return_null_token = true;
    nsapi_error_t err = unit->test_send_request(1, buf, sizeof(buf));
    ASSERT_EQ(err, NSAPI_ERROR_DEVICE_ERROR);
    RILAdaptation_stub::return_null_token = false;

    delete unit;
}

TEST_F(TestRIL_CellularDevice, lock_and_send_return_timeout)
{
    RILAdaptation ril;
    test_device2 *unit = new test_device2(ril);

    char buf[50];
    ConditionVariable_stub::time_out = true;
    nsapi_error_t err = unit->test_send_request(1, buf, sizeof(buf));
    ASSERT_EQ(err, NSAPI_ERROR_TIMEOUT);

    delete unit;
}

TEST_F(TestRIL_CellularDevice, lock_and_send_return)
{
    RILAdaptation ril;
    test_device2 *unit = new test_device2(ril);
    EXPECT_TRUE(unit != NULL);

    ConditionVariable_stub::time_out = false;

    RILAdaptation_stub::ril_token_id = 12;
    unit->expected_token_id = RILAdaptation_stub::ril_token_id + 1; // Stub will increase token_id by 1 for each req

    unit->expected_request_id = 8;

    char buf[50];
    nsapi_error_t err = unit->test_send_request(8, buf, sizeof(buf));
    ASSERT_EQ(err, NSAPI_ERROR_OK);

    delete unit;
}
